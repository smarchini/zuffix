#pragma once

#include "functions.hpp"
#include <sux/util/Vector.hpp>

namespace zarr {
using ::sux::util::Vector;

template <typename T, size_t sigma, size_t C = 1 << 12> class CyclicPolyHash {
  private:
	const T *string;
	Vector<uint64_t> statetable;
	uint64_t state = 0;
	size_t l = 0, r = 0;
	uint64_t h[sigma];

  public:
	// TODO kinda bad doing this thing at runtime
	// when it will be fixed, recompute the best value of C
	CyclicPolyHash(const T *string) : string(string), statetable(1) {
		std::mt19937 rng(2023); // fixed seed
		std::uniform_int_distribution<uint64_t> dist(0, std::numeric_limits<uint64_t>::max());
		for (size_t i = 0; i < sigma; i++) h[i] = dist(rng);
		statetable[0] = 0;
	}

	uint64_t operator()(size_t to) {
		const uint8_t *str = reinterpret_cast<const uint8_t *>(string);
		const size_t length = to * sizeof(T);

		size_t tpos = to / C;
		if (statetable.size() <= tpos) {
			const size_t last = statetable.size() - 1;
			uint64_t hash = statetable[last];
			statetable.resize(tpos + 1);
			for (size_t i = last * C + 1; i <= length; i++) {
				hash = rotate(hash ^ h[str[i - 1]], 1);
				if (i % C == 0) statetable[i / C] = hash;
			}
			return hash ^ fmix64(length);
		}

		uint64_t hash = statetable[tpos];
		for (size_t i = tpos * C; i < length; i++) hash = rotate(hash ^ h[str[i]], 1);
		return hash ^ fmix64(length);
	}

	uint64_t operator()(size_t from, size_t length) {
		const uint8_t *str = reinterpret_cast<const uint8_t *>(string);
		from = from * sizeof(T);
		length = length * sizeof(T);

		for (; l < from; l++) state = state ^ rotate(h[str[l]], r - l);
		for (; l > from; l--) state = state ^ rotate(h[str[l - 1]], r - l + 1);
		for (; r < from + length; r++) state = rotate(state ^ h[str[r]], 1);
		for (; r > from + length; r--) state = rotate(state, 64 - 1) ^ h[str[r - 1]];
		return state ^ fmix64(length);
	}

	uint64_t immediate(size_t from, size_t length) const {
		const uint8_t *str = reinterpret_cast<const uint8_t *>(string);
		from = from * sizeof(T);
		length = length * sizeof(T);

		uint64_t result = 0;
		for (size_t i = 0; i < length; i++) result = rotate(result ^ h[str[from + i]], 1);
		return result ^ fmix64(length);
	}

    size_t bitCount() const { return sizeof(*this) * 8 + statetable.bitCount() - sizeof(statetable) * 8; }

  private:
	static inline uint64_t rotate(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
};

} // namespace zarr
