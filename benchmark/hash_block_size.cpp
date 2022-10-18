#include <benchmark/benchmark.h>

#include "common.hpp"

using namespace zarr;
using namespace sux::util;

static void args(benchmark::internal::Benchmark *b) {
	// { length }
	b->Args({1 << 5})->Args({1 << 10})->Args({1 << 15})->Args({1 << 20})->Args({1 << 30});
}

#define BM_PREFIX(NAME, DS)                                                                                                                                                                            \
	static void BM_##NAME##_prefix(benchmark::State &state) {                                                                                                                                          \
		auto n = state.range(0);                                                                                                                                                                       \
		static std::random_device rd;                                                                                                                                                                  \
		static xoroshiro128plus_engine rng(rd());                                                                                                                                                      \
		std::uniform_int_distribution<size_t> dist(0, n - 1);                                                                                                                                          \
		constexpr uint8_t charset[] = "abcdefghijklmnopqrstuvwxyz";                                                                                                                                    \
		auto string = random(n, charset, 26);                                                                                                                                                          \
		DS h(&string);                                                                                                                                                                                 \
		h(n - 1); /* preloading */                                                                                                                                                                     \
		for (auto _ : state) {                                                                                                                                                                         \
			size_t a = dist(rng);                                                                                                                                                                      \
			benchmark::DoNotOptimize(h(a));                                                                                                                                                            \
		}                                                                                                                                                                                              \
	}                                                                                                                                                                                                  \
	BENCHMARK(BM_##NAME##_prefix)->Apply(args);

#define COMMA ,

// WARNING: It's very likely that the block size should actually be evaluated in
// Bytes, and not in symbols; thus, the best value sould be something like:
// (1 << 12) / sizeof(T)
// TODO: test it!

BM_PREFIX(XXH3_1kB, XXH3Hash<uint8_t COMMA 1 << 10>)
BM_PREFIX(XXH3_2kB, XXH3Hash<uint8_t COMMA 1 << 11>)
BM_PREFIX(XXH3_4kB, XXH3Hash<uint8_t COMMA 1 << 12>) // default
BM_PREFIX(XXH3_8kB, XXH3Hash<uint8_t COMMA 1 << 13>)
BM_PREFIX(XXH3_16kB, XXH3Hash<uint8_t COMMA 1 << 14>)

BM_PREFIX(RabinKarp_16B, RabinKarpHash<uint8_t COMMA 1 << 4>)
BM_PREFIX(RabinKarp_32B, RabinKarpHash<uint8_t COMMA 1 << 5>)
BM_PREFIX(RabinKarp_64B, RabinKarpHash<uint8_t COMMA 1 << 6>) // default
BM_PREFIX(RabinKarp_128B, RabinKarpHash<uint8_t COMMA 1 << 7>)
BM_PREFIX(RabinKarp_256B, RabinKarpHash<uint8_t COMMA 1 << 8>)

BM_PREFIX(CyclicPoly128_16B, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 4>)
BM_PREFIX(CyclicPoly128_32B, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 5>)
BM_PREFIX(CyclicPoly128_64B, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 6>) // default
BM_PREFIX(CyclicPoly128_128B, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 7>)
BM_PREFIX(CyclicPoly128_256B, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 8>)

BENCHMARK_MAIN();
