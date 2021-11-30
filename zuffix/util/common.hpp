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

template <typename T> inline Vector<ssize_t> LCPConstructionByKarkkainenPsi(const String<T> &string, const Vector<size_t> &sa) {
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

} // namespace zarr
