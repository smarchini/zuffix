#include <benchmark/benchmark.h>

#include "common.hpp"

using namespace zarr;
using namespace sux::util;

static void args(benchmark::internal::Benchmark *b) {
	// { text_length, pattern_length }
	for (long i = 17; i < 20; i++)
		b->Args({20, i}); //   1 MiB text - i KiB pattern
						  // or (long i = 15; i < 23; i ++) b->Args({23, i});    //   1 MiB text - i KiB pattern
						  // for (long i = 5; i <= 20; i += 5) b->Args({1L << 20, i << 10});    //   1 MiB text - i KiB pattern
						  // for (long i = 5; i <= 20; i += 5) b->Args({50L << 20, i << 20});   //  50 MiB text - i MiB pattern
						  // for (long i = 50; i <= 70; i += 5) b->Args({100L << 20, i << 20}); // 100 MiB text - i MiB pattern
}

static void BM_Simple(benchmark::State &state) {
	size_t n = state.range(0), m = state.range(1);
	SimpleSuffixArray<char> ds(fibonacci(n, true));
	uint64_t cnt = 0;
	String<char> p = fibonacci(m);
	for (auto _ : state) benchmark::DoNotOptimize(cnt = ds.find(p).length());
	state.counters["cnt"] = cnt;
}
BENCHMARK(BM_Simple)->Apply(args);

static void BM_Enhanced(benchmark::State &state) {
	size_t n = state.range(0), m = state.range(1);
	EnhancedSuffixArray<char> ds(fibonacci(n, true));
	uint64_t cnt = 0;
	String<char> p = fibonacci(m);
	for (auto _ : state) benchmark::DoNotOptimize(cnt = ds.find(p).length());
	state.counters["cnt"] = cnt;
}
BENCHMARK(BM_Enhanced)->Apply(args);

static void BM_ZuffixRabinKarp(benchmark::State &state) {
	size_t n = state.range(0), m = state.range(1);
	EnhancedZuffixArray<char, RabinKarpHash> ds(fibonacci(n, true));
	uint64_t cnt = 0;
	String<char> p = fibonacci(m);
	for (auto _ : state) benchmark::DoNotOptimize(cnt = ds.find(p).length());
	state.counters["cnt"] = cnt;
}
BENCHMARK(BM_ZuffixRabinKarp)->Apply(args);

static void BM_ZuffixCyclicPoly128(benchmark::State &state) {
	size_t n = state.range(0), m = state.range(1);
	EnhancedZuffixArray<char, CyclicPoly128Hash> ds(fibonacci(n, true));
	uint64_t cnt = 0;
	String<char> p = fibonacci(m);
	for (auto _ : state) benchmark::DoNotOptimize(cnt = ds.find(p).length());
	state.counters["cnt"] = cnt;
}
BENCHMARK(BM_ZuffixCyclicPoly128)->Apply(args);

static void BM_ZuffixO1(benchmark::State &state) {
	size_t n = state.range(0), m = state.range(1);
	EnhancedZuffixArray<char, O1Hash> ds(fibonacci(n, true));
	uint64_t cnt = 0;
	String<char> p = fibonacci(m);
	for (auto _ : state) benchmark::DoNotOptimize(cnt = ds.find(p).length());
	state.counters["cnt"] = cnt;
}
BENCHMARK(BM_ZuffixO1)->Apply(args);

static void BM_ZuffixXXH3(benchmark::State &state) {
	size_t n = state.range(0), m = state.range(1);
	EnhancedZuffixArray<char, XXH3Hash> ds(fibonacci(n, true));
	uint64_t cnt = 0;
	String<char> p = fibonacci(m);
	for (auto _ : state) benchmark::DoNotOptimize(cnt = ds.find(p).length());
	state.counters["cnt"] = cnt;
}
BENCHMARK(BM_ZuffixXXH3)->Apply(args);

BENCHMARK_MAIN();
