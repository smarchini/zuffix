#pragma once

#include "String.hpp"
#include <cstdint>
#include <stack>
#include <sux/util/Vector.hpp>

namespace zarr {

using namespace sux;
using namespace sux::util;

/** Hash function type
 *
 * Prefer template parameters to function pointers, so the compiler can perform
 * inlining.
 */
typedef uint64_t (*hash_function)(const void *, size_t);

/** 2-fattest number in the (a, b] interval
 *
 * The two fattest number within an interval is the number with most trailing
 * zeros in binary.
 *
 * @return 2-fattest number in (a, b], undefined if a = b
 */
inline int64_t twoFattest(size_t a, size_t b) { return (1L << 63) >> __builtin_clzll(a ^ b) & b; }

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

} // namespace zarr
