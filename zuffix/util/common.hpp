#pragma once

#include "String.hpp"
#include <cstdint>
#include <divsufsort64.h>
#include <libsais64.h>
#include <stack>
#include <sux/util/Vector.hpp>

namespace zarr {

using namespace sux;
using namespace sux::util;

/** Next poewr of two greater than or euqual to x
 *
 * @return the next power of two >= x or zero if x is zero
 */
static inline uint64_t ceil_pow2(uint64_t x) {
	if (x == 0) return 0;
	int clz = __builtin_clzll(x);
	return (1ULL << 63) >> (clz - ((x & (-1ULL >> 1) >> clz) != 0));
}

/** 2-fattest number in the (a, b] interval
 *
 * The two fattest number within an interval is the number with most trailing
 * zeros in binary.
 *
 * @return 2-fattest number in (a, b], undefined if a = b
 */
inline int64_t twoFattestR(size_t a, size_t b) { return (1L << 63) >> __builtin_clzll(a ^ b) & b; }

/** 2-fattest number in the [a, b] interval
 *
 * The two fattest number within an interval is the number with most trailing
 * zeros in binary.
 *
 * @return 2-fattest number in [a, b], which is 0 if a <= 0 <= b
 */
inline int64_t twoFattestLR(size_t a, size_t b) { return (1L << 63) >> __builtin_clzll((a - 1) ^ b) & b; }

/** Suffix array construction algorithm by explicit sort
 *
 * @return The array SA
 */
template <typename T> inline Vector<size_t> SAConstructBySort(const String<T> &string) {
	size_t n = string.length();
	Vector<size_t> result(n);
	for (size_t i = 0; i < n; i++) result[i] = i;
	std::sort(&result[0], &result[n], [&string](const auto &lhs, const auto &rhs) {
		size_t i = 0;
		while (string[lhs + i] == string[rhs + i]) i++;
		return string[lhs + i] < string[rhs + i];
	});
	return result;
}

/** Suffix array construction algorithm: DivSufSort
 *
 * @return The array SA
 */
template <typename T> inline Vector<size_t> SAConstructByDivSufSort(const String<T> &string) {
	size_t n = string.length();
	Vector<size_t> result(n);
	divsufsort64((const unsigned char *)&string, (long int *)&result, n);
	return result;
}

/** Suffix array construction algorithm: IlyaGrebnov's libsais
 *
 * @return The array SA
 */
template <typename T> inline Vector<size_t> SAConstructByGrebnovSAIS(const String<T> &string) {
	size_t n = string.length();
	Vector<size_t> result(n);
	libsais64((const uint8_t *)&string, (int64_t *)&result, n, 0, nullptr);
	return result;
}

/** LCP array construction algorithm by explicit string comparison
 *
 * @return The array LCP
 */
template <typename T> inline Vector<ssize_t> LCPConstructByStrcmp(const String<T> &string, const Vector<size_t> &sa) {
	size_t n = sa.size();
	Vector<ssize_t> result(n + 1);

	result[0] = -1;
	for (size_t i = 1; i < n; i++) {
		size_t lcp = 0;
		while (string[sa[i] + lcp] == string[sa[i - 1] + lcp]) lcp++;
		result[i] = lcp;
	}
	result[n] = -1;

	return result;
}

/** CT (child table) array construction algorithm by Abouelhoda's paper
 *
 * @return The array CT
 */
inline Vector<size_t> CTConstructByAbouelhoda(const Vector<ssize_t> &lcp) {
	size_t n = lcp.size() - 1;
	Vector<size_t> result(n);
	std::stack<size_t> stack;
	stack.push(0);
	size_t lastIndex = 0;

	for (size_t i = 1; i <= n; i++) {
		while (lcp[i] < lcp[stack.top()]) {
			lastIndex = stack.top();
			stack.pop();
			if (lcp[i] <= lcp[stack.top()] && lcp[stack.top()] != lcp[lastIndex]) result[stack.top()] = lastIndex;
		}
		if (lcp[i] == lcp[stack.top()]) result[stack.top()] = i;
		if (lastIndex != 0) {
			result[i - 1] = lastIndex;
			lastIndex = 0;
		}
		stack.push(i);
	}

	return result;
}

template <typename T> inline Vector<ssize_t> LCPConstructByKarkkainenPsi(const String<T> &string, const Vector<size_t> &sa) {
	// using namespace std;
	size_t n = sa.size();
	Vector<size_t> plcp(n + 1);
	for (size_t i = 0, sai = n; i < n; sai = sa[i], i++) plcp[sa[i]] = sai;
	// for (size_t i = 0; i < n; i++) cout << "plcp[" << i << "] = " <<  plcp[i] << endl;

	size_t maxl = 0;
	for (size_t i = 0, l = 0; i < n; i++) {
		while (string[i + l] == string[plcp[i] + l]) l++;
		plcp[i] = l;
		if (l) {
			maxl = max(maxl, l);
			l--;
		}
	}

	Vector<ssize_t> lcp(n + 1);
	for (size_t i = 1; i < n; i++) lcp[i] = plcp[sa[i]];
	lcp[0] = lcp[n] = -1;
	return lcp;
}

#ifdef NO_AVX_MEMCMP
__attribute__((optimize("no-tree-vectorize")))
int memcmp(const void *a, const void *b, size_t length) {
	size_t pos = 0;

	const uint64_t *a8 = reinterpret_cast<const uint64_t *>(a);
	const uint64_t *b8 = reinterpret_cast<const uint64_t *>(b);
	for (; pos + 8 < length; pos += 8) {
		if (a8[pos >> 3] != b8[pos >> 3]) return 1;
	}

	const uint32_t *a4 = reinterpret_cast<const uint32_t *>(a);
	const uint32_t *b4 = reinterpret_cast<const uint32_t *>(b);
	for (; pos + 4 < length; pos += 4) {
		if (a4[pos >> 2] != b4[pos >> 2]) return 1;
	}

	const uint16_t *a2 = reinterpret_cast<const uint16_t *>(a);
	const uint16_t *b2 = reinterpret_cast<const uint16_t *>(b);
	for (; pos + 2 < length; pos += 2) {
		if (a2[pos >> 1] != b2[pos >> 1]) return 1;
	}

	const uint8_t *a1 = reinterpret_cast<const uint8_t *>(a);
	const uint8_t *b1 = reinterpret_cast<const uint8_t *>(b);
	for (; pos < length; pos++) {
		if (a1[pos] != b1[pos]) return 1;
	}

	return 0;
}
#endif

} // namespace zarr
