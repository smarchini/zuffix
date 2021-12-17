#pragma once

#include "functions.hpp"
#include <sux/util/Vector.hpp>

namespace zarr {
using ::sux::util::Vector;

template <typename T> class O1Hash {
  private:
	const T *string;

  public:
	O1Hash(T *string) : string(string) {
		const char *s1 = "abcd99999";
		const char *s2 = "abcd00000";
		auto a = o1hash(s1, 4);
		auto b = o1hash(s2, 4);
		assert(a == b);
	}
	uint64_t operator()(size_t to) { return immediate(0, to); }
	uint64_t operator()(size_t from, size_t length) { return immediate(from, length); }
	uint64_t immediate(size_t from, size_t length) const { return o1hash(string + from, length * sizeof(T)) ^ fmix64(length); }

  private:
	static inline unsigned o1r4(const uint8_t *p) {
		unsigned v;
		memcpy(&v, p, 4);
		return v;
	}

	static inline uint64_t o1hash(const void *key, size_t len) {
		const uint8_t *p = (const uint8_t *)key;
		// if (len >= 8) {
		// 	unsigned first = o1r4(p), middle = o1r4(p + (len >> 1) - 2), last = o1r4(p + len - 4);
		// 	return (uint64_t)(first + last) * middle * (uint64_t)key * len;
		// }
		if (len) {
			uint64_t tail = ((((unsigned)p[0]) << 16) | (((unsigned)p[len >> 1]) << 8) | p[len - 1]);
			return tail * 0xa0761d6478bd642full;
		}
		return 0;
	}
};

} // namespace zarr
