#pragma once

#include <iostream>
using namespace std;

#include "../random/xoroshiro128plus_engine.hpp"
#include <random>

namespace zarr {

template <typename T, size_t sigma> class CyclicPolyHash {
  private:
	T *string;
	uint64_t state = 0;
	size_t l = 0, r = 0;
	uint64_t h[sigma];

  public:
	CyclicPolyHash(T *string, uint64_t seed = 3) : string(string) {
		xoroshiro128plus_engine rng(seed);
		std::uniform_int_distribution<uint64_t> dist(0, std::numeric_limits<uint64_t>::max());
		for (size_t i = 0; i < sigma; i++) h[i] = dist(rng);
	}

	uint64_t operator()(size_t from, size_t length) {
		for (; l < from; l++) state = state ^ rot(h[string[l]], r - l);
		for (; l > from; l--) state = state ^ rot(h[string[l - 1]], r - l + 1);
		for (; r < from + length; r++) state = rot(state ^ h[string[r]], 1);
		for (; r > from + length; r--) state = rot(state, 64 - 1) ^ h[string[r - 1]];
		return state;
	}

  private:
	static inline uint64_t rot(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
};

} // namespace zarr
