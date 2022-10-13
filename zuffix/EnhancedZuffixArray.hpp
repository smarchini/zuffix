#pragma once

#include <sux/support/common.hpp>
#include <sux/util/Vector.hpp>

#include "util/LInterval.hpp"
#include "util/LinearProber.hpp"
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
	LinearProber<uint64_t> z;
	size_t maxhlen = 0;

  public:
	EnhancedZuffixArray() {}

	EnhancedZuffixArray(String<T> string) : text(std::move(string)), sa(SAConstructByGrebnovSAIS(text)), lcp(LCPConstructByKarkkainenPsi(text, sa)), ct(CTConstructByAbouelhoda(lcp)) {
		// z.resize(ceil_pow2(text.length()) << 1); // TODO tweak me
		RH<T> hash(&text);
		ZFillByDFS(0, text.length(), 0, hash);
		// ZFillByBottomUp();
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
			size_t f = twoFattestR(l, r);
			LInterval<size_t> beta = unpack(z[h(f)].value_or(0x100000000));
			size_t elen = getlcp(beta.from, beta.to) + 1;
			size_t nlen = 1 + max(lcp[beta.from], lcp[beta.to]);
			size_t hlen = twoFattestLR(nlen, elen);
			if (beta.isEmpty() || hlen != f) {
				r = f - 1;
			} else if (!alpha.contains(beta)) {
				l = elen;
			} else {
				l = elen - 1;
				alpha = beta;
			}
		}
		size_t nlen = 1 + max(lcp[alpha.from], lcp[alpha.to]);
		size_t end = min(nlen, pattern.length());
		if (memcmp(&pattern, &text + sa[alpha.from], end * sizeof(T))) {
			return {0, text.length()}; // mischivious collision
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

	void ZFillByDFS(size_t i, size_t j, size_t nlen, RH<T> &h, size_t dept = 0) {
		if (j - i <= 1) return; // leaves are not in the z-map
		size_t l = i;
		size_t r = i < ct[j - 1] && ct[j - 1] < j ? ct[j - 1] : ct[i];
		ssize_t elen = lcp[r];
		size_t hlen = twoFattestLR(nlen, elen);
		if (maxhlen <= hlen) maxhlen = hlen;
		assert(dept <= hlen);

		z.store(h.immediate(sa[i], hlen), pack({i, j}));
		if (z.elements() * 3 > z.size()) growZTable();

		do {
			ZFillByDFS(l, r, elen + 1, h, dept + 1);
			l = r;
			r = ct[r];
		} while (lcp[l] == lcp[ct[l]] && lcp[l] <= lcp[l + 1]);
		ZFillByDFS(l, j, elen + 1, h, dept + 1);
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
				if (maxhlen <= hlen) maxhlen = hlen;

				z.store(h.immediate(sa[intervali], hlen), pack({intervali, intervalj}));
				if (z.elements() * 3 > z.size()) growZTable();

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
		RH<T> h(&text);
		size_t n = z.size();
		auto table = z.getTable();
		LinearProber<uint64_t> zlarge(n * 2);
		for (size_t i = 0; i < n; i++) {
			auto [signature, value] = table[i];
			if (signature) {
				auto [intervali, intervalj] = unpack(value);
				ssize_t nlen = 1 + max(lcp[intervali], lcp[intervalj]);
				ssize_t elen = getlcp(intervali, intervalj);
				size_t hlen = twoFattestLR(nlen, elen);
				zlarge.store(h.immediate(sa[intervali], hlen), value);
			}
		}
		std::swap(z, zlarge);
	}

	uint64_t pack(LInterval<size_t> x) const { return x.from << 32 | x.to; }
	LInterval<size_t> unpack(uint64_t x) const { return {x >> 32, x & 0xffffffff}; }

	friend std::ostream &operator<<(std::ostream &os, const EnhancedZuffixArray<T, RH> &ds) { return os << ds.text << ds.sa << ds.lcp << ds.ct << ds.z << ds.maxhlen; }
	friend std::istream &operator>>(std::istream &is, EnhancedZuffixArray<T, RH> &ds) { return is >> ds.text >> ds.sa >> ds.lcp >> ds.ct >> ds.z >> ds.maxhlen; }
};

} // namespace zarr
