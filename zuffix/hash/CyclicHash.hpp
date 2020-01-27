#pragma once

#include "../random/xoroshiro128plus_engine.hpp"
#include "common.hpp"
#include <memory>
#include <random>

namespace zarr {

template <typename RE>
class CyclicHash {
public:
  typedef typename RE::result_type seed_type;

private:
  const uint64_t Mask;
  RE Rng;
  uint64_t Map[256];

public:
  explicit CyclicHash(uint64_t w) : CyclicHash(w, std::random_device{}()) {}

  CyclicHash(uint64_t w, seed_type seed) : Mask((1ULL << w) - 1), Rng(seed) {
    for (size_t i = 0; i < 256; i++)
      Map[i] = Rng();
  }

  void seed(seed_type seed) { Rng.seed(seed); }

  uint64_t operator()(const void *message, size_t length) {
		uint64_t h = 0;

		for(size_t i = 0; i < length; i++)
			h ^= rotl(Map[static_cast<const char*>(message)[i] & 0xFF], i);

		return (h ^ murmur_hash_3(length)) & Mask;
  }

private:
  static inline uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (32 - k)); }

  static inline uint64_t murmur_hash_3(uint64_t x) {
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccdL;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53L;
    x ^= x >> 33;
    return x;
  }
};

} // namespace zuffix
