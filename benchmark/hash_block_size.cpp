#include <benchmark/benchmark.h>

#include "common.hpp"

using namespace zarr;
using namespace sux::util;

template <typename RH> size_t simulation(RH &h, size_t n) {
	static std::uniform_int_distribution<int64_t> offset(0, 10);

	benchmark::DoNotOptimize(h(n));

	int64_t cnt = 0;
	size_t l = 0, r = n;
	while (l < r) {
		cnt++;
		size_t f = twoFattestR(l, r);
		benchmark::DoNotOptimize(h(f));
		int64_t x = 1 + offset(rng);
		if (rng() & 1) {
			r = f - 1;
		} else {
			l = f + x;
		}
	}
	return cnt;
}

static void args(benchmark::internal::Benchmark *b) {
	// { length }
	b->Args({1 << 5})->Args({1 << 10})->Args({1 << 15})->Args({1 << 20})->Args({1 << 25})->Args({1 << 30});
}

#define BM_SIMULATION(NAME, DS)                                                                                                                                                                        \
	static void BM_##NAME(benchmark::State &state) {                                                                                                                                                   \
		auto n = state.range(0);                                                                                                                                                                       \
		auto string = randword(n);                                                                                                                                                          \
		for (auto _ : state) {                                                                                                                                                                         \
			DS h(&string, string.size());                                                                                                                                                            \
			benchmark::DoNotOptimize(simulation(h, n));                                                                                                                                                \
		}                                                                                                                                                                                              \
	}                                                                                                                                                                                                  \
	BENCHMARK(BM_##NAME)->Apply(args);

#define COMMA ,

// WARNING: It's very likely that the block size should actually be evaluated in
// Bytes, and not in symbols; thus, the best value sould be something like:
// (1 << 12) / sizeof(T)

BM_SIMULATION(CRC32CFolly_16B,  CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 4>)
BM_SIMULATION(CRC32CFolly_32B,  CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 5>)
BM_SIMULATION(CRC32CFolly_64B,  CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 6>)
BM_SIMULATION(CRC32CFolly_128B, CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 7>)
BM_SIMULATION(CRC32CFolly_256B, CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 8>)
BM_SIMULATION(CRC32CFolly_512B, CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 9>) // default (testare su blew)
BM_SIMULATION(CRC32CFolly_1kB,  CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 10>)
BM_SIMULATION(CRC32CFolly_2kB,  CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 11>)
BM_SIMULATION(CRC32CFolly_4kB,  CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 12>)
BM_SIMULATION(CRC32CFolly_8kB,  CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 13>)
BM_SIMULATION(CRC32CFolly_16kB,  CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 14>)
BM_SIMULATION(CRC32CFolly_32kB,  CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 15>)
BM_SIMULATION(CRC32CFolly_64kB,  CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 16>)
BM_SIMULATION(CRC32CFolly_128kB,  CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 17>)
BM_SIMULATION(CRC32CFolly_256kB,  CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 18>)
BM_SIMULATION(CRC32CFolly_512kB,  CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 19>)
BM_SIMULATION(CRC32CFolly_1MB,  CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 20>)
BM_SIMULATION(CRC32CFolly_2MB,  CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 21>)
BM_SIMULATION(CRC32CFolly_4MB,  CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 22>)
BM_SIMULATION(CRC32CFolly_8MB,  CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 23>)
BM_SIMULATION(CRC32CFolly_16MB,  CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 24>)
BM_SIMULATION(CRC32CFolly_32MB,  CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 25>)
BM_SIMULATION(CRC32CFolly_64MB,  CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 26>)
BM_SIMULATION(CRC32CFolly_128MB,  CRC32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 27>)

BM_SIMULATION(CRC32Zlib_16B, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 4>)
BM_SIMULATION(CRC32Zlib_32B, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 5>)
BM_SIMULATION(CRC32Zlib_64B, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 6>)
BM_SIMULATION(CRC32Zlib_128B, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 7>)
BM_SIMULATION(CRC32Zlib_256B, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 8>)
BM_SIMULATION(CRC32Zlib_512B, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 9>)
BM_SIMULATION(CRC32Zlib_1kB, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 10>)
BM_SIMULATION(CRC32Zlib_2kB, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 11>)
BM_SIMULATION(CRC32Zlib_4kB, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 12>) // default
BM_SIMULATION(CRC32Zlib_8kB, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 13>)
BM_SIMULATION(CRC32Zlib_16kB, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 14>)
BM_SIMULATION(CRC32Zlib_32kB, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 15>)
BM_SIMULATION(CRC32Zlib_64kB, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 16>)
BM_SIMULATION(CRC32Zlib_128kB, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 17>)
BM_SIMULATION(CRC32Zlib_256kB, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 18>)
BM_SIMULATION(CRC32Zlib_512kB, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 19>)
BM_SIMULATION(CRC32Zlib_1MB, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 20>)
BM_SIMULATION(CRC32Zlib_2MB, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 21>)
BM_SIMULATION(CRC32Zlib_4MB, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 22>)
BM_SIMULATION(CRC32Zlib_8MB, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 23>)
BM_SIMULATION(CRC32Zlib_16MB, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 24>)
BM_SIMULATION(CRC32Zlib_32MB, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 25>)
BM_SIMULATION(CRC32Zlib_64MB, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 26>)
BM_SIMULATION(CRC32Zlib_128MB, CRC32ZlibHash<char COMMA ALLOC_TYPE COMMA 1 << 27>)

BM_SIMULATION(CRC32CRC32C_16B,   CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 4>)
BM_SIMULATION(CRC32CRC32C_32B,   CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 5>)
BM_SIMULATION(CRC32CRC32C_64B,   CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 6>)
BM_SIMULATION(CRC32CRC32C_128B,  CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 7>)
BM_SIMULATION(CRC32CRC32C_256B,  CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 8>)
BM_SIMULATION(CRC32CRC32C_512B,  CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 9>)
BM_SIMULATION(CRC32CRC32C_1kB,   CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 10>)
BM_SIMULATION(CRC32CRC32C_2kB,   CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 11>)
BM_SIMULATION(CRC32CRC32C_4kB,   CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 12>) // default
BM_SIMULATION(CRC32CRC32C_8kB,   CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 13>)
BM_SIMULATION(CRC32CRC32C_16kB,   CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 14>)
BM_SIMULATION(CRC32CRC32C_32kB,   CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 15>)
BM_SIMULATION(CRC32CRC32C_64kB,   CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 16>)
BM_SIMULATION(CRC32CRC32C_128kB,   CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 17>)
BM_SIMULATION(CRC32CRC32C_256kB,   CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 18>)
BM_SIMULATION(CRC32CRC32C_512kB,   CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 19>)
BM_SIMULATION(CRC32CRC32C_1MB,   CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 20>)
BM_SIMULATION(CRC32CRC32C_2MB,   CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 21>)
BM_SIMULATION(CRC32CRC32C_4MB,   CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 22>)
BM_SIMULATION(CRC32CRC32C_8MB,   CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 23>)
BM_SIMULATION(CRC32CRC32C_16MB,   CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 24>)
BM_SIMULATION(CRC32CRC32C_32MB,   CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 25>)
BM_SIMULATION(CRC32CRC32C_64MB,   CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 26>)
BM_SIMULATION(CRC32CRC32C_128MB,   CRC32Plus32CFollyHash<char COMMA ALLOC_TYPE COMMA 1 << 27>)

BM_SIMULATION(XXH3_16B, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 4>)
BM_SIMULATION(XXH3_32B, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 5>)
BM_SIMULATION(XXH3_64B, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 6>)
BM_SIMULATION(XXH3_128B, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 8>)
BM_SIMULATION(XXH3_256B, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 9>)
BM_SIMULATION(XXH3_512B, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 9>)
BM_SIMULATION(XXH3_1kB, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 10>)
BM_SIMULATION(XXH3_2kB, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 11>)
BM_SIMULATION(XXH3_4kB, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 12>)
BM_SIMULATION(XXH3_8kB, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 13>)
BM_SIMULATION(XXH3_16kB, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 14>)
BM_SIMULATION(XXH3_32kB, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 15>) // default
BM_SIMULATION(XXH3_64kB, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 16>)
BM_SIMULATION(XXH3_128kB, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 17>)
BM_SIMULATION(XXH3_256kB, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 18>)
BM_SIMULATION(XXH3_512kB, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 19>)
BM_SIMULATION(XXH3_1MB, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 20>)
BM_SIMULATION(XXH3_2MB, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 21>)
BM_SIMULATION(XXH3_4MB, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 22>)
BM_SIMULATION(XXH3_8MB, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 23>)
BM_SIMULATION(XXH3_16MB, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 24>)
BM_SIMULATION(XXH3_32MB, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 25>)
BM_SIMULATION(XXH3_64MB, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 26>)
BM_SIMULATION(XXH3_128MB, XXH3Hash<char COMMA ALLOC_TYPE COMMA 1 << 27>)

BM_SIMULATION(WyHash_1, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 1>)
BM_SIMULATION(WyHash_2, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 2>)
BM_SIMULATION(WyHash_3, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 3>)
BM_SIMULATION(WyHash_4, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 4>)
BM_SIMULATION(WyHash_5, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 5>) // default (testare su blew)
BM_SIMULATION(WyHash_6, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 6>)
BM_SIMULATION(WyHash_7, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 7>)
BM_SIMULATION(WyHash_8, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 8>)
BM_SIMULATION(WyHash_9, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 9>)
BM_SIMULATION(WyHash_10, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 10>)
BM_SIMULATION(WyHash_20, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 20>)
BM_SIMULATION(WyHash_30, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 30>)
BM_SIMULATION(WyHash_40, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 40>)
BM_SIMULATION(WyHash_50, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 50>)
BM_SIMULATION(WyHash_60, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 60>)
BM_SIMULATION(WyHash_70, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 70>)
BM_SIMULATION(WyHash_80, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 80>)
BM_SIMULATION(WyHash_90, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 90>)
BM_SIMULATION(WyHash_100, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 100>)
BM_SIMULATION(WyHash_200, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 200>)
BM_SIMULATION(WyHash_300, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 300>)
BM_SIMULATION(WyHash_400, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 400>)
BM_SIMULATION(WyHash_500, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 500>)
BM_SIMULATION(WyHash_1000, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 1000>)
BM_SIMULATION(WyHash_10000, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 10000>)
BM_SIMULATION(WyHash_100000, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 100000>)
BM_SIMULATION(WyHash_1000000, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 1000000>)
BM_SIMULATION(WyHash_10000000, WyHash<char COMMA ALLOC_TYPE COMMA 48 * 10000000>)

// BM_SIMULATION(RabinKarp_16B, RabinKarpHash<char COMMA AllocType::MALLOC COMMA 1 << 4>)
// BM_SIMULATION(RabinKarp_32B, RabinKarpHash<char COMMA AllocType::MALLOC COMMA 1 << 5>)
// BM_SIMULATION(RabinKarp_64B, RabinKarpHash<char COMMA AllocType::MALLOC COMMA 1 << 6>)
// BM_SIMULATION(RabinKarp_128B, RabinKarpHash<char COMMA AllocType::MALLOC COMMA 1 << 7>)
// BM_SIMULATION(RabinKarp_256B, RabinKarpHash<char COMMA AllocType::MALLOC COMMA 1 << 8>) // default
// BM_SIMULATION(RabinKarp_512B, RabinKarpHash<char COMMA AllocType::MALLOC COMMA 1 << 9>)
// BM_SIMULATION(RabinKarp_1kB, RabinKarpHash<char COMMA AllocType::MALLOC COMMA 1 << 10>)
// BM_SIMULATION(RabinKarp_2kB, RabinKarpHash<char COMMA AllocType::MALLOC COMMA 1 << 11>)
// BM_SIMULATION(RabinKarp_4kB, RabinKarpHash<char COMMA AllocType::MALLOC COMMA 1 << 12>)
// BM_SIMULATION(RabinKarp_8kB, RabinKarpHash<char COMMA AllocType::MALLOC COMMA 1 << 13>)
// BM_SIMULATION(RabinKarp_16kB, RabinKarpHash<char COMMA AllocType::MALLOC COMMA 1 << 14>)
// BM_SIMULATION(RabinKarp_32kB, RabinKarpHash<char COMMA AllocType::MALLOC COMMA 1 << 15>)
// BM_SIMULATION(RabinKarp_64kB, RabinKarpHash<char COMMA AllocType::MALLOC COMMA 1 << 16>)
// BM_SIMULATION(RabinKarp_128kB, RabinKarpHash<char COMMA AllocType::MALLOC COMMA 1 << 17>)
// BM_SIMULATION(RabinKarp_256kB, RabinKarpHash<char COMMA AllocType::MALLOC COMMA 1 << 18>)
// BM_SIMULATION(RabinKarp_512kB, RabinKarpHash<char COMMA AllocType::MALLOC COMMA 1 << 19>)
// BM_SIMULATION(RabinKarp_1MB, RabinKarpHash<char COMMA AllocType::MALLOC COMMA 1 << 20>)
//
// BM_SIMULATION(CyclicPoly128_16B, CyclicPolyHash<char COMMA AllocType::MALLOC COMMA 128 COMMA AllocType::MALLOC COMMA 1 << 4>)
// BM_SIMULATION(CyclicPoly128_32B, CyclicPolyHash<char COMMA AllocType::MALLOC COMMA 128 COMMA AllocType::MALLOC COMMA 1 << 5>)
// BM_SIMULATION(CyclicPoly128_64B, CyclicPolyHash<char COMMA AllocType::MALLOC COMMA 128 COMMA AllocType::MALLOC COMMA 1 << 6>)
// BM_SIMULATION(CyclicPoly128_128B, CyclicPolyHash<char COMMA AllocType::MALLOC COMMA 128 COMMA AllocType::MALLOC COMMA 1 << 7>)
// BM_SIMULATION(CyclicPoly128_256B, CyclicPolyHash<char COMMA AllocType::MALLOC COMMA 128 COMMA AllocType::MALLOC COMMA 1 << 8>)
// BM_SIMULATION(CyclicPoly128_512B, CyclicPolyHash<char COMMA AllocType::MALLOC COMMA 128 COMMA AllocType::MALLOC COMMA 1 << 9>)
// BM_SIMULATION(CyclicPoly128_1kB, CyclicPolyHash<char COMMA AllocType::MALLOC COMMA 128 COMMA AllocType::MALLOC COMMA 1 << 10>)
// BM_SIMULATION(CyclicPoly128_2kB, CyclicPolyHash<char COMMA AllocType::MALLOC COMMA 128 COMMA AllocType::MALLOC COMMA 1 << 11>)
// BM_SIMULATION(CyclicPoly128_4kB, CyclicPolyHash<char COMMA AllocType::MALLOC COMMA 128 COMMA AllocType::MALLOC COMMA 1 << 12>) // default (?)
// BM_SIMULATION(CyclicPoly128_8kB, CyclicPolyHash<char COMMA AllocType::MALLOC COMMA 128 COMMA AllocType::MALLOC COMMA 1 << 13>)
// BM_SIMULATION(CyclicPoly128_16kB, CyclicPolyHash<char COMMA AllocType::MALLOC COMMA 128 COMMA AllocType::MALLOC COMMA 1 << 14>)
// BM_SIMULATION(CyclicPoly128_32kB, CyclicPolyHash<char COMMA AllocType::MALLOC COMMA 128 COMMA AllocType::MALLOC COMMA 1 << 15>)
// BM_SIMULATION(CyclicPoly128_64kB, CyclicPolyHash<char COMMA AllocType::MALLOC COMMA 128 COMMA AllocType::MALLOC COMMA 1 << 16>)
// BM_SIMULATION(CyclicPoly128_128kB, CyclicPolyHash<char COMMA AllocType::MALLOC COMMA 128 COMMA AllocType::MALLOC COMMA 1 << 17>)
// BM_SIMULATION(CyclicPoly128_256kB, CyclicPolyHash<char COMMA AllocType::MALLOC COMMA 128 COMMA AllocType::MALLOC COMMA 1 << 18>)
// BM_SIMULATION(CyclicPoly128_512kB, CyclicPolyHash<char COMMA AllocType::MALLOC COMMA 128 COMMA AllocType::MALLOC COMMA 1 << 19>)
// BM_SIMULATION(CyclicPoly128_1MB, CyclicPolyHash<char COMMA AllocType::MALLOC COMMA 128 COMMA AllocType::MALLOC COMMA 1 << 20>)

BENCHMARK_MAIN();
