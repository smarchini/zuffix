#include <benchmark/benchmark.h>

#include "common.hpp"

using namespace zarr;
using namespace sux::util;

static void args(benchmark::internal::Benchmark *b) {
	// { text_length, pattern_length }
	for (long i = 20; i <= 23; i++) b->Args({23, i});
}

#define BM(NAME, DS)                                                                                                                                                                                   \
	static void BM_##NAME(benchmark::State &state) {                                                                                                                                                   \
		size_t n = state.range(0), m = state.range(1);                                                                                                                                                 \
		DS ds(fibonacci(n, true));                                                                                                                                                                     \
		uint64_t cnt = 0;                                                                                                                                                                              \
		String<char> p = fibonacci(m);                                                                                                                                                                 \
		for (auto _ : state) benchmark::DoNotOptimize(cnt = ds.find(p).length());                                                                                                                      \
		state.counters["cnt"] = cnt;                                                                                                                                                                   \
	}                                                                                                                                                                                                  \
	BENCHMARK(BM_##NAME)->Apply(args);

#define COMMA ,

BM(Simple, SimpleSuffixArray<char>)
BM(Enhanced, EnhancedSuffixArray<char>)
BM(ZuffixRabinKarp, ExactZuffixArray<char COMMA RabinKarpHash>)
BM(ZuffixCyclicPoly128, ExactZuffixArray<char COMMA CyclicPoly128Hash>)
BM(ZuffixO1, ExactZuffixArray<char COMMA O1Hash>)
BM(ZuffixXXH3, ExactZuffixArray<char COMMA XXH3Hash>)
BM(ZuffixCRC32, ExactZuffixArray<char COMMA CRC32Hash>)

BENCHMARK_MAIN();
