#pragma once

#include "functions.hpp"
#include <sux/util/Vector.hpp>

namespace zarr {
using ::sux::util::AllocType;

template <typename T, AllocType AT = sux::util::MALLOC> class O1Hash {
  public:
	using signature_t = uint64_t;

  private:
	const T *string;

  public:
	O1Hash() : string(nullptr) {}
	O1Hash(const T *string, size_t size) : string(string) {}
    void setString(const T *s) { string = s; }
    void reserve(size_t size) { }
	signature_t operator()(size_t to) { return (*this)(0, to); }
	signature_t operator()(size_t from, size_t length) { return o1hash(string + from, length * sizeof(T)) ^ fmix64(length); }
	signature_t immediate(size_t from, size_t length) const { return o1hash(string + from, length * sizeof(T)) ^ fmix64(length); }

  private:
	static inline unsigned o1r4(const uint8_t *p) {
		unsigned v;
		std::memcpy(&v, p, 4);
		return __builtin_bswap32(v);
	}

	static inline signature_t o1hash(const void *key, size_t len) {
		const uint8_t *p = (const uint8_t *)key;
		if (len >= 4) {
			unsigned first = o1r4(p), middle = o1r4(p + (len >> 1) - 2), last = o1r4(p + len - 4);
			return (uint64_t)(first + last) * middle * (uint64_t)key * len;
		}
		if (len) {
			uint64_t tail = ((((unsigned)p[0]) << 16) | (((unsigned)p[len >> 1]) << 8) | p[len - 1]);
			return tail * 0xa0761d6478bd642full;
		}
		return 0;
	}

	size_t bitCount() const { return sizeof(*this) * 8; }
};

} // namespace zarr
