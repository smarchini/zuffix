#pragma once

#include "../random/xoroshiro128plus_engine.hpp"
#include "common.hpp"
#include <memory>
#include <random>

namespace zuffix {

class CyclicHash {
public:
  typedef xoroshiro128plus_engine::result_type seed_type;

private:
  const uint64_t Mask;
  xoroshiro128plus_engine Rng;
  uint64_t Map[256];

public:
  explicit CyclicHash(uint64_t w) : CyclicHash(w, std::random_device{}()) {}

  CyclicHash(uint64_t w, seed_type seed) : Mask((1ULL << w) - 1), Rng(seed) {
    for (size_t i = 0; i < 256; i++)
      Map[i] = Rng();
  }

  void seed(seed_type seed) { Rng.seed(seed); }

  uint64_t operator()(const void *message, size_t length, seed_type seed) {
    this->seed(seed);
    return this->operator()(message, length);
  }

  uint64_t operator()(const void *message, size_t length) {
		uint64_t h = 0;

		for(size_t i = 0; i < length; i++)
			h ^= rotl(Map[static_cast<const char*>(message)[i] & 0xFF], i);

		return (h ^ murmur_hash_3(length)) & Mask;
  }

private:
  static inline uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (32 - k)); }
};

} // namespace zuffix
