// required by common.hpp
#define SYMBOLTYPE char

#include "common.hpp"
#include "hash.hpp"
#include <benchmark/benchmark.h>

#include <zuffix/ZuffixArray.hpp>
#include <zuffix/SimpleSuffixArray.hpp>
#include <zuffix/EnhancedSuffixArray.hpp>
#include <zuffix/EnhancedZuffixArray.hpp>

using namespace zarr;
using namespace sux::util;

static void args(benchmark::internal::Benchmark *b) {
  b->Args({1L << 5, 1L << 2});
  b->Args({1L << 10, 1L << 2});
  b->Args({1L << 15, 1L << 2});
  b->Args({1L << 20, 1L << 5});
  b->Args({1L << 20, 1L << 10});
  b->Args({1L << 20, 1L << 15});

  //b->Args({1L << 30, 1L << 5});
  //b->Args({1L << 30, 1L << 10});
  //b->Args({1L << 30, 1L << 15});

  //for (int i = 20; i < 25; i++)
  //  for (int j = 10; j < 25; j++)
  //    b->Args({1L << i, 1L << j}); // { text_length, pattern_length }
}

static void BM_Find(benchmark::State &state) {
  ZuffixArray<SYMBOLTYPE, spooky_hash> zuffix(random_dna(state.range(0), true));
  auto p = random_dna(state.range(1));

  for (auto _ : state) {
    benchmark::DoNotOptimize(zuffix.find(p));
  }
}
BENCHMARK(BM_Find)->Apply(args);

static void BM_FindExact(benchmark::State &state) {
  ZuffixArray<SYMBOLTYPE, spooky_hash> zuffix(random_dna(state.range(0), true));
  auto p = random_dna(state.range(1));

  for (auto _ : state) {
    benchmark::DoNotOptimize(zuffix.findExact(p));
  }
}
BENCHMARK(BM_FindExact)->Apply(args);

static void BM_Simple(benchmark::State &state) {
  SimpleSuffixArray<SYMBOLTYPE> simple(random_dna(state.range(0), true));
  auto p = random_dna(state.range(1));

  for (auto _ : state) {
    benchmark::DoNotOptimize(simple.find(p));
  }
}
BENCHMARK(BM_Simple)->Apply(args);

static void BM_Enhanced(benchmark::State &state) {
  EnhancedSuffixArray<SYMBOLTYPE> enhanced(random_dna(state.range(0), true));
  auto p = random_dna(state.range(1));

  for (auto _ : state) {
    benchmark::DoNotOptimize(enhanced.find(p));
  }
}
BENCHMARK(BM_Enhanced)->Apply(args);

static void BM_ZuffixRabinKarp(benchmark::State &state) {
  EnhancedZuffixArray<SYMBOLTYPE, RabinKarpHash> zuffix(random_dna(state.range(0), true));
  auto p = random_dna(state.range(1));

  for (auto _ : state) {
    benchmark::DoNotOptimize(zuffix.find(p));
  }
}
BENCHMARK(BM_ZuffixRabinKarp)->Apply(args);

static void BM_ZuffixCyclicPoly128(benchmark::State &state) {
  EnhancedZuffixArray<SYMBOLTYPE, CyclicPoly128Hash> zuffix(random_dna(state.range(0), true));
  auto p = random_dna(state.range(1));

  for (auto _ : state) {
    benchmark::DoNotOptimize(zuffix.find(p));
  }
}
BENCHMARK(BM_ZuffixCyclicPoly128)->Apply(args);

BENCHMARK_MAIN();
