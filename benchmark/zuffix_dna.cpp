#include <benchmark/benchmark.h>
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

using namespace zarr;
using namespace sux::util;

static constexpr char base[] = {'A', 'C', 'G', 'T'};

uint64_t spooky_hash(const void *message, size_t length) {
  return SpookyHash::Hash64(message, length, 0);
}

size_t filesize(std::string filename) {
  std::ifstream file(filename, std::ios::in);
  file.ignore(std::numeric_limits<std::streamsize>::max());
  return file.gcount();
}

String<char> fileToString(std::string filename) {
  std::ifstream file(filename, std::ios::in);
  std::stringstream strstream;
  strstream << file.rdbuf();
  return String<char>(strstream.str());
}

constexpr size_t TEXTS = 4;
String<char> text(size_t i) {
  std::string path = "../../dataset/";
  const char *file[TEXTS] = {"dna.gz", "dna.50MB.gz", "dna.100MB.gz", "dna.200MB.gz"};
  return fileToString(path + file[i]);
}

static String<char> pattern(size_t m) {
  String<char> result(m);

  static std::random_device rd;
  static xoroshiro128plus_engine rng(rd());
  std::uniform_int_distribution<char> dist(0, 4);

  for (size_t i = 0; i < m; i++)
    result[i] = dist(rng);

  return result;
}

static void CustomArguments(benchmark::internal::Benchmark *b) {
  for (int i = 0; i < TEXTS; i++)
    for (int j = 0; j < 20; j++)
      b->Args({i, 1ULL << j});
}

// BENCHMARKS

static void BM_Find(benchmark::State &state) {
  ZuffixArray<char, spooky_hash> zuffix(text(state.range(0)));
  auto p = pattern(state.range(1));

  for (auto _ : state) {
    benchmark::DoNotOptimize(zuffix.find(p));
  }
}
BENCHMARK(BM_Find)->Apply(CustomArguments);

static void BM_FindExact(benchmark::State &state) {
  ZuffixArray<char, spooky_hash> zuffix(text(state.range(0)));
  auto p = pattern(state.range(1));

  for (auto _ : state) {
    benchmark::DoNotOptimize(zuffix.findExact(p));
  }
}
BENCHMARK(BM_FindExact)->Apply(CustomArguments);

BENCHMARK_MAIN();
