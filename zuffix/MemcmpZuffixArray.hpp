#pragma once

#include <sux/support/common.hpp>
#include <sux/util/Vector.hpp>

#include "util/LInterval.hpp"
#include "util/LinearProber.hpp"
#include "util/common.hpp"

#include <chrono>

namespace zarr {
using ::sux::util::AllocType;
using ::sux::util::Vector;

template <typename T, template <typename U, AllocType AT> class RH, AllocType AT = MALLOC> class MemcmpZuffixArray {
  private:
	std::span<const T> text;
	std::span<const T> pattern;

	Vector<size_t, AT> sa;
	Vector<ssize_t, AT> lcp;
	Vector<size_t, AT> ct;

	LinearProber<typename RH<T, AT>::signature_t, LInterval<size_t>, AT> z;
	size_t maxnlen = 0, maxhlen = 0;

	RH<T, AT> hpattern;

  public:
	MemcmpZuffixArray() {}

	MemcmpZuffixArray(std::span<const T> string)
		: text(std::move(string)),
		  sa(SAConstructByGrebnovSAIS<T, AT>(text)),
		  lcp(LCPConstructByKarkkainenPsi<T, AT>(text, sa)),
		  ct(CTConstructByAbouelhoda<AT>(lcp)) {
		assert(text.data()[text.size() - 1] == std::numeric_limits<T>::max() && "Missing $-terminator");
		// z.resize(ceil_pow2(text.size()) << 1); // TODO: tweak me to improve construction performance
		RH<T, AT> htext(text.data(), text.size());
		ZFillByDFS(0, text.size(), 0, htext); // Alternatively: ZFillByBottomUp();
		hpattern.reserve(text.size());
		DEBUGDO(print_stats("Construction"));
	}

	LInterval<size_t> getChild(size_t i, size_t j, const T &c) { // const {
		DEBUGDO(_getChild++);
		size_t l = i;
		size_t r = i < ct[j - 1] && ct[j - 1] < j ? ct[j - 1] : ct[i];
		ssize_t d = lcp[r];
		while (l < r && r < j && (sa[l] + d >= text.size() || text[sa[l] + d] != c)) {
			l = r;
			r = (lcp[r] != lcp[ct[r]] || lcp[r] > lcp[r + 1]) ? j : ct[r];
		}

		if (unlikely(sa[l] + d >= text.size() || text[sa[l] + d] != c)) return {1, 0};

		return {l, r};
	}

	LInterval<size_t> exit(std::span<const T> pattern, size_t i, size_t j) { // const {
		DEBUGDO(_exit++);
		size_t nlen = 1 + max(lcp[i], lcp[j]);
		size_t elen = j - i == 1 ? text.size() - sa[i] : getlcp(i, j);
		size_t end = min(elen, pattern.size()) - nlen;
		// NOTE: The following comparison can be quite expensive. We can start
		// it from hlen (instead of nlen), but then is the fatBinarySearch
		// memcmy to be potentially expensive 
		if (memcmp(pattern.data() + nlen, text.data() + sa[i] + nlen, end * sizeof(T))) return {1, 0};
		if (elen < pattern.size()) {
			auto [l, r] = getChild(i, j, pattern[elen]);
			if (r < l) return {1, 0};
			return exit(pattern, l, r);
		}
		return {i, j};
	}

	LInterval<size_t> exit_prefix(std::span<const T> pattern, size_t i, size_t j) { // const {
		DEBUGDO(_exit++);
		size_t nlen = 1 + max(lcp[i], lcp[j]);
		if (j - i == 1) {
			if (memcmp(pattern.data() + nlen, text.data() + sa[i] + nlen, sizeof(T))) return {1, 0};
		} else {
			size_t elen = getlcp(i, j);
			size_t end = min(elen, pattern.size()) - nlen;
			if (memcmp(pattern.data() + nlen, text.data() + sa[i] + nlen, end * sizeof(T))) return {1, 0};
			if (elen < pattern.size()) {
				auto [l, r] = getChild(i, j, pattern[elen]);
				if (r < l) return {1, 0};
				return exit_prefix(pattern, l, r);
			}
		}
		return {i, j};
	}

	LInterval<size_t> fatBinarySearch_lambdabased(std::span<const T> pattern) {
		DEBUGDO(_fatBinarySearch++);
		LInterval<size_t> alpha = {0, text.size()};
		size_t l = 0, r = min(pattern.size(), maxnlen); // maxhlen
		while (l < r) {
			DEBUGDO(_fatBinarySearch_while_reps++);
			size_t f = twoFattestR(l, r);
			LInterval<size_t> beta = z[hpattern(f)].value_or(LInterval<size_t>::empty());
			size_t elen = getlcp(beta.from, beta.to) + 1;
			size_t nlen = 1 + max(lcp[beta.from], lcp[beta.to]);
			size_t hlen = twoFattestLR(nlen, elen);
			if (beta.isEmpty() || hlen != f) {
				DEBUGDO(if (beta.isEmpty()) _fatBinarySearch_beta_empty++; else if (hlen != f) _fatBinarySearch_wrong_beta_by_hlen++);
				r = f - 1;
			} else if (!alpha.contains(beta)) {
				DEBUGDO(_fatBinarySearch_wrong_beta_by_contains++);
				l = elen;
			} else {
				DEBUGDO(_fatBinarySearch_beta_ok++);
				l = elen - 1;
				alpha = beta;
			}
		}
		size_t nlen = 1 + max(lcp[alpha.from], lcp[alpha.to]);
		size_t end = min(nlen, pattern.size());
		if (memcmp(pattern.data(), text.data() + sa[alpha.from], end * sizeof(T))) {
			DEBUGDO(_fatBinarySearch_mischivious_collisions++);
			return {0, text.size()};
		}
		return alpha;
	}

	LInterval<size_t> fatBinarySearch_lambdaless(std::span<const T> pattern) {
		DEBUGDO(_fatBinarySearch++);
		LInterval<size_t> alpha = {0, text.size()};
		size_t l = 0, r = min(pattern.size(), maxnlen); // maxhlen
		int64_t m = -1ULL << 63;
		while (l < r) {
			DEBUGDO(_fatBinarySearch_while_reps++);
			while ((m & l) == (m & r)) m >>= 1;
			size_t f = m & r;
			assert(f == twoFattestR(l, r) && "wrong 2-fattest number");
			LInterval<size_t> beta = z[hpattern(f)].value_or(LInterval<size_t>::empty());
			size_t elen = getlcp(beta.from, beta.to) + 1;
			if (beta.isEmpty()) { // NOTE: `|| hlen != f` requries lambda
				DEBUGDO(_fatBinarySearch_beta_empty++);
				r = f - 1;
			} else if (!alpha.contains(beta)) {
				DEBUGDO(_fatBinarySearch_wrong_beta_by_contains++);
				l = elen;
			} else {
				DEBUGDO(_fatBinarySearch_beta_ok++);
				l = elen - 1;
				alpha = beta;
			}
		}
		size_t nlen = 1 + max(lcp[alpha.from], lcp[alpha.to]);
		size_t end = min(nlen, pattern.size());
		if (memcmp(pattern.data(), text.data() + sa[alpha.from], end * sizeof(T))) {
			DEBUGDO(_fatBinarySearch_mischivious_collisions++);
			return {0, text.size()};
		}
		return alpha;
	}

	LInterval<size_t> fatBinarySearch_quasilambdaless(std::span<const T> pattern) {
		DEBUGDO(_fatBinarySearch++);
		LInterval<size_t> alpha = {0, text.size()};
		size_t l = 0, r = min(pattern.size(), maxnlen); // maxhlen
		int64_t m = -1ULL << 63;
		while (l < r) {
			DEBUGDO(_fatBinarySearch_while_reps++);
			while ((m & l) == (m & r)) m >>= 1;
			size_t f = m & r;
			assert(f == twoFattestR(l, r) && "wrong 2-fattest number");
			LInterval<size_t> beta = z[hpattern(f)].value_or(LInterval<size_t>::empty());
			size_t elen = getlcp(beta.from, beta.to) + 1;
			size_t nlen = 1 + max(lcp[beta.from], lcp[beta.to]);
			size_t hlen = twoFattestLR(nlen, elen);
			if (beta.isEmpty() || hlen != f) {
				DEBUGDO(_fatBinarySearch_beta_empty++);
				r = f - 1;
			} else if (!alpha.contains(beta)) {
				DEBUGDO(_fatBinarySearch_wrong_beta_by_contains++);
				l = elen;
			} else {
				DEBUGDO(_fatBinarySearch_beta_ok++);
				l = elen - 1;
				alpha = beta;
			}
		}
		size_t nlen = 1 + max(lcp[alpha.from], lcp[alpha.to]);
		size_t end = min(nlen, pattern.size());
		if (memcmp(pattern.data(), text.data() + sa[alpha.from], end * sizeof(T))) {
			DEBUGDO(_fatBinarySearch_mischivious_collisions++);
			return {0, text.size()};
		}
		return alpha;
	}

	LInterval<size_t> fatBinarySearch(std::span<const T> pattern) {
		DEBUGDO(_fatBinarySearch++);
		LInterval<size_t> alpha = {0, text.size()};
		size_t l = 0, r = min(pattern.size(), maxnlen); // maxhlen
		int64_t m = -1ULL << (lambda(l ^ r) + 1);
		while (l < r) {
			DEBUGDO(_fatBinarySearch_while_reps++);
			while ((m & l) == (m & r)) m >>= 1;
			size_t f = m & r;
			assert(f == twoFattestR(l, r) && "wrong 2-fattest number");
			LInterval<size_t> beta = z[hpattern(f)].value_or(LInterval<size_t>::empty());
			size_t elen = getlcp(beta.from, beta.to) + 1;
			size_t nlen = 1 + max(lcp[beta.from], lcp[beta.to]);
			size_t hlen = twoFattestLR(nlen, elen);
			if (beta.isEmpty() || hlen != f) {
				DEBUGDO(_fatBinarySearch_beta_empty++);
				r = f - 1;
			} else if (!alpha.contains(beta)) {
				DEBUGDO(_fatBinarySearch_wrong_beta_by_contains++);
				l = elen;
			} else {
				DEBUGDO(_fatBinarySearch_beta_ok++);
				l = elen - 1;
				alpha = beta;
			}
		}
		size_t nlen = 1 + max(lcp[alpha.from], lcp[alpha.to]);
		size_t end = min(nlen, pattern.size());
		if (memcmp(pattern.data(), text.data() + sa[alpha.from], end * sizeof(T))) {
			DEBUGDO(_fatBinarySearch_mischivious_collisions++);
			return {0, text.size()};
		}
		return alpha;
	}

	LInterval<size_t> find() {
		DEBUGDO(_find++);
		auto [i, j] = fatBinarySearch(pattern);
		return exit(pattern, i, j);
	}

	LInterval<size_t> find(std::span<const T> pattern) {
		DEBUGDO(_find++);
		hpattern.setString(pattern.data());
		auto [i, j] = fatBinarySearch(pattern);
		return exit(pattern, i, j);
	}

	LInterval<size_t> find_prefix() {
		DEBUGDO(_find++);
		auto [i, j] = fatBinarySearch(pattern);
		return exit_prefix(pattern, i, j);
	}

	LInterval<size_t> find_prefix(std::span<const T> pattern) {
		DEBUGDO(_find++);
		hpattern.setString(pattern.data());
		auto [i, j] = fatBinarySearch(pattern);
		return exit_prefix(pattern, i, j);
	}

	std::span<const T> getText() const { return text; }

	void setPattern(std::span<const T> p) {
		pattern = p;
		hpattern.setString(pattern.data());
		hpattern(pattern.size() - 1);
	}

	std::span<const T> getPattern() const { return pattern; }

	const Vector<size_t, AT> &getSA() const { return sa; }

	const Vector<ssize_t, AT> &getLCP() const { return lcp; }

	const Vector<size_t, AT> &getCT() const { return ct; }

	size_t bitCount() const {
		return sizeof(*this) * 8
			+ sa.bitCount() - sizeof(sa) * 8
			+ lcp.bitCount() - sizeof(lcp) * 8
			+ ct.bitCount() - sizeof(ct) * 8
			+ z.bitCount() - sizeof(z) * 8
			+ hpattern.bitCount() - sizeof(hpattern) * 8;
	}

  private:
	inline ssize_t getlcp(size_t i, size_t j) const { return lcp[i < ct[j - 1] && ct[j - 1] < j ? ct[j - 1] : ct[i]]; }

	void ZFillByDFS(size_t i, size_t j, size_t nlen, RH<T, AT> &htext, size_t depth = 0) {
		DEBUGDO(if (_construction_depth < depth) _construction_depth = depth);
		if (j - i <= 1) return; // leaves are not in the z-map
		size_t l = i;
		size_t r = i < ct[j - 1] && ct[j - 1] < j ? ct[j - 1] : ct[i];
		ssize_t elen = lcp[r];
		size_t hlen = twoFattestLR(nlen, elen);

		if (maxnlen <= nlen) maxnlen = nlen;
		if (maxhlen <= hlen) maxhlen = hlen;

		z.store(htext(sa[i], hlen), LInterval(i, j));
		if (z.elements() * 3 / 2 > z.size()) {
			DEBUGDO(_growZTable++);
			z = LinearProber<typename RH<T, AT>::signature_t, LInterval<size_t>, AT>(z, z.size() * 2);
		}

		do {
			ZFillByDFS(l, r, elen + 1, htext, depth + 1);
			l = r;
			r = ct[r];
		} while (lcp[l] == lcp[ct[l]] && lcp[l] <= lcp[l + 1]);
		ZFillByDFS(l, j, elen + 1, htext, depth + 1);
	}

	void ZFillByBottomUp() {
		struct Node { size_t l, i, j; };
		RH<T, AT> htext(text.data(), text.size());
		Vector<Node, AT> stack;
		stack.reserve(text.size());
		stack.pushBack(Node{0, 0, 0});

		for (size_t i = 1; i < text.size(); i++) {
			size_t lb = i - 1;
			while (lcp[i] < stack[stack.size() - 1].l) {
				Node node = stack.popBack();
				node.j = i;

				ssize_t nlen = 1 + max(lcp[node.i], lcp[node.j]);
				ssize_t elen = getlcp(node.i, node.j);
				size_t hlen = twoFattestLR(nlen, elen);

				if (maxnlen <= nlen) maxnlen = nlen;
				if (maxhlen <= hlen) maxhlen = hlen;

				z.store(htext(sa[node.i], hlen), LInterval(node.i, node.j));
				if (z.elements() * 3 / 2 > z.size()) {
					DEBUGDO(_growZTable++);
					z = LinearProber<typename RH<T, AT>::signature_t, LInterval<size_t>, AT>(z, z.size() * 2);
				}

				lb = node.i;
			}
			if (lcp[i] > stack[stack.size() - 1].l) stack.pushBack(Node{lcp[i], lb, i});
		}
	}

	friend std::ostream &operator<<(std::ostream &os, const MemcmpZuffixArray<T, RH, AT> &ds) { return os << ds.text << ds.sa << ds.lcp << ds.ct << ds.z << ds.maxnlen << ds.maxhlen; }
	friend std::istream &operator>>(std::istream &is, MemcmpZuffixArray<T, RH, AT> &ds) { return is >> ds.text >> ds.sa >> ds.lcp >> ds.ct >> ds.z>> ds.maxnlen >> ds.maxhlen; }

#ifdef DEBUG
  public:
	int _construction_depth = 0;

	int _getChild = 0;
	int _exit = 0;
	int _fatBinarySearch = 0;
	int _find = 0;
	int _growZTable = 0;

	int _fatBinarySearch_while_reps = 0;
	int _fatBinarySearch_beta_empty = 0;
	int _fatBinarySearch_wrong_beta_by_contains = 0;
	int _fatBinarySearch_wrong_beta_by_hlen = 0;
	int _fatBinarySearch_beta_ok = 0;
	int _fatBinarySearch_mischivious_collisions = 0;

	void reset_stats() {
		_construction_depth = 0;

		_getChild = 0;
		_exit = 0;
		_fatBinarySearch = 0;
		_find = 0;

		_fatBinarySearch_while_reps = 0;
		_fatBinarySearch_beta_empty = 0;
		_fatBinarySearch_wrong_beta_by_contains = 0;
		_fatBinarySearch_wrong_beta_by_hlen = 0;
		_fatBinarySearch_beta_ok = 0;
		_fatBinarySearch_mischivious_collisions = 0;

		z.reset_stats();
	}

	void print_stats(const char *msg = "") {
		std::cerr << "--------------------------------------------------------------------------------" << std::endl;
		std::cerr << "MemcmpZuffixArray.hpp: " << msg << std::endl;
		std::cerr << "- construction_depth: " << _construction_depth << std::endl;
		std::cerr << "- construction_maxnlen: " << maxnlen << std::endl;
		std::cerr << "- construction_maxhlen: " << maxhlen << std::endl;
		std::cerr << "- getChild: " << _getChild << std::endl;
		std::cerr << "- exit: " << _exit << std::endl;
		std::cerr << "- fatBinarySearch: " << _fatBinarySearch << std::endl;
		std::cerr << "- find: " << _find << std::endl;
		std::cerr << "- fatBinarySearch_while_reps: " << _fatBinarySearch_while_reps << std::endl;
		std::cerr << "- fatBinarySearch_beta_beta_empty: " << _fatBinarySearch_beta_empty << std::endl;
		std::cerr << "- fatBinarySearch_wrong_beta_by_contains: " << _fatBinarySearch_wrong_beta_by_contains << std::endl;
		std::cerr << "- fatBinarySearch_wrong_beta_by_hlen: " << _fatBinarySearch_wrong_beta_by_hlen << std::endl;
		std::cerr << "- fatBinarySearch_beta_ok: " << _fatBinarySearch_beta_ok << std::endl;
		std::cerr << "- fatBinarySearch_mischivious_collisions: " << _fatBinarySearch_mischivious_collisions << std::endl;
		z.print_stats();
		std::cerr << "--------------------------------------------------------------------------------" << std::endl;
	}
#endif
};

} // namespace zarr
