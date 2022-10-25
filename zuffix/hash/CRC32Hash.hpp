#pragma once

#include "functions.hpp"
#include "sse2.hpp"
#include <sux/util/Vector.hpp>

#define CALC_CRC(op, crc, type, buf, len)                                                                                                                                                              \
	do {                                                                                                                                                                                               \
		for (; (len) >= sizeof(type); (len) -= sizeof(type), buf += sizeof(type)) {                                                                                                                    \
			(crc) = op((crc), *(type *)(buf));                                                                                                                                                         \
		}                                                                                                                                                                                              \
	} while (0)

namespace zarr {
using ::sux::util::Vector;

template <typename T> class CRC32Hash {
  private:
	const T *string;
	Vector<uint64_t> statetable;
	uint64_t state = 0;
	const uint8_t *l, *r;

  public:
	CRC32Hash(T *string) : string(string), l(reinterpret_cast<const uint8_t *>(string)), r(reinterpret_cast<const uint8_t *>(string)) {}

	uint64_t operator()(size_t to) { return (*this)(0, to); }

	uint64_t operator()(size_t from, size_t length) {
		state = 0;
		const uint8_t *b = reinterpret_cast<const uint8_t *>(string + from);
		const uint8_t *e = reinterpret_cast<const uint8_t *>(string + from + length);

		// align b and e to word
		for (; b < e && (uint64_t)b & 0b111; b++) state = _mm_crc32_u8(state, *b);
		for (; b < e && (uint64_t)e & 0b111; e--) state = _mm_crc32_u8(state, *e);

		intptr_t distance = e - b;
#if defined(__x86_64__) || defined(__aarch64__)
		CALC_CRC(_mm_crc32_u64, state, uint64_t, b, distance);
#endif
		CALC_CRC(_mm_crc32_u32, state, uint32_t, b, distance);
		CALC_CRC(_mm_crc32_u16, state, uint16_t, b, distance);
		CALC_CRC(_mm_crc32_u8, state, uint8_t, b, distance);

		l = reinterpret_cast<const uint8_t *>(string + from);
		r = reinterpret_cast<const uint8_t *>(string + from + length);
		return state;
	}

	uint64_t immediate(size_t from, size_t length) const {
		uint64_t result = 0;
		const uint8_t *b = reinterpret_cast<const uint8_t *>(string + from);
		const uint8_t *e = reinterpret_cast<const uint8_t *>(string + from + length);

		// align b and e to word
		for (; b < e && (uint64_t)b & 0b111; b++) result = _mm_crc32_u8(result, *b);
		for (; b < e && (uint64_t)e & 0b111; e--) result = _mm_crc32_u8(result, *e);

		intptr_t distance = e - b;
#if defined(__x86_64__) || defined(__aarch64__)
		CALC_CRC(_mm_crc32_u64, result, uint64_t, b, distance);
#endif
		CALC_CRC(_mm_crc32_u32, result, uint32_t, b, distance);
		CALC_CRC(_mm_crc32_u16, result, uint16_t, b, distance);
		CALC_CRC(_mm_crc32_u8, result, uint8_t, b, distance);

		return result;
	}
};

} // namespace zarr
