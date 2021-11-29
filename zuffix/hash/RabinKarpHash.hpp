#pragma once

#include "functions.hpp"
#include <sux/util/Vector.hpp>

namespace zarr {
using ::sux::util::Vector;

template <typename T> class RabinKarpHash {
  public:
	const uint64_t m = 0x100000001b3;      // FNV prime
	const uint64_t d = 0xce965057aff6957b; // md = 1 mod 2^64

  private:
	const T *string;
	Vector<uint64_t> statetable;
	static constexpr size_t C = 1 << 6;
	uint64_t state = 0;
	const uint8_t *l, *r;
	uint64_t pow = 1;

  public:
	RabinKarpHash(T *string) : string(string), statetable(0), l(reinterpret_cast<const uint8_t *>(string)), r(reinterpret_cast<const uint8_t *>(string)) {}

	RabinKarpHash(T *string, size_t length) : string(string), statetable(length / C + 1), l(reinterpret_cast<const uint8_t *>(string)), r(reinterpret_cast<const uint8_t *>(string)) {
		uint64_t hash = 0;
		const uint8_t *b = reinterpret_cast<const uint8_t *>(string);
		const uint8_t *e = reinterpret_cast<const uint8_t *>(string + length);
		for (size_t i = 0; b < e; b++, i++) {
			if (i % C == 0) statetable[i / C] = hash;
			hash = hash * m + b[0];
		}
	}

	uint64_t operator()(size_t to) {
		if (statetable.size() == 0) return (*this)(0, to);
		uint64_t hash = statetable[to / C];
		const uint8_t *b = reinterpret_cast<const uint8_t *>(string + (to / C) * C);
		const uint8_t *e = reinterpret_cast<const uint8_t *>(string + to);
		for (; b < e; b++) hash = hash * m + b[0];
		return hash ^ fmix64(to);
	}

	uint64_t operator()(size_t from, size_t length) {
		const uint8_t *b = reinterpret_cast<const uint8_t *>(string + from);
		const uint8_t *e = reinterpret_cast<const uint8_t *>(string + from + length);

		for (; l < b; l++, pow *= d) state -= l[0] * pow * d;
		for (; l > b; l--, pow *= m) state += l[-1] * pow;
		for (; r < e; r++, pow *= m) state = state * m + r[0];
		for (; r > e; r--, pow *= d) state = (state - r[-1]) * d;

		l = reinterpret_cast<const uint8_t *>(string + from);
		r = reinterpret_cast<const uint8_t *>(string + from + length);
		return state ^ fmix64(length);
	}

	uint64_t immediate(size_t from, size_t length) const {
		uint64_t result = 0;
		const uint8_t *b = reinterpret_cast<const uint8_t *>(string + from);
		const uint8_t *e = reinterpret_cast<const uint8_t *>(string + from + length);
		for (; b < e; b++) result = result * m + (*b);
		return result ^ fmix64(length);
	}
};

} // namespace zarr
