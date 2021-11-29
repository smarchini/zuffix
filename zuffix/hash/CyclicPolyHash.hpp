#pragma once

#include "functions.hpp"
#include <sux/util/Vector.hpp>

namespace zarr {
using ::sux::util::Vector;

template <typename T, size_t sigma> class CyclicPolyHash {
  private:
	const T *string;
	Vector<uint64_t> statetable;
	static constexpr size_t C = 1 << 6;
	uint64_t state = 0;
	size_t l = 0, r = 0;
	uint64_t h[sigma];

  public:
	CyclicPolyHash(const T *string) : string(string), statetable(0) {
		xoroshiro128plus_engine rng(0); // fixed seed
		std::uniform_int_distribution<uint64_t> dist(0, std::numeric_limits<uint64_t>::max());
		for (size_t i = 0; i < sigma; i++) h[i] = dist(rng);
	}

	CyclicPolyHash(const T *string, size_t length) : string(string), statetable(length / C + 1) {
		xoroshiro128plus_engine rng(0); // fixed seed
		std::uniform_int_distribution<uint64_t> dist(0, std::numeric_limits<uint64_t>::max());
		for (size_t i = 0; i < sigma; i++) h[i] = dist(rng);

		uint64_t hash = 0;
		for (size_t i = 0; i < length; i++) {
			if (i % C == 0) statetable[i / C] = hash;
			hash = rotate(hash ^ h[string[i]], 1);
		}
	}

	uint64_t operator()(size_t to) {
		if (statetable.size() == 0) return (*this)(0, to);
		uint64_t hash = statetable[to / C];
		for (size_t i = (to / C) * C; i < to; i++) hash = rotate(hash ^ h[string[i]], 1);
		return hash ^ fmix64(to);
	}

	uint64_t operator()(size_t from, size_t length) {
		for (; l < from; l++) state = state ^ rotate(h[string[l]], r - l);
		for (; l > from; l--) state = state ^ rotate(h[string[l - 1]], r - l + 1);
		for (; r < from + length; r++) state = rotate(state ^ h[string[r]], 1);
		for (; r > from + length; r--) state = rotate(state, 64 - 1) ^ h[string[r - 1]];
		return state ^ fmix64(length);
	}

	uint64_t immediate(size_t from, size_t length) const {
		uint64_t result = 0;
		for (size_t i = 0; i < length; i++) result = rotate(result ^ h[string[from + i]], 1);
		return result ^ fmix64(length);
	}

  private:
	static inline uint64_t rotate(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
};

} // namespace zarr
