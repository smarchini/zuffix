#pragma once

#include <fstream>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <zuffix/random/xoroshiro128plus_engine.hpp>
#include <zuffix/util/String.hpp>

constexpr char dnabase[] = {'A', 'C', 'G', 'T'};

zarr::String<char> random_dna(size_t m, bool dollar = false) {
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

template <typename T, sux::util::AllocType AT = sux::util::MALLOC>
std::tuple<std::string, size_t, size_t, std::vector<zarr::String<T, AT>>>
load_pizzachili_patterns(std::string filename) {
  std::vector<zarr::String<T, AT>> res;
  std::ifstream file(filename, std::ios::in);

  std::string header;
  std::getline(file, header);

  auto get = [&header](std::string item) {
    auto from = header.find(item) + item.length() + 1;
    auto len = header.substr(from).find(" ");
    return header.substr(from, len);
  };

  auto number = std::atoi(get("number").c_str());
  auto length = std::atoi(get("length").c_str());

  res.reserve(number);
  std::unique_ptr<char[]> buffer(new char[length]);

  for (int i = 0; i < number; i++) {
    file.read(buffer.get(), length);
    res.push_back(zarr::String<T, AT>(buffer.get(), length));
  }

  return std::make_tuple(get("file"), number, length, std::move(res));
}
