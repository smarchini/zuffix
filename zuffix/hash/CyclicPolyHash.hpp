#pragma once

#include <iostream>
using namespace std;

#include "../random/xoroshiro128plus_engine.hpp"
#include <random>

namespace zarr {

// TODO scrivere unit test per le rolling hash functions
template <typename T, size_t sigma> class CyclicPolyHash {
  private:
	T *string;
	uint64_t state = 0;
	size_t lpos = 0, rpos = 0;
	uint64_t h[sigma];

  public:
	CyclicPolyHash(T *string, uint64_t seed = 43) : string(string) {
		xoroshiro128plus_engine rng(seed);
		std::uniform_int_distribution<uint64_t> dist(0, std::numeric_limits<uint64_t>::max());
		for (size_t i = 0; i < sigma; i++) h[i] = dist(rng);
	}

	uint64_t operator()(size_t from, size_t length) {
		for (; lpos < from; lpos++) state ^= rotl(h[string[lpos]], lpos);
		for (; lpos > from; lpos--) state ^= rotl(h[string[lpos]], lpos);
		for (; rpos < from + length; rpos++) state ^= rotl(h[string[rpos]], rpos);
		for (; rpos > from + length; rpos--) state ^= rotl(h[string[rpos]], rpos);
        //cout << "hash(" << from << ", " << length << ") = " << state << endl;
		return state;
	}

  private:
	static inline uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
};

} // namespace zarr
