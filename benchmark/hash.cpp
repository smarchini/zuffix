#include <benchmark/benchmark.h>

#include "common.hpp"

using namespace zarr;
using namespace sux::util;

static void args(benchmark::internal::Benchmark *b) {
	// { length }
	b->Args({1 << 5})->Args({1 << 10})->Args({1 << 15})->Args({1 << 20})->Args({1 << 25})->Args({1 << 30});
}

#define BM_PREFIX(NAME, DS)                                                                                                                                                                            \
	static void BM_##NAME##_prefix(benchmark::State &state) {                                                                                                                                          \
		auto n = state.range(0);                                                                                                                                                                       \
		std::uniform_int_distribution<size_t> dist(0, n - 1);                                                                                                                                          \
		auto string = randword(n);                                                                                                                                                          \
		DS h(&string, string.size());                                                                                                                                                                \
		h(n - 1); /* preloading */                                                                                                                                                                     \
		for (auto _ : state) {                                                                                                                                                                         \
			size_t a = dist(rng);                                                                                                                                                                      \
			benchmark::DoNotOptimize(h(a));                                                                                                                                                            \
		}                                                                                                                                                                                              \
	}                                                                                                                                                                                                  \
	BENCHMARK(BM_##NAME##_prefix)->Apply(args);

#define BM_MIDDLE(NAME, DS)                                                                                                                                                                            \
	static void BM_##NAME##_middle(benchmark::State &state) {                                                                                                                                          \
		auto n = state.range(0);                                                                                                                                                                       \
		std::uniform_int_distribution<size_t> dist(0, n - 1);                                                                                                                                          \
		auto string = randword(n);                                                                                                                                                          \
		DS h(&string, string.size());                                                                                                                                                                \
		h(n - 1); /* preloading */                                                                                                                                                                     \
		for (auto _ : state) {                                                                                                                                                                         \
			size_t a = dist(rng), b = dist(rng);                                                                                                                                                       \
			size_t from = std::min(a, b), len = n - std::max(a, b);                                                                                                                                    \
			benchmark::DoNotOptimize(h(from, len));                                                                                                                                                    \
		}                                                                                                                                                                                              \
	}                                                                                                                                                                                                  \
	BENCHMARK(BM_##NAME##_middle)->Apply(args);

#define BM_IMMEDIATE(NAME, DS)                                                                                                                                                                         \
	static void BM_##NAME##_immediate(benchmark::State &state) {                                                                                                                                       \
		auto n = state.range(0);                                                                                                                                                                       \
		std::uniform_int_distribution<size_t> dist(0, n - 1);                                                                                                                                          \
		auto string = randword(n);                                                                                                                                                          \
		DS h(&string, string.size());                                                                                                                                                                \
		h(n - 1); /* preloading: not needed */                                                                                                                                                         \
		for (auto _ : state) {                                                                                                                                                                         \
			size_t a = dist(rng), b = dist(rng);                                                                                                                                                       \
			size_t from = std::min(a, b), len = n - std::max(a, b);                                                                                                                                    \
			benchmark::DoNotOptimize(h.immediate(from, len));                                                                                                                                          \
		}                                                                                                                                                                                              \
	}                                                                                                                                                                                                  \
	BENCHMARK(BM_##NAME##_immediate)->Apply(args);

#define COMMA ,

BM_PREFIX(XXH3, XXH3Hash<char>)
BM_MIDDLE(XXH3, XXH3Hash<char>)
BM_IMMEDIATE(XXH3, XXH3Hash<char>)

BM_PREFIX(WyHash, WyHash<char>)
BM_MIDDLE(WyHash, WyHash<char>)
BM_IMMEDIATE(Wyhash, WyHash<char>)

BM_PREFIX(CRC32CFolly, CRC32CFollyHash<char>)
BM_MIDDLE(CRC32CFolly, CRC32CFollyHash<char>)
BM_IMMEDIATE(CRC32CFolly, CRC32CFollyHash<char>)

BM_PREFIX(CRC32Zlib, CRC32ZlibHash<char>)
BM_MIDDLE(CRC32Zlib, CRC32ZlibHash<char>)
BM_IMMEDIATE(CRC32Zlib, CRC32ZlibHash<char>)

BM_PREFIX(CRC32Plus32CFolly, CRC32Plus32CFollyHash<char>)
BM_MIDDLE(CRC32Plus32CFolly, CRC32Plus32CFollyHash<char>)
BM_IMMEDIATE(CRC32Plus32CFolly, CRC32Plus32CFollyHash<char>)

BM_PREFIX(RabinKarpHash, RabinKarpHash<char>)
BM_MIDDLE(RabinKarpHash, RabinKarpHash<char>)
BM_IMMEDIATE(RabinKarpHash, RabinKarpHash<char>)

BM_PREFIX(CyclicPoly128, CyclicPolyHash<char COMMA 128>)
BM_MIDDLE(CyclicPoly128, CyclicPolyHash<char COMMA 128>)
BM_IMMEDIATE(CyclicPoly128, CyclicPolyHash<char COMMA 128>)

BM_PREFIX(O1, O1Hash<char>)
BM_MIDDLE(O1, O1Hash<char>)
BM_IMMEDIATE(O1, O1Hash<char>)

BENCHMARK_MAIN();
