#pragma once

#include "../xoroshiro128pp.hpp"

#include <gtest/gtest.h>
#include <string>
#include <unordered_set>
#include <zuffix/ZuffixArray.hpp>
#include <zuffix/hash/CyclicHash.hpp>
#include <zuffix/hash/SpookyV2.h>
#include <zuffix/random/xoroshiro128plus_engine.hpp>

uint64_t spooky_hash(const void *message, size_t length) {
  return SpookyHash::Hash64(message, length, 0);
}

uint64_t cyclic_hash(const void *message, size_t length) {
  static zuffix::CyclicHash<zuffix::xoroshiro128plus_engine> hash(0, 37);
  return hash(message, length);
}

uint64_t broken_spooky_hash(const void *message, size_t length) {
  static constexpr size_t mask = (1ULL << 37) - 1;
  return SpookyHash::Hash64(message, length, 0) & mask;
}

sux::util::Vector<char> stringToVector(std::string string) {
  sux::util::Vector<char> vector(string.length());

  for (size_t i = 0; i < string.length(); i++)
    vector[i] = string[i];

  return vector;
}

template <typename T> sux::util::Vector<T> arrayToVector(const T *array, size_t length) {
  sux::util::Vector<T> vector(length);

  for (size_t i = 0; i < length; i++)
    vector[i] = array[i];

  return vector;
}

template <typename T1, typename T2> void print_pair(std::pair<T1, T2> pair) {
  std::cout << pair.first << ", " << pair.second << std::endl;
}

template <typename T>
bool array_equals(const sux::util::Vector<T> &vector, const T *array, size_t length) {
  if (vector.size() != length) return false;

  for (size_t i = 0; i < length; i++) {
    if (vector[i] != array[i]) return false;
  }

  return true;
}

template <typename T, zarr::hash_function HF>
void check(const sux::util::Vector<T> &string, const zarr::ZuffixArray<T, HF> &zuffix,
           const sux::util::Vector<T> &pattern) {
  auto interval = zuffix.find(pattern);
  EXPECT_EQ(zuffix.findExact(pattern), zuffix.find(pattern));

  std::unordered_set<size_t> pos;

  for (size_t j = 0; j < interval.length(); j++) {
    size_t p = zuffix.getSuffix()[interval.from + j];
    pos.insert(p);
    // EXPECT_EQ
  }

  int64_t limit = string.size() - pattern.size();
  for (int64_t p = 0; p < limit; p++)
    EXPECT_EQ(array_equals(pattern, &string + p, pattern.size()), pos.find(p) != std::end(pos));
}

sux::util::Vector<int8_t> dna(size_t length) {
  sux::util::Vector<int8_t> ret(length);

  for (size_t i = 0, c = next() & 0b11; i < length; i++) {
    if (next() < UINT64_MAX / 4) c = next() & 0b11;
    ret[i] = c;
  }

  return ret;
}

TEST(zuffix, abracadabra) {
  std::string abracadabra("ABRACADABRA");
  zarr::ZuffixArray<char, spooky_hash> zuffix(stringToVector(abracadabra));
  zuffix.visitPre(0, abracadabra.length(), 0, 0);
}

TEST(zuffix, test) {
  int8_t s[] = {0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1};
  zarr::ZuffixArray<int8_t, spooky_hash> zuffix(arrayToVector(s, sizeof(s) / sizeof(s[0])));
  zuffix.visitPre(0, sizeof(s) / sizeof(s[0]), 0, 0);

  int8_t _v[] = {0, 1, 0, 1};
  auto v = arrayToVector(_v, sizeof(_v) / sizeof(_v[0]));
  std::cout << "find: " << zuffix.find(v) << std::endl;
  std::cout << "findExact: " << zuffix.findExact(v) << std::endl;
}

TEST(zuffix, test_all) {
  int8_t s_[] = {0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1};
  sux::util::Vector<int8_t> s = arrayToVector(s_, sizeof(s_) / sizeof(s_[0]));
  zarr::ZuffixArray<int8_t, spooky_hash> zuffix(std::move(s));

  for (size_t l = 1; l < 15; l++) {
    for (size_t i = 0; i < (1ULL << l); i++) {
      sux::util::Vector<int8_t> v(l);

      for (size_t j = 0; j < l; j++)
        v[j] = (i & (1ULL << j)) != 0;

      check(s, zuffix, v);
    }
  }
}

TEST(zuffix, test_const) {
  sux::util::Vector<int8_t> s(5);
  zarr::ZuffixArray<int8_t, spooky_hash> zuffix(std::move(s));

  for (size_t l = 1; l < 7; l++) {
    for (size_t i = 0; i < (1ULL << l); i++) {
      sux::util::Vector<int8_t> v(l);

      for (size_t j = 0; j < l; j++)
        v[j] = (i & (1ULL << j)) != 0;

      check(s, zuffix, v);
    }
  }
}

TEST(zuffix, test_distinct) {
  sux::util::Vector<int8_t> s(4);

  for (size_t i = 0; i < s.size(); i++)
    s[i] = i;

  zarr::ZuffixArray<int8_t, spooky_hash> zuffix(std::move(s));

  for (size_t l = 1; l < 6; l++) {
    for (size_t i = 0; i < (1ULL << 2 * l); i++) {
      sux::util::Vector<int8_t> v(l);

      for (size_t j = 0; j < l; j++)
        v[j] = i >> j * 2 & 3;

      check(s, zuffix, v);
    }
  }
}

TEST(zuffix, dna_1000) {
  auto s = dna(1000);
  zarr::ZuffixArray<int8_t, spooky_hash> zuffix(std::move(s));

  for (size_t t = 1; t < 100000; t++) {
    auto v = dna(1 + next() % (s.size() + 4));
    check(s, zuffix, v);
  }
}

TEST(zuffix, dna_all) {
  auto s = dna(10);
  zarr::ZuffixArray<int8_t, spooky_hash> zuffix(std::move(s));

  for (size_t l = 1; l < 12; l++) {
    for (size_t i = 0; i < (1ULL << 2 * l); i++) {
      sux::util::Vector<int8_t> v(l);

      for (size_t j = 0; j < l; j++)
        v[j] = i >> j * 2 & 3;

      check(s, zuffix, v);
    }
  }
}

TEST(zuffix, dna_collisions_spooky) {
  auto s = dna(1000000);
  size_t mask = (1ULL << 37) - 1;
  // we need to typedef this thing to make gtest macro trickeries happy
  typedef zarr::ZuffixArray<int8_t, broken_spooky_hash> zuffix_type;
  ASSERT_DEATH(zuffix_type zuffix(std::move(s)), "The element alaredy exists");
}

TEST(zuffix, dna_collisions_cyclic) {
  auto s = dna(1000000);
  // we need to typedef this thing to make gtest macro trickeries happy
  typedef zarr::ZuffixArray<int8_t, cyclic_hash> zuffix_type;
  ASSERT_DEATH(zuffix_type zuffix(std::move(s)), "The element alaredy exists");
}
