#pragma once

#include <sux/support/common.hpp>
#include <sux/util/Vector.hpp>

#include "util/LInterval.hpp"
#include "util/common.hpp"

namespace zarr {
using ::sux::util::AllocType;
using ::sux::util::Vector;

template <typename T, AllocType AT = MALLOC> class EnhancedSuffixArray {
  private:
	std::span<const T> text;
	Vector<size_t, AT> sa;
	Vector<ssize_t, AT> lcp;
	Vector<size_t, AT> ct;

  public:
	EnhancedSuffixArray() {}

	EnhancedSuffixArray(std::span<const T> string)
		: text(std::move(string)),
		  sa(SAConstructByGrebnovSAIS<T, AT>(text)),
		  lcp(LCPConstructByKarkkainenPsi<T, AT>(text, sa)),
		  ct(CTConstructByAbouelhoda<AT>(lcp)) {
		assert(text.data()[text.size() - 1] == std::numeric_limits<T>::max() && "Missing $-terminator");
	}

	LInterval<size_t> getChild(size_t i, size_t j, const T &c) const {
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

	LInterval<size_t> find(std::span<const T> pattern) const {
		size_t c = 0;
		size_t i = 0, j = text.size();
		while (c < pattern.size() && i < j) {
			std::tie(i, j) = getChild(i, j, pattern[c]);

			if (j - i == 1) {
				if (memcmp(pattern.data() + c, text.data() + sa[i] + c, (pattern.size() - c) * sizeof(T))) return {1, 0};
				break;
			}

			ssize_t d = min(getlcp(i, j), static_cast<ssize_t>(pattern.size()));
			if (memcmp(pattern.data() + c, text.data() + sa[i] + c, (d - c) * sizeof(T))) return {1, 0};
			c = d;
		}
		return {i, j};
	}

	LInterval<size_t> find_prefix(std::span<const T> pattern) const {
		size_t c = 0;
		size_t i = 0, j = text.size();
		while (c < pattern.size() && i < j) {
			std::tie(i, j) = getChild(i, j, pattern[c]);

			if (j - i == 1) {
				if (memcmp(pattern.data() + c, text.data() + sa[i] + c, sizeof(T))) return {1, 0};
				break;
			}

			ssize_t d = min(getlcp(i, j), static_cast<ssize_t>(pattern.size()));
			if (memcmp(pattern.data() + c, text.data() + sa[i] + c, (d - c) * sizeof(T))) return {1, 0};
			c = d;
		}
		return {i, j};
	}

	std::span<const T> getText() const { return text; }

	const Vector<size_t, AT> &getSA() const { return sa; }

	const Vector<ssize_t, AT> &getLCP() const { return lcp; }

	const Vector<size_t, AT> &getCT() const { return ct; }

	size_t bitCount() const {
		return sizeof(*this) * 8
			+ sa.bitCount() - sizeof(sa) * 8
			+ lcp.bitCount() - sizeof(lcp) * 8
			+ ct.bitCount() - sizeof(ct) * 8;
	}

  private:
	inline ssize_t getlcp(size_t i, size_t j) const { return lcp[i < ct[j - 1] && ct[j - 1] < j ? ct[j - 1] : ct[i]]; }

	friend std::ostream &operator<<(std::ostream &os, const EnhancedSuffixArray<T, AT> &ds) { return os << ds.text << ds.sa << ds.lcp << ds.ct; }
	friend std::istream &operator>>(std::istream &is, EnhancedSuffixArray<T, AT> &ds) { return is >> ds.text >> ds.sa >> ds.lcp >> ds.ct; }

#ifdef DEBUG
  public:
	void print_stats() {}
	void reset_stats() {}
#endif
};

} // namespace zarr
