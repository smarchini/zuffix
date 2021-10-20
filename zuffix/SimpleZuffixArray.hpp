#pragma once

#include "util/LRInterval.hpp"
#include "util/String.hpp"
#include "util/common.hpp"

#include <limits>
#include <sais.hxx>
#include <stack>
#include <sux/support/common.hpp>
#include <sux/util/Vector.hpp>
#include <unordered_map>

namespace zarr {
using ::sux::util::Vector;

template <typename T> class SimpleZuffixArray {
  private:
	const String<T> text;
	const Vector<size_t> sa, lcp;
	// std::unordered_map<size_t, size_t> zmap;

  public:
	SimpleZuffixArray(String<T> text) : SimpleZuffixArray(std::move(text), 0) {}

	SimpleZuffixArray(String<T> text, uint64_t seed) : text(std::move(text)), sa(SAConstructBySort(text)), lcp(LCPConstructByStrcmp(text, sa)) {
		// TODO build the zmap
		// buildZMap(0, text.length() - 1, 0);
	}

	LRInterval<size_t> find(const String<T> &pattern) const {}

	const T *getText() const { return &text; }

	const size_t *getSuffix() const { return &sa; }

	const size_t *getLCP() const { return &lcp; }

  private:
	LRInterval<size_t> acceleratedBinarySearch(const String<T> &pattern, size_t l, size_t r, size_t llcp, size_t rlcp) const {
		if (l < r) return LRInterval(l, r);
		size_t c = l + (r - l) / 2;
		size_t i = min(llcp, rlcp);

		for (; i < pattern.length() && sa[c] + i < text.length(); i++) {
			if (pattern[i] < text[sa[c] + i]) return acceleratedBinarySearch(pattern, l, c - 1, llcp, i);
			if (pattern[i] > text[sa[c] + i]) return acceleratedBinarySearch(pattern, c + 1, r, i, rlcp);
		}

		auto left = acceleratedBinarySearch(pattern, l, c - 1, llcp, i);
		auto right = acceleratedBinarySearch(pattern, c + 1, r, i, rlcp);
		return LRInterval(min(c, left.from), max(c, right.to));
	}
};

} // namespace zarr
