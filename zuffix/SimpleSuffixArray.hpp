#pragma once

#include <sux/support/common.hpp>
#include <sux/util/Vector.hpp>

#include "util/LInterval.hpp"
#include "util/String.hpp"
#include "util/common.hpp"

namespace zarr {
using ::sux::util::Vector;
using ::sux::util::AllocType;

template <typename T, AllocType AT = MALLOC> class SimpleSuffixArray {
  private:
	std::span<const T> text;
	Vector<size_t, AT> sa;

  public:
	SimpleSuffixArray() {}

	SimpleSuffixArray(std::span<const T> string) : text(std::move(string)), sa(SAConstructByGrebnovSAIS<T, AT>(text)) {
		assert(text.data()[text.size() - 1] == std::numeric_limits<T>::max() && "Missing $-terminator");
	}

	LInterval<size_t> find(std::span<const T> pattern) const { return acceleratedBinarySearch(pattern, 0, text.size(), 0, 0); }

	std::span<const T> getText() const { return text; }

	const Vector<size_t, AT> &getSA() const { return sa; }

	size_t bitCount() const {
		return sizeof(*this) * 8
			+ sa.bitCount() - sizeof(sa) * 8;
	}

  private:
	LInterval<size_t> acceleratedBinarySearch(std::span<const T> pattern, size_t l, size_t r, size_t llcp, size_t rlcp) const {
		if (r <= l) return {l, r};
		size_t c = l + (r - l) / 2;

		size_t i = min(llcp, rlcp);
		for (; i < pattern.size() && sa[c] + i < text.size(); i++) {
			if (pattern[i] < text[sa[c] + i]) return acceleratedBinarySearch(pattern, l, c, llcp, i);
			if (pattern[i] > text[sa[c] + i]) return acceleratedBinarySearch(pattern, c + 1, r, i, rlcp);
		}

		auto left = acceleratedBinarySearch(pattern, l, c, llcp, i);
		auto right = acceleratedBinarySearch(pattern, c + 1, r, i, rlcp);
		return {min(c, left.from), max(c, right.to)};
	}

	friend std::ostream &operator<<(std::ostream &os, const SimpleSuffixArray<T, AT> &ds) { return os << ds.text << ds.sa; }
	friend std::istream &operator>>(std::istream &is, SimpleSuffixArray<T, AT> &ds) { return is >> ds.text >> ds.sa; }

#ifdef DEBUG
  public:
	void print_stats() {}
	void reset_stats() {}
#endif
};

} // namespace zarr
