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

BM(MemcmpZuffixXXH3, MemcmpZuffixArray<char COMMA XXH3Hash>)
BM(SignatureZuffixXXH3, SignatureZuffixArray<char COMMA XXH3Hash>)

BM(MemcmpZuffixCRC32CFolly, MemcmpZuffixArray<char COMMA CRC32CFollyHash>)
BM(SignatureZuffixCRC32CFolly, SignatureZuffixArray<char COMMA CRC32CFollyHash>)

BM(MemcmpZuffixCRC32Zlib, MemcmpZuffixArray<char COMMA CRC32ZlibHash>)
BM(SignatureZuffixCRC32Zlib, SignatureZuffixArray<char COMMA CRC32ZlibHash>)

BM(MemcmpZuffixRabinKarp, MemcmpZuffixArray<char COMMA RabinKarpHash>)
BM(SignatureZuffixRabinKarp, SignatureZuffixArray<char COMMA RabinKarpHash>)

BM(MemcmpZuffixCyclicPoly128, MemcmpZuffixArray<char COMMA CyclicPoly128Hash>)
BM(SignatureZuffixCyclicPoly128, SignatureZuffixArray<char COMMA CyclicPoly128Hash>)

BENCHMARK_MAIN();
