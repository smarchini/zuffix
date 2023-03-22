#pragma once

#include <sux/support/common.hpp>
#include <sux/util/Vector.hpp>

#include "util/LInterval.hpp"
#include "util/LinearProber.hpp"
#include "util/String.hpp"
#include "util/common.hpp"

namespace zarr {
using ::sux::util::Vector;
using ::sux::util::AllocType;

template <typename T, template <typename U> class RH, AllocType AT = MALLOC> class NothingZuffixArray {
  private:
	std::span<const T> text;
	Vector<size_t, AT> sa;
	Vector<ssize_t, AT> lcp;
	Vector<size_t, AT> ct;
	LinearProber<uint64_t, AT> z;
	size_t maxhlen = 0;

  public:
	NothingZuffixArray() {}

	NothingZuffixArray(std::span<const T> string) : text(std::move(string)), sa(SAConstructByGrebnovSAIS<T, AT>(text)), lcp(LCPConstructByKarkkainenPsi<T, AT>(text, sa)), ct(CTConstructByAbouelhoda<AT>(lcp)) {
		assert(text.data()[text.size() - 1] == std::numeric_limits<T>::max() && "Missing $-terminator");
		// z.resize(ceil_pow2(text.size()) << 1); // TODO: tweak me to improve construction performance
		RH<T> hash(text.data());
		ZFillByDFS(0, text.size(), 0, hash);
		// ZFillByBottomUp();  // alternative z-map construction
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

		if (sa[l] + d >= text.size() || text[sa[l] + d] != c) return {1, 0};

		return {l, r};
	}

	LInterval<size_t> exit(std::span<const T> pattern, size_t i, size_t j) { // const {
		DEBUGDO(_exit++);
		size_t nlen = 1 + max(lcp[i], lcp[j]);
		size_t elen = j - i == 1 ? text.size() - sa[i] : getlcp(i, j);
		size_t end = min(min(elen, pattern.size()), maxhlen) - nlen;
		if (memcmp(pattern.data() + nlen, text.data() + sa[i] + nlen, end * sizeof(T))) return {1, 0};
		if (elen < pattern.size()) {
			auto [l, r] = getChild(i, j, pattern[elen]);
			if (r < l) return {1, 0};
			return exit(pattern, l, r);
		}
		return {i, j};
	}

	LInterval<size_t> fatBinarySearch(std::span<const T> pattern) {
		DEBUGDO(_fatBinarySearch++);
		RH<T> h(pattern.data());
		LInterval<size_t> alpha = {0, text.size()};
		size_t l = 0, r = min(pattern.size(), maxhlen);
		int64_t m = -1ULL << (lambda(l ^ r) + 1);
		while (l < r) {
			DEBUGDO(_fatBinarySearch_while_reps++);
			while ((m & l) == (m & r)) m >>= 1;
			size_t f = m & r;
			assert(f == twoFattestR(l, r) && "wrong 2-fattest number");
			LInterval<size_t> beta = unpack(z[h(f)].value_or(0x100000000));
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

	LInterval<size_t> find(std::span<const T> pattern) {
		DEBUGDO(_find++);
		auto [i, j] = fatBinarySearch(pattern);
		return exit(pattern, i, j);
	}

	std::span<const T> getText() const { return text; }

	const Vector<size_t> &getSA() const { return sa; }

	const Vector<ssize_t> &getLCP() const { return lcp; }

	const Vector<size_t> &getCT() const { return ct; }

	size_t bitCount() const {
		return sizeof(*this) * 8
			+ sa.bitCount() - sizeof(sa) * 8
			+ lcp.bitCount() - sizeof(lcp) * 8
			+ ct.bitCount() - sizeof(ct) * 8
			+ z.bitCount() - sizeof(z) * 8;
	}

  private:
	inline ssize_t getlcp(size_t i, size_t j) const { return lcp[i < ct[j - 1] && ct[j - 1] < j ? ct[j - 1] : ct[i]]; }

	void ZFillByDFS(size_t i, size_t j, size_t nlen, RH<T> &h, size_t depth = 0) {
		DEBUGDO(if (_construction_depth < depth) _construction_depth = depth);
		if (j - i <= 1) return; // leaves are not in the z-map
		size_t l = i;
		size_t r = i < ct[j - 1] && ct[j - 1] < j ? ct[j - 1] : ct[i];
		ssize_t elen = lcp[r];
		size_t hlen = twoFattestLR(nlen, elen);
		if (maxhlen <= hlen) maxhlen = hlen;
		assert(depth <= hlen);

		z.store(h(sa[i], hlen), pack({i, j}));
		if (z.elements() * 3 / 2 > z.size()) growZTable(); // TODO tweak this constant?

		do {
			ZFillByDFS(l, r, elen + 1, h, depth + 1);
			l = r;
			r = ct[r];
		} while (lcp[l] == lcp[ct[l]] && lcp[l] <= lcp[l + 1]);
		ZFillByDFS(l, j, elen + 1, h, depth + 1);
	}

	// TODO clean me
	void ZFillByBottomUp() {
		RH<T> h(text.data());
		Vector<ssize_t> stackl(0);
		Vector<ssize_t> stacki(0);
		Vector<ssize_t> stackj(0);
		stackl.reserve(text.size());
		stacki.reserve(text.size());
		stackj.reserve(text.size());
		stackl.pushBack(0);
		stacki.pushBack(0);
		stackj.pushBack(text.size());

		for (size_t i = 1; i < text.size(); i++) {
			size_t lb = i - 1;
			while (lcp[i] < stackl[stackl.size() - 1]) {
				size_t intervall = stackl.popBack();
				size_t intervali = stacki.popBack();
				size_t intervalj = stackj.popBack();
				intervalj = i;

				ssize_t nlen = 1 + max(lcp[intervali], lcp[intervalj]);
				ssize_t elen = getlcp(intervali, intervalj);
				size_t hlen = twoFattestLR(nlen, elen);
				if (maxhlen <= hlen) maxhlen = hlen;

				z.store(h(sa[intervali], hlen), pack({intervali, intervalj}));
				if (z.elements() * 3 / 2 > z.size()) growZTable(); // TODO tweak this constnat?

				lb = intervali;
			}
			if (lcp[i] > stackl[stackl.size() - 1]) {
				stackl.pushBack(lcp[i]);
				stacki.pushBack(lb);
				stackj.pushBack(i);
			}
		}
	}

	void growZTable() {
		DEBUGDO(z.reset_stats());
		DEBUGDO(_growZTable++);
		size_t n = z.size();
		auto table = z.getTable();
		LinearProber<uint64_t, AT> zlarge(n * 2);
		for (size_t i = 0; i < n; i++) {
			auto [signature, value] = table[i];
			if (signature) zlarge.store(signature, value);
		}
		std::swap(z, zlarge);
	}

	uint64_t pack(LInterval<size_t> x) const { return x.from << 32 | x.to; }
	LInterval<size_t> unpack(uint64_t x) const { return {x >> 32, x & 0xffffffff}; }

	friend std::ostream &operator<<(std::ostream &os, const NothingZuffixArray<T, RH, AT> &ds) { return os << ds.text << ds.sa << ds.lcp << ds.ct << ds.z << ds.maxhlen; }
	friend std::istream &operator>>(std::istream &is, NothingZuffixArray<T, RH, AT> &ds) { return is >> ds.text >> ds.sa >> ds.lcp >> ds.ct >> ds.z >> ds.maxhlen; }

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
		std::cerr << "NothingZuffixArray.hpp: " << msg << std::endl;
		std::cerr << "- construction_depth: " << _construction_depth << std::endl;
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
