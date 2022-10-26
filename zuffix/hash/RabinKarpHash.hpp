#pragma once

#include "functions.hpp"
#include <sux/util/Vector.hpp>

namespace zarr {
using ::sux::util::Vector;

template <typename T, size_t C = 1 << 8> class RabinKarpHash {
  public:
	const uint64_t m = 0x100000001b3;      // FNV prime
	const uint64_t d = 0xce965057aff6957b; // md = 1 mod 2^64

  private:
	const T *string;
	Vector<uint64_t> statetable;
	uint64_t state = 0;
	const uint8_t *l, *r;
	uint64_t pow = 1;

  public:
	RabinKarpHash(T *string) : string(string), statetable(1), l(reinterpret_cast<const uint8_t *>(string)), r(reinterpret_cast<const uint8_t *>(string)) { statetable[0] = 0; }

	uint64_t operator()(size_t to) {
		const uint8_t *b = reinterpret_cast<const uint8_t *>(string);
		const uint8_t *e = reinterpret_cast<const uint8_t *>(string + to);

		size_t tpos = to / C;
		if (statetable.size() <= tpos) {
			const size_t last = statetable.size() - 1;
			const uint8_t *p = b + last * C;
			uint64_t hash = statetable[last];
			statetable.resize(tpos + 1);
			for (size_t i = last * C + 1; p < e; p++, i++) {
				hash = hash * m + p[0];
				if (i % C == 0) statetable[i / C] = hash;
			}
			return hash ^ fmix64(to);
		}

		uint64_t hash = statetable[tpos];
		for (const uint8_t *p = b + tpos * C; p < e; p++) hash = hash * m + p[0];
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
