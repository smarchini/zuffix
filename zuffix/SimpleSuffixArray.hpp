#pragma once

#include "util/LInterval.hpp"
#include "util/String.hpp"
#include "util/common.hpp"

#include <limits>
#include <sux/support/common.hpp>
#include <sux/util/Vector.hpp>
#include <unordered_map>

namespace zarr {
using ::sux::util::Vector;

template <typename T> class SimpleSuffixArray {
  private:
	const String<T> text;
	const Vector<size_t> sa;

  public:
	SimpleSuffixArray(String<T> string) : text(std::move(string)), sa(SAConstructBySort(text)) {}

	LInterval<ssize_t> find(const String<T> &pattern) const { return acceleratedBinarySearch(pattern, 0, text.length() - 1, 0, 0); }

	const String<T> &getText() const { return text; }

	const Vector<size_t> &getSA() const { return sa; }

  private:
	LInterval<ssize_t> acceleratedBinarySearch(const String<T> &pattern, ssize_t l, ssize_t r, ssize_t llcp, ssize_t rlcp) const {
		if (r < l) return {l, r};
		ssize_t c = l + (r - l) / 2;

		ssize_t i = min(llcp, rlcp);
		for (; i < pattern.length() && sa[c] + i < text.length(); i++) {
			if (pattern[i] < text[sa[c] + i]) return acceleratedBinarySearch(pattern, l, c - 1, llcp, i);
			if (pattern[i] > text[sa[c] + i]) return acceleratedBinarySearch(pattern, c + 1, r, i, rlcp);
		}

		auto left = acceleratedBinarySearch(pattern, l, c - 1, llcp, i);
		auto right = acceleratedBinarySearch(pattern, c + 1, r, i, rlcp);
		return {min(c, left.from), max(c, right.to)};
	}
};

} // namespace zarr
