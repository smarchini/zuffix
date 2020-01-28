#pragma once

#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <sux/util/Vector.hpp>
#include <zuffix/ZuffixArray.hpp>
#include <SpookyV2.h>
#include <sais.hxx>
#include <zuffix/random/xoroshiro128plus_engine.hpp>

constexpr char dnabase[] = {'A', 'C', 'G', 'T'};

uint64_t spooky_hash(const void *message, size_t length) {
  return SpookyHash::Hash64(message, length, 0);
}

zarr::String<char> file_to_string(std::string filename) {
  std::ifstream file(filename, std::ios::in);
  std::stringstream strstream;
  strstream << file.rdbuf();
  return zarr::String<char>(strstream.str());
}

zarr::String<char> random_dna(size_t m) {
  zarr::String<char> result(m);

  static std::random_device rd;
  static zarr::xoroshiro128plus_engine rng(rd());
  std::uniform_int_distribution<char> dist(0, 4);

  for (size_t i = 0; i < m; i++)
    result[i] = dnabase[dist(rng)];

  return result;
}

template <class T> std::string pretty(T value) {
  std::stringstream ss;
  ss.imbue(std::locale(""));
  ss << std::fixed << value;
  return ss.str();
}
