#pragma once

#include "../random/xoroshiro128plus_engine.hpp"

#include <cstdint>
#include <cstring>
#include <random>

namespace zarr {

// MurmurHash3 scrambling function: force all bits of a hash block to avalanche
inline uint64_t fmix64(uint64_t k) {
	k ^= k >> 33;
	k *= 0xff51afd7ed558ccdull;
	k ^= k >> 33;
	k *= 0xc4ceb9fe1a85ec53ull;
	k ^= k >> 33;
	return k;
}

} // namespace zarr
