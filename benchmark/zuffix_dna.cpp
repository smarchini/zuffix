#include <benchmark/benchmark.h>
#include "common.hpp"

using namespace zarr;
using namespace sux::util;

static void args(benchmark::internal::Benchmark *b) {
  for (int i = 20; i < 30; i++)
    for (int j = 10; j < 20; j++)
      b->Args({1ULL << i, 1ULL << j}); // { text_length, pattern_length }
}

static void BM_Find(benchmark::State &state) {
  ZuffixArray<char, spooky_hash> zuffix(random_dna(state.range(0)));
  auto p = random_dna(state.range(1));

  for (auto _ : state) {
    benchmark::DoNotOptimize(zuffix.find(p));
  }
}
BENCHMARK(BM_Find)->Apply(args);

static void BM_FindExact(benchmark::State &state) {
  ZuffixArray<char, spooky_hash> zuffix(random_dna(state.range(0)));
  auto p = random_dna(state.range(1));

  for (auto _ : state) {
    benchmark::DoNotOptimize(zuffix.findExact(p));
  }
}
BENCHMARK(BM_FindExact)->Apply(args);

BENCHMARK_MAIN();
