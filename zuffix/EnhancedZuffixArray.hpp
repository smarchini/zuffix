#pragma once

#include "util/LInterval.hpp"
#include "util/String.hpp"
#include "util/common.hpp"

#include <limits>
#include <sux/support/common.hpp>
#include <sux/util/Vector.hpp>
#include <tuple>
#include <unordered_map>

namespace zarr {
using ::sux::util::Vector;

template <typename T, hash_function hash> class EnhancedZuffixArray {
  private:
	const String<T> text;
	const Vector<size_t> sa;
	const Vector<ssize_t> lcp;
	const Vector<size_t> ct;
	const std::unordered_map<size_t, size_t> zmap;

  public:
	EnhancedZuffixArray(String<T> string)
		: text(std::move(string)), sa(SAConstructBySort(text)), lcp(LCPConstructByStrcmp(text, sa)), ct(CTConstructByAbouelhoda(lcp)), zmap(ZMapConstructByTraversal(0, text.length(), 0)) {}

	LInterval<size_t> getChild(size_t i, size_t j, const T &c) const {
		size_t l = i;
		size_t r = i < ct[j - 1] && ct[j - 1] < j ? ct[j - 1] : ct[i];
		ssize_t d = lcp[r];
		while (l < r && (sa[l] + d >= text.length() || text[sa[l] + d] != c)) {
			l = r;
			if (lcp[r] != lcp[ct[r]]) return {l, j};
			r = ct[r];
		}
		return {l, r};
	}

	LInterval<size_t> find(const String<T> &pattern) const {
		size_t c = 0;
		size_t i = 0, j = text.length();
		while (c < pattern.length() && i < j) {
			std::tie(i, j) = getChild(i, j, pattern[c]);
			ssize_t d = getlcp(i, j);
			if (d <= 0) d = text.length();
			d = min(static_cast<size_t>(d), pattern.length());
			for (size_t k = c; k < d; k++)
				if (text[sa[i] + k] != pattern[k]) return {1, 0};
			c = d;
		}
		return {i, j};
	}

	const String<T> &getText() const { return text; }

	const Vector<size_t> &getSA() const { return sa; }

	const Vector<ssize_t> &getLCP() const { return lcp; }

	const Vector<size_t> &getCT() const { return ct; }

  private:
	inline ssize_t getlcp(size_t i, size_t j) const { return lcp[i < ct[j - 1] && ct[j - 1] < j ? ct[j - 1] : ct[i]]; }

	std::unordered_map<size_t, size_t> ZMapConstructByTraversal(size_t i, size_t j, size_t name) {

    }
};

} // namespace zarr
