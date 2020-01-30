#pragma once

#include <memory>
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <zuffix/util/String.hpp>
#include <zuffix/random/xoroshiro128plus_engine.hpp>

constexpr char dnabase[] = {'A', 'C', 'G', 'T'};

zarr::String<SYMBOLTYPE> file_to_string(std::string filename, bool dollar=false) {
  std::ifstream file(filename, std::ios::in);
  file.seekg(0, file.end);
  int filesize = file.tellg();
  file.seekg(0, file.beg);

  if (filesize % sizeof(SYMBOLTYPE) != 0) {
    std::cerr << "Bad file or wrong type: text file can't contain half-symbols\n";
    exit(-1);
  }

  std::unique_ptr<char[]> buffer(new char[filesize]);
  file.read(buffer.get(), filesize);
  return zarr::String<SYMBOLTYPE>(buffer.get(), filesize, dollar);
}

zarr::String<char> random_dna(size_t m, bool dollar=false) {
  zarr::String<char> result(m, dollar);

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
