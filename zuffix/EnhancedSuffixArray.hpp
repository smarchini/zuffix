#pragma once

#include <sux/support/common.hpp>
#include <sux/util/Vector.hpp>

#include "util/LInterval.hpp"
#include "util/String.hpp"
#include "util/common.hpp"

namespace zarr {
using ::sux::util::Vector;

template <typename T> class EnhancedSuffixArray {
  private:
	const String<T> text;
	const Vector<size_t> sa;
	const Vector<ssize_t> lcp;
	const Vector<size_t> ct;

  public:
	EnhancedSuffixArray(String<T> string) : text(std::move(string)), sa(SAConstructBySAIS(text)), lcp(LCPConstructByStrcmp(text, sa)), ct(CTConstructByAbouelhoda(lcp)) {}

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

	LInterval<size_t> find(const String<T> &pattern) const {
		size_t c = 0;
		size_t i = 0, j = text.length();
		while (c < pattern.length() && i < j) {
			std::tie(i, j) = getChild(i, j, pattern[c]);

			if (j - i == 1) {
				for (size_t k = c; k < pattern.length(); k++)
					if (text[sa[i] + k] != pattern[k]) return {1, 0};
				break;
			}

			size_t d = min(static_cast<size_t>(getlcp(i, j)), pattern.length());
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
};

} // namespace zarr
