#include <benchmark/benchmark.h>

#include "common.hpp"

using namespace zarr;
using namespace sux::util;

template <typename RH> size_t simulation(RH &h, size_t n) {
	static std::random_device rd;
	static xoroshiro128plus_engine rng(rd());
	static std::uniform_int_distribution<int64_t> offset(0, 10);

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
	b->Args({1 << 5})->Args({1 << 10})->Args({1 << 15})->Args({1 << 20})->Args({1 << 30});
}

#define BM_SIMULATION(NAME, DS)                                                                                                                                                                        \
	static void BM_##NAME(benchmark::State &state) {                                                                                                                                                   \
		auto n = state.range(0);                                                                                                                                                                       \
		constexpr uint8_t charset[] = "abcdefghijklmnopqrstuvwxyz";                                                                                                                                    \
		auto string = random(n, charset, 26);                                                                                                                                                          \
		for (auto _ : state) {                                                                                                                                                                         \
			DS h(&string);                                                                                                                                                                             \
			benchmark::DoNotOptimize(simulation(h, n));                                                                                                                                           \
		}                                                                                                                                                                                              \
	}                                                                                                                                                                                                  \
	BENCHMARK(BM_##NAME)->Apply(args);

#define COMMA ,

// WARNING: It's very likely that the block size should actually be evaluated in
// Bytes, and not in symbols; thus, the best value sould be something like:
// (1 << 12) / sizeof(T)
// TODO: test it!

BM_SIMULATION(CRC32Folly_16B,   CRC32FollyHash<uint8_t COMMA 1 << 4>)
BM_SIMULATION(CRC32Folly_32B,   CRC32FollyHash<uint8_t COMMA 1 << 5>)
BM_SIMULATION(CRC32Folly_64B,   CRC32FollyHash<uint8_t COMMA 1 << 6>)
BM_SIMULATION(CRC32Folly_128B,  CRC32FollyHash<uint8_t COMMA 1 << 7>)
BM_SIMULATION(CRC32Folly_256B,  CRC32FollyHash<uint8_t COMMA 1 << 8>)
BM_SIMULATION(CRC32Folly_512B,  CRC32FollyHash<uint8_t COMMA 1 << 9>) // default (testare su blew)
BM_SIMULATION(CRC32Folly_1kB,   CRC32FollyHash<uint8_t COMMA 1 << 10>)
BM_SIMULATION(CRC32Folly_2kB,   CRC32FollyHash<uint8_t COMMA 1 << 11>)
BM_SIMULATION(CRC32Folly_4kB,   CRC32FollyHash<uint8_t COMMA 1 << 12>)
BM_SIMULATION(CRC32Folly_8kB,   CRC32FollyHash<uint8_t COMMA 1 << 13>)
BM_SIMULATION(CRC32Folly_16kB,  CRC32FollyHash<uint8_t COMMA 1 << 14>)
BM_SIMULATION(CRC32Folly_32kB,  CRC32FollyHash<uint8_t COMMA 1 << 15>)
BM_SIMULATION(CRC32Folly_64kB,  CRC32FollyHash<uint8_t COMMA 1 << 16>)
BM_SIMULATION(CRC32Folly_128KB, CRC32FollyHash<uint8_t COMMA 1 << 17>)
BM_SIMULATION(CRC32Folly_256KB, CRC32FollyHash<uint8_t COMMA 1 << 18>)
BM_SIMULATION(CRC32Folly_512KB, CRC32FollyHash<uint8_t COMMA 1 << 19>)
BM_SIMULATION(CRC32Folly_1MB,   CRC32FollyHash<uint8_t COMMA 1 << 20>)

BM_SIMULATION(CRC32Zlib_16B, CRC32ZlibHash<uint8_t COMMA 1 << 4>)
BM_SIMULATION(CRC32Zlib_32B, CRC32ZlibHash<uint8_t COMMA 1 << 5>)
BM_SIMULATION(CRC32Zlib_64B, CRC32ZlibHash<uint8_t COMMA 1 << 6>)
BM_SIMULATION(CRC32Zlib_128B, CRC32ZlibHash<uint8_t COMMA 1 << 7>)
BM_SIMULATION(CRC32Zlib_256B, CRC32ZlibHash<uint8_t COMMA 1 << 8>)
BM_SIMULATION(CRC32Zlib_512B, CRC32ZlibHash<uint8_t COMMA 1 << 9>)
BM_SIMULATION(CRC32Zlib_1kB, CRC32ZlibHash<uint8_t COMMA 1 << 10>)
BM_SIMULATION(CRC32Zlib_2kB, CRC32ZlibHash<uint8_t COMMA 1 << 11>)
BM_SIMULATION(CRC32Zlib_4kB, CRC32ZlibHash<uint8_t COMMA 1 << 12>) // default
BM_SIMULATION(CRC32Zlib_8kB, CRC32ZlibHash<uint8_t COMMA 1 << 13>)
BM_SIMULATION(CRC32Zlib_16kB, CRC32ZlibHash<uint8_t COMMA 1 << 14>)
BM_SIMULATION(CRC32Zlib_32kB, CRC32ZlibHash<uint8_t COMMA 1 << 15>)
BM_SIMULATION(CRC32Zlib_64kB, CRC32ZlibHash<uint8_t COMMA 1 << 16>)
BM_SIMULATION(CRC32Zlib_128KB, CRC32ZlibHash<uint8_t COMMA 1 << 17>)
BM_SIMULATION(CRC32Zlib_256KB, CRC32ZlibHash<uint8_t COMMA 1 << 18>)
BM_SIMULATION(CRC32Zlib_512KB, CRC32ZlibHash<uint8_t COMMA 1 << 19>)
BM_SIMULATION(CRC32Zlib_1MB, CRC32ZlibHash<uint8_t COMMA 1 << 20>)

BM_SIMULATION(XXH3_16B, XXH3Hash<uint8_t COMMA 1 << 4>)
BM_SIMULATION(XXH3_32B, XXH3Hash<uint8_t COMMA 1 << 5>)
BM_SIMULATION(XXH3_64B, XXH3Hash<uint8_t COMMA 1 << 6>)
BM_SIMULATION(XXH3_128B, XXH3Hash<uint8_t COMMA 1 << 8>)
BM_SIMULATION(XXH3_256B, XXH3Hash<uint8_t COMMA 1 << 9>)
BM_SIMULATION(XXH3_512B, XXH3Hash<uint8_t COMMA 1 << 9>)
BM_SIMULATION(XXH3_1kB, XXH3Hash<uint8_t COMMA 1 << 10>)
BM_SIMULATION(XXH3_2kB, XXH3Hash<uint8_t COMMA 1 << 11>)
BM_SIMULATION(XXH3_4kB, XXH3Hash<uint8_t COMMA 1 << 12>)
BM_SIMULATION(XXH3_8kB, XXH3Hash<uint8_t COMMA 1 << 13>)
BM_SIMULATION(XXH3_16kB, XXH3Hash<uint8_t COMMA 1 << 14>)
BM_SIMULATION(XXH3_32kB, XXH3Hash<uint8_t COMMA 1 << 15>) // default
BM_SIMULATION(XXH3_64kB, XXH3Hash<uint8_t COMMA 1 << 16>)
BM_SIMULATION(XXH3_128kB, XXH3Hash<uint8_t COMMA 1 << 17>)
BM_SIMULATION(XXH3_256kB, XXH3Hash<uint8_t COMMA 1 << 18>)
BM_SIMULATION(XXH3_512kB, XXH3Hash<uint8_t COMMA 1 << 19>)
BM_SIMULATION(XXH3_1MB, XXH3Hash<uint8_t COMMA 1 << 20>)

BM_SIMULATION(RabinKarp_16B, RabinKarpHash<uint8_t COMMA 1 << 4>)
BM_SIMULATION(RabinKarp_32B, RabinKarpHash<uint8_t COMMA 1 << 5>)
BM_SIMULATION(RabinKarp_64B, RabinKarpHash<uint8_t COMMA 1 << 6>)
BM_SIMULATION(RabinKarp_128B, RabinKarpHash<uint8_t COMMA 1 << 7>)
BM_SIMULATION(RabinKarp_256B, RabinKarpHash<uint8_t COMMA 1 << 8>) // default
BM_SIMULATION(RabinKarp_512B, RabinKarpHash<uint8_t COMMA 1 << 9>)
BM_SIMULATION(RabinKarp_1kB, RabinKarpHash<uint8_t COMMA 1 << 10>)
BM_SIMULATION(RabinKarp_2kB, RabinKarpHash<uint8_t COMMA 1 << 11>)
BM_SIMULATION(RabinKarp_4kB, RabinKarpHash<uint8_t COMMA 1 << 12>)
BM_SIMULATION(RabinKarp_8kB, RabinKarpHash<uint8_t COMMA 1 << 13>)
BM_SIMULATION(RabinKarp_16kB, RabinKarpHash<uint8_t COMMA 1 << 14>)
BM_SIMULATION(RabinKarp_32kB, RabinKarpHash<uint8_t COMMA 1 << 15>)
BM_SIMULATION(RabinKarp_64kB, RabinKarpHash<uint8_t COMMA 1 << 16>)
BM_SIMULATION(RabinKarp_128kB, RabinKarpHash<uint8_t COMMA 1 << 17>)
BM_SIMULATION(RabinKarp_256kB, RabinKarpHash<uint8_t COMMA 1 << 18>)
BM_SIMULATION(RabinKarp_512kB, RabinKarpHash<uint8_t COMMA 1 << 19>)
BM_SIMULATION(RabinKarp_1MB, RabinKarpHash<uint8_t COMMA 1 << 20>)

BM_SIMULATION(CyclicPoly128_16B, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 4>)
BM_SIMULATION(CyclicPoly128_32B, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 5>)
BM_SIMULATION(CyclicPoly128_64B, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 6>)
BM_SIMULATION(CyclicPoly128_128B, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 7>)
BM_SIMULATION(CyclicPoly128_256B, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 8>)
BM_SIMULATION(CyclicPoly128_512B, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 9>)
BM_SIMULATION(CyclicPoly128_1kB, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 10>)
BM_SIMULATION(CyclicPoly128_2kB, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 11>)
BM_SIMULATION(CyclicPoly128_4kB, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 12>) // default (?)
BM_SIMULATION(CyclicPoly128_8kB, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 13>)
BM_SIMULATION(CyclicPoly128_16kB, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 14>)
BM_SIMULATION(CyclicPoly128_32kB, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 15>)
BM_SIMULATION(CyclicPoly128_64kB, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 16>)
BM_SIMULATION(CyclicPoly128_128kB, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 17>)
BM_SIMULATION(CyclicPoly128_256kB, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 18>)
BM_SIMULATION(CyclicPoly128_512kB, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 19>)
BM_SIMULATION(CyclicPoly128_1MB, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 20>)

BENCHMARK_MAIN();
