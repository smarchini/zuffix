#pragma once

#include <sux/support/common.hpp>
#include <sux/util/Vector.hpp>

#include "util/LInterval.hpp"
#include "util/OpenAddressing.hpp"
#include "util/String.hpp"
#include "util/common.hpp"

namespace zarr {
using ::sux::util::Vector;

template <typename T, template <typename U> class RH> class EnhancedZuffixArray {
  private:
	String<T> text;
	Vector<size_t> sa;
	Vector<ssize_t> lcp;
	Vector<size_t> ct;
	OpenAddressing<uint64_t> z;
    size_t maxhlen = 0;


  public:
	EnhancedZuffixArray() {}

	EnhancedZuffixArray(String<T> string) : text(std::move(string)), sa(SAConstructByGrebnovSAIS(text)), lcp(LCPConstructByKarkkainenPsi(text, sa)), ct(CTConstructByAbouelhoda(lcp)) {
		z.resize(ceil_pow2(text.length() * 3)); // TODO tweak me
		RH<T> hash(&text);
		//ZFillByDFS(0, text.length(), 0, hash);
		ZFillByBottomUp();
		print_debug_stats("CONSTRUCTION");
	}

	LInterval<size_t> getChild(size_t i, size_t j, const T &c) {
		getChild_calls++;
		size_t l = i;
		size_t r = i < ct[j - 1] && ct[j - 1] < j ? ct[j - 1] : ct[i];
		ssize_t d = lcp[r];
		while (l < r && r < j && (sa[l] + d >= text.length() || text[sa[l] + d] != c)) {
			l = r;
			r = (lcp[r] != lcp[ct[r]] || lcp[r] > lcp[r + 1]) ? j : ct[r];
		}

		if (sa[l] + d >= text.length() || text[sa[l] + d] != c) return {1, 0};

		return {l, r};
	}

	LInterval<size_t> exit(const String<T> &pattern, size_t i, size_t j) {
		exit_calls++;
		size_t nlen = 1 + max(lcp[i], lcp[j]);
		size_t elen = j - i == 1 ? text.length() - sa[i] : getlcp(i, j);
		size_t end = min(elen, pattern.length()) - nlen;
		if (memcmp(&pattern + nlen, &text + sa[i] + nlen, end * sizeof(T))) return {1, 0};
		if (elen < pattern.length()) {
			auto [l, r] = getChild(i, j, pattern[elen]);
			if (r < l) return {1, 0};
			return exit(pattern, l, r);
		}
		return {i, j};
	}

	LInterval<size_t> fatBinarySearch(const String<T> &pattern) {
		RH<T> h(&pattern);
		LInterval<size_t> alpha = {0, text.length()};
		size_t l = 0, r = min(pattern.length(), maxhlen);
		while (l < r) {
			fatBinarySearch_while_reps++;
			size_t f = twoFattestR(l, r);
			LInterval<size_t> beta = unpack(z[h(f)].value_or(0x100000000));
			size_t elen = getlcp(beta.from, beta.to) + 1;
			size_t nlen = 1 + max(lcp[beta.from], lcp[beta.to]);
			std::cerr << "h(" << (&pattern)[0] << " .. " << (&pattern)[f-1] << "), " << sizeof(T) << ", " << sizeof((&pattern)[0])  << std::endl;
			std::cerr << "2-fattest(" << l << " .. " << r << "] = " << f << ", beta = [" << beta.from << " .. " << beta.to << "), nlen = " << nlen << ", elen = " << elen << ", signature = " << h(f) << std::endl;
			if (beta.isEmpty() || nlen > pattern.length() || elen < f) { // forse un < ?
				if (beta.isEmpty())
					fatBinarySearch_beta_empty++;
				else if (elen < f)
					fatBinarySearch_beta_fake_by_bad_elen++;
				else if (nlen > pattern.length())
					fatBinarySearch_beta_fake_by_bad_nlen++;
				r = f - 1;
			} else if (!alpha.contains(beta)) {
				fatBinarySearch_beta_fake_by_contains++;
				l = elen; // remove +1 here too???? (see few lines below)
			} else {
				fatBinarySearch_beta_good++;
				l = elen; // remove +1 because we use twoFattest (l .. r] (we exclude the left boundary)
				l = f; // BUG: this line shouldn't be here.... but without it, it doesn't quite work. maybe elen is wrong?!?!?!
				alpha = beta;
			}
		}
		size_t nlen = 1 + max(lcp[alpha.from], lcp[alpha.to]);
		size_t end = min(nlen, pattern.length());
		if (memcmp(&pattern, &text + sa[alpha.from], end * sizeof(T))) {
			fatBinarySearch_mischivious_collision++;
			return {0, text.length()}; // mischivious collision
		}
		return alpha;
	}

	LInterval<size_t> find(const String<T> &pattern) {
		std::cerr << "text: " << text.length() << " characters" << std::endl;
		std::cerr << "pattern: " << pattern.length() << " characters" << std::endl;
		reset_debug_stats();
		auto [i, j] = fatBinarySearch(pattern);
		auto e = exit(pattern, i, j);
		print_debug_stats("FIND");
		assert(exit_calls <= 2 || fatBinarySearch_beta_fake_by_bad_elen > 0 || fatBinarySearch_beta_fake_by_contains > 0 || fatBinarySearch_mischivious_collision > 0);
		return e;
	}

	const String<T> &getText() const { return text; }

	const Vector<size_t> &getSA() const { return sa; }

	const Vector<ssize_t> &getLCP() const { return lcp; }

	const Vector<size_t> &getCT() const { return ct; }

  private:
	inline ssize_t getlcp(size_t i, size_t j) const { return lcp[i < ct[j - 1] && ct[j - 1] < j ? ct[j - 1] : ct[i]]; }

	void ZFillByDFS(size_t i, size_t j, size_t nlen, RH<T> &h, size_t depth = 0) {
		if (suffixtree_depth < depth) suffixtree_depth = depth;
		if (j - i <= 1) return; // leaves are not in the z-map
		size_t l = i;
		size_t r = i < ct[j - 1] && ct[j - 1] < j ? ct[j - 1] : ct[i];
		ssize_t elen = lcp[r];
		size_t hlen = twoFattestLR(nlen, elen);
		if (maxhlen < hlen) maxhlen = hlen;

		assert(depth <= hlen);
		// std::cerr << "Storing node: [" << i << " .. " << j << "): nlen = " << nlen << ", hlen = " << hlen << ", elen = " << elen << std::endl;
		assert(nlen <= hlen && hlen <= elen);
		z.store(h.immediate(sa[i], hlen), pack({i, j}));

		do {
			ZFillByDFS(l, r, elen + 1, h, depth + 1);
			l = r;
			r = ct[r];
		} while (lcp[l] == lcp[ct[l]] && lcp[l] <= lcp[l + 1]);
		ZFillByDFS(l, j, elen + 1, h, depth + 1);
	}

	// TODO clean me
	void ZFillByBottomUp() {
		RH<T> h(&text);
		Vector<ssize_t> stackl(0);
		Vector<ssize_t> stacki(0);
		Vector<ssize_t> stackj(0);
		stackl.reserve(text.length());
		stacki.reserve(text.length());
		stackj.reserve(text.length());
		stackl.pushBack(0);
		stacki.pushBack(0);
		stackj.pushBack(text.length());

		for (size_t i = 1; i < text.length(); i++) {
			size_t lb = i - 1;
			while (lcp[i] < stackl[stackl.size() - 1]) {
				size_t intervall = stackl.popBack();
				size_t intervali = stacki.popBack();
				size_t intervalj = stackj.popBack();
				intervalj = i;

				ssize_t nlen = 1 + max(lcp[intervali], lcp[intervalj]);
				ssize_t elen = getlcp(intervali, intervalj);
				size_t hlen = twoFattestLR(nlen, elen);
				if (maxhlen < hlen) maxhlen = hlen;

				// std::cerr << "Storing node: " << intervall << "-[" << intervali << " .. " << intervalj << "): nlen = " << nlen << ", hlen = " << hlen << ", elen = " << elen << ", signature = " << h.immediate(sa[intervali], hlen) << std::endl;
				z.store(h.immediate(sa[intervali], hlen), pack({intervali, intervalj}));

				lb = intervali;
			}
			if (lcp[i] > stackl[stackl.size() - 1]) {
				stackl.pushBack(lcp[i]);
				stacki.pushBack(lb);
				stackj.pushBack(i);
			}
		}
	}

	uint64_t pack(LInterval<size_t> x) const { return x.from << 32 | x.to; }
	LInterval<size_t> unpack(uint64_t x) const { return {x >> 32, x & 0xffffffff}; }

	friend std::ostream &operator<<(std::ostream &os, const EnhancedZuffixArray<T, RH> &ds) { return os << ds.text << ds.sa << ds.lcp << ds.ct << ds.z << ds.maxhlen; }
	friend std::istream &operator>>(std::istream &is, EnhancedZuffixArray<T, RH> &ds) { return is >> ds.text >> ds.sa >> ds.lcp >> ds.ct >> ds.z >> ds.maxhlen; }

  public:
	int getChild_calls = 0;
	int exit_calls = 0;
	int fatBinarySearch_while_reps = 0;
	int fatBinarySearch_beta_fake_by_contains = 0;
	int fatBinarySearch_beta_fake_by_bad_elen = 0;
	int fatBinarySearch_beta_fake_by_bad_nlen = 0;
	int fatBinarySearch_beta_empty = 0;
	int fatBinarySearch_beta_good = 0;
	int fatBinarySearch_mischivious_collision = 0;
	int suffixtree_depth = 0;

	void print_debug_stats(const char *msg) {
		std::cerr << "--------------------------------------------------------------------------------" << std::endl;
		std::cerr << msg << std::endl;
		std::cerr << "--------------------------------------------------------------------------------" << std::endl;
		std::cerr << "DEBUG_STATS_EnhancedZuffixArray.hpp:" << std::endl;
		std::cerr << "- suffixtree_depth: " << suffixtree_depth << std::endl;
		std::cerr << "- suffixtree_maxhlen: " << maxhlen << std::endl;
		std::cerr << "- getChild_calls: " << getChild_calls << std::endl;
		std::cerr << "- exit_calls: " << exit_calls << std::endl;
		std::cerr << "- fatBinarySearch_while_reps: " << fatBinarySearch_while_reps << std::endl;
		std::cerr << "- fatBinarySearch_beta_fake_by_contains: " << fatBinarySearch_beta_fake_by_contains << std::endl;
		std::cerr << "- fatBinarySearch_beta_fake_by_bad_elen: " << fatBinarySearch_beta_fake_by_bad_elen << std::endl;
		std::cerr << "- fatBinarySearch_beta_empty: " << fatBinarySearch_beta_empty << std::endl;
		std::cerr << "- fatBinarySearch_beta_good: " << fatBinarySearch_beta_good << std::endl;
		std::cerr << "- fatBinarySearch_mischivious_collision: " << fatBinarySearch_mischivious_collision << std::endl;
		z.print_debug_stats();
		std::cerr << "--------------------------------------------------------------------------------" << std::endl;
	}

	void reset_debug_stats() {
		getChild_calls = 0;
		exit_calls = 0;
		fatBinarySearch_while_reps = 0;
		fatBinarySearch_beta_fake_by_contains = 0;
		fatBinarySearch_beta_fake_by_bad_elen = 0;
		fatBinarySearch_beta_fake_by_bad_nlen = 0;
		fatBinarySearch_beta_empty = 0;
		fatBinarySearch_beta_good = 0;
		fatBinarySearch_mischivious_collision = 0;
		suffixtree_depth = 0;
		z.reset_debug_stats();
	}
};

} // namespace zarr
