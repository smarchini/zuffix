#include <benchmark/benchmark.h>

#include "common.hpp"

using namespace zarr;
using namespace sux::util;

static void args(benchmark::internal::Benchmark *b) {
	// { text_length, pattern_length }
	for (long i = 20; i <= 24; i++) b->Args({25, i});
}

#define BM(NAME, DS)                                                                                                                                                                                   \
	static void BM_##NAME(benchmark::State &state) {                                                                                                                                                   \
		size_t n = state.range(0), m = state.range(1);                                                                                                                                                 \
		DS ds(fibonacci(n, true));                                                                                                                                                                     \
		String<char> p = fibonacci(m);                                                                                                                                                                 \
		for (auto _ : state) {                                                                                                                                                                         \
			benchmark::DoNotOptimize(p);                                                                                                                                                               \
			auto f = ds.find(p);                                                                                                                                                                       \
			benchmark::DoNotOptimize(f);                                                                                                                                                               \
		}                                                                                                                                                                                              \
	}                                                                                                                                                                                                  \
	BENCHMARK(BM_##NAME)->Apply(args);

#define COMMA ,

BM(Simple, SimpleSuffixArray<char>)
BM(Enhanced, EnhancedSuffixArray<char>)

BM(ExactZuffixXXH3, ExactZuffixArray<char COMMA XXH3Hash>)
BM(ProbabilisticZuffixXXH3, ProbabilisticZuffixArray<char COMMA XXH3Hash>)

BM(ExactZuffixCRC32CFolly, ExactZuffixArray<char COMMA CRC32CFollyHash>)
BM(ProbabilisticZuffixCRC32CFolly, ProbabilisticZuffixArray<char COMMA CRC32CFollyHash>)

BM(ExactZuffixCRC32Zlib, ExactZuffixArray<char COMMA CRC32ZlibHash>)
BM(ProbabilisticZuffixCRC32Zlib, ProbabilisticZuffixArray<char COMMA CRC32ZlibHash>)

BM(ExactZuffixRabinKarp, ExactZuffixArray<char COMMA RabinKarpHash>)
BM(ProbabilisticZuffixRabinKarp, ProbabilisticZuffixArray<char COMMA RabinKarpHash>)

BM(ExactZuffixCyclicPoly128, ExactZuffixArray<char COMMA CyclicPoly128Hash>)
BM(ProbabilisticZuffixCyclicPoly128, ProbabilisticZuffixArray<char COMMA CyclicPoly128Hash>)

BENCHMARK_MAIN();
