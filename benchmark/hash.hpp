#pragma once

#include <SpookyV2.h>
#include <random>
#include <zuffix/hash/CyclicHash.hpp>
#include <zuffix/random/xoroshiro128plus_engine.hpp>

uint64_t spooky_hash(const void *message, size_t length) {
  return SpookyHash::Hash64(message, length, 0);
}

uint64_t cyclic_hash(const void *message, size_t length) {
  static zarr::CyclicHash<zarr::xoroshiro128plus_engine> hash(0, 37);
  return hash(message, length);
}

uint64_t broken_spooky_hash(const void *message, size_t length) {
  static constexpr size_t mask = (1ULL << 37) - 1;
  return SpookyHash::Hash64(message, length, 0) & mask;
}
