#pragma once

#include "util/LInterval.hpp"
#include "util/String.hpp"
#include "util/common.hpp"

#include "hash/CyclicPolyHash.hpp"
#include "hash/RabinKarpHash.hpp"

#include <limits>
#include <sux/support/common.hpp>
#include <sux/util/Vector.hpp>
#include <tuple>
#include <unordered_map>

namespace zarr {
using ::sux::util::Vector;

template <typename T, template <typename U> class RH> class EnhancedZuffixArray {
  private:
	const String<T> text;
	const Vector<size_t> sa;
	const Vector<ssize_t> lcp;
	const Vector<size_t> ct;
	Vector<size_t> z;

  public:
	EnhancedZuffixArray(String<T> string) : text(std::move(string)), sa(SAConstructBySort(text)), lcp(LCPConstructByStrcmp(text, sa)), ct(CTConstructByAbouelhoda(lcp)) {
		z.resize(max(round_pow2(text.length()) << 1, (uint64_t)1 << 20)); // TODO tweak me
		ZFillByDFS(0, text.length(), 0, RH<T>(&text));
	}

	LInterval<size_t> getChild(size_t i, size_t j, const T &c) const {
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

	LInterval<size_t> exit(const String<T> &pattern, size_t i, size_t j) const {
		size_t nlen = 1 + max(lcp[i], lcp[j]);
		size_t elen = j - i == 1 ? text.length() - sa[i] : getlcp(i, j);
		for (size_t k = nlen; k < elen && k < pattern.length(); k++)
			if (pattern[k] != text[sa[i] + k]) return {1, 0};
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
		size_t l = 0, r = pattern.length();
		while (l < r) {
			size_t f = twoFattestR(l, r);
			LInterval<size_t> beta = unpack(z[h(0, f) % z.size()]);
			if (beta.isEmpty()) {
				r = f - 1;
			} else if (!alpha.contains(beta)) {
				break;
			} else {
				alpha = beta;
				l = getlcp(alpha.from, alpha.to) + 1;
			}
		}
		size_t nlen = 1 + max(lcp[alpha.from], lcp[alpha.to]);
		for (size_t k = min(nlen, pattern.length()); k > 0; k--) {
			if (pattern[k - 1] != text[sa[alpha.from] + k - 1]) return {0, text.length()};
		}
		return alpha;
	}

	LInterval<size_t> find(const String<T> &pattern) {
		auto [i, j] = fatBinarySearch(pattern);
		return exit(pattern, i, j);
	}

	const String<T> &getText() const { return text; }

	const Vector<size_t> &getSA() const { return sa; }

	const Vector<ssize_t> &getLCP() const { return lcp; }

	const Vector<size_t> &getCT() const { return ct; }

  private:
	inline ssize_t getlcp(size_t i, size_t j) const { return lcp[i < ct[j - 1] && ct[j - 1] < j ? ct[j - 1] : ct[i]]; }

	void ZFillByDFS(size_t i, size_t j, size_t nlen, RH<T> h) {
		if (j - i <= 1) return; // leaves are not in the z-map
		size_t l = i;
		size_t r = i < ct[j - 1] && ct[j - 1] < j ? ct[j - 1] : ct[i];
		ssize_t elen = lcp[r];
		size_t hlen = twoFattestLR(nlen, elen);
		z[h(sa[i], hlen) % z.size()] = pack({i, j});
		do {
			ZFillByDFS(l, r, elen + 1, h);
			l = r;
			r = ct[r];
		} while (lcp[l] == lcp[ct[l]] && lcp[l] <= lcp[l + 1]);
		ZFillByDFS(l, j, elen + 1, h);
	}

	size_t pack(LInterval<size_t> x) const { return x.from << 32 | x.to; }

	LInterval<size_t> unpack(size_t x) const { return {x >> 32, x & 0xffffffff}; }
};

} // namespace zarr
