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

BM_SIMULATION(CRC32_16B, CRC32ZlibHash<uint8_t COMMA 1 << 4>)
BM_SIMULATION(CRC32_32B, CRC32ZlibHash<uint8_t COMMA 1 << 5>)
BM_SIMULATION(CRC32_64B, CRC32ZlibHash<uint8_t COMMA 1 << 6>) // default
BM_SIMULATION(CRC32_128B, CRC32ZlibHash<uint8_t COMMA 1 << 7>)
BM_SIMULATION(CRC32_256B, CRC32ZlibHash<uint8_t COMMA 1 << 8>)
BM_SIMULATION(CRC32_512b, CRC32ZlibHash<uint8_t COMMA 1 << 9>)
BM_SIMULATION(CRC32_1kb, CRC32ZlibHash<uint8_t COMMA 1 << 10>)

BM_SIMULATION(XXH3_1kb, XXH3Hash<uint8_t COMMA 1 << 10>)
BM_SIMULATION(XXH3_2kb, XXH3Hash<uint8_t COMMA 1 << 11>)
BM_SIMULATION(XXH3_4kb, XXH3Hash<uint8_t COMMA 1 << 12>) // default
BM_SIMULATION(XXH3_8kb, XXH3Hash<uint8_t COMMA 1 << 13>)
BM_SIMULATION(XXH3_16kb, XXH3Hash<uint8_t COMMA 1 << 14>)

BM_SIMULATION(RabinKarp_16B, RabinKarpHash<uint8_t COMMA 1 << 4>)
BM_SIMULATION(RabinKarp_32B, RabinKarpHash<uint8_t COMMA 1 << 5>)
BM_SIMULATION(RabinKarp_64B, RabinKarpHash<uint8_t COMMA 1 << 6>) // default
BM_SIMULATION(RabinKarp_128B, RabinKarpHash<uint8_t COMMA 1 << 7>)
BM_SIMULATION(RabinKarp_256B, RabinKarpHash<uint8_t COMMA 1 << 8>)

BM_SIMULATION(CyclicPoly128_16B, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 4>)
BM_SIMULATION(CyclicPoly128_32B, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 5>)
BM_SIMULATION(CyclicPoly128_64B, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 6>) // default
BM_SIMULATION(CyclicPoly128_128B, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 7>)
BM_SIMULATION(CyclicPoly128_256B, CyclicPolyHash<uint8_t COMMA 128 COMMA 1 << 8>)

BENCHMARK_MAIN();
