#pragma once

#include "util/LInterval.hpp"
#include "util/String.hpp"
#include "util/common.hpp"

#include "hash/RabinKarpHash.hpp"

#include <limits>
#include <sux/support/common.hpp>
#include <sux/util/Vector.hpp>
#include <tuple>
#include <unordered_map>

namespace zarr {
using ::sux::util::Vector;

// TODO RabinKarp should be a template argument
template <typename T, hash_function hash> class EnhancedZuffixArray {
  private:
	const String<T> text;
	const Vector<size_t> sa;
	const Vector<ssize_t> lcp;
	const Vector<size_t> ct;
	// std::unordered_map<size_t, size_t> z;
	Vector<size_t> htz; // TODO this will replace z

  public:
	EnhancedZuffixArray(String<T> string) : text(std::move(string)), sa(SAConstructBySort(text)), lcp(LCPConstructByStrcmp(text, sa)), ct(CTConstructByAbouelhoda(lcp)) {
		htz.resize(1 << 13); // TODO not really
		ZFillByDFS(0, text.length(), 0, RabinKarpHash(&text));
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
		// TODO assert n_[i,j) \preceq P
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
		RabinKarpHash h(&pattern);
		size_t i = 0, j = text.length();
		size_t l = 0, r = pattern.length();
		while (l < r) {
			size_t f = twoFattestR(l, r);
			// LInterval<size_t> beta = unpack(z[hash(&pattern, f - 1)]);
			LInterval<size_t> beta = unpack(htz[h(0, f) % htz.size()]);
			if (beta.isEmpty()) {
				r = f - 1;
			} else {
				std::tie(i, j) = beta;
				l = getlcp(i, j) + 1;
			}
		}
		return {i, j};
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

	void ZFillByDFS(size_t i, size_t j, size_t nlen, RabinKarpHash<T> h) {
		if (j - i <= 1) return; // leaves are not in the z-map
		size_t l = i;
		size_t r = i < ct[j - 1] && ct[j - 1] < j ? ct[j - 1] : ct[i];
		ssize_t elen = lcp[r];
		size_t hlen = twoFattestLR(nlen, elen);
		// z[hash(&text[sa[i]], hlen)] = pack({i, j});
		//htz[h(sa[i] + hlen) % htz.size()] = pack({i, j});
		htz[h(sa[i], hlen) % htz.size()] = pack({i, j});
		do {
			ZFillByDFS(l, r, elen + 1, h);
			l = r;
			r = ct[r];
		} while (lcp[l] == lcp[ct[l]] && lcp[l] <= lcp[l + 1]);
		ZFillByDFS(l, j, elen + 1, h);
	}

	size_t pack(LInterval<size_t> x) const { return x.from << 32 | x.to; }

	LInterval<size_t> unpack(size_t x) const { return {x >> 32, x & 0xffff}; }
};

} // namespace zarr
