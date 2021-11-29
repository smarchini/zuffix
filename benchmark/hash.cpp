#include <benchmark/benchmark.h>

#include "common.hpp"

using namespace zarr;
using namespace sux::util;

static void args(benchmark::internal::Benchmark *b) {
	// { length }
	b->Args({1 << 10})->Args({1 << 20})->Args({1 << 30});
}

#define BM_PREFIX(NAME, DS)                                                                                                                                                                            \
	static void BM_##NAME##prefix(benchmark::State &state) {                                                                                                                                           \
		auto n = state.range(0);                                                                                                                                                                       \
		static std::random_device rd;                                                                                                                                                                  \
		static xoroshiro128plus_engine rng(rd());                                                                                                                                                      \
		std::uniform_int_distribution<size_t> dist(0, n - 1);                                                                                                                                          \
		constexpr uint8_t charset[] = "abcdefghijklmnopqrstuvwxyz";                                                                                                                                    \
		auto string = random(n, charset, 26);                                                                                                                                                          \
		DS h(&string);                                                                                                                                                                                 \
		for (auto _ : state) {                                                                                                                                                                         \
			size_t a = dist(rng);                                                                                                                                                                      \
			benchmark::DoNotOptimize(h(a));                                                                                                                                                            \
		}                                                                                                                                                                                              \
	}                                                                                                                                                                                                  \
	BENCHMARK(BM_##NAME##prefix)->Apply(args);

#define BM_MIDDLE(NAME, DS)                                                                                                                                                                            \
	static void BM_##NAME##middle(benchmark::State &state) {                                                                                                                                           \
		auto n = state.range(0);                                                                                                                                                                       \
		static std::random_device rd;                                                                                                                                                                  \
		static xoroshiro128plus_engine rng(rd());                                                                                                                                                      \
		std::uniform_int_distribution<size_t> dist(0, n - 1);                                                                                                                                          \
		constexpr uint8_t charset[] = "abcdefghijklmnopqrstuvwxyz";                                                                                                                                    \
		auto string = random(n, charset, 26);                                                                                                                                                          \
		DS h(&string);                                                                                                                                                                                 \
		for (auto _ : state) {                                                                                                                                                                         \
			size_t a = dist(rng), b = dist(rng);                                                                                                                                                       \
			size_t from = std::min(a, b), len = n - std::max(a, b);                                                                                                                                    \
			benchmark::DoNotOptimize(h(from, len));                                                                                                                                                    \
		}                                                                                                                                                                                              \
	}                                                                                                                                                                                                  \
	BENCHMARK(BM_##NAME##middle)->Apply(args);

#define BM_IMMEDIATE(NAME, DS)                                                                                                                                                                         \
	static void BM_##NAME##immediate(benchmark::State &state) {                                                                                                                                        \
		auto n = state.range(0);                                                                                                                                                                       \
		static std::random_device rd;                                                                                                                                                                  \
		static xoroshiro128plus_engine rng(rd());                                                                                                                                                      \
		std::uniform_int_distribution<size_t> dist(0, n - 1);                                                                                                                                          \
		constexpr uint8_t charset[] = "abcdefghijklmnopqrstuvwxyz";                                                                                                                                    \
		auto string = random(n, charset, 26);                                                                                                                                                          \
		DS h(&string);                                                                                                                                                                                 \
		for (auto _ : state) {                                                                                                                                                                         \
			size_t a = dist(rng), b = dist(rng);                                                                                                                                                       \
			size_t from = std::min(a, b), len = n - std::max(a, b);                                                                                                                                    \
			benchmark::DoNotOptimize(h.immediate(from, len));                                                                                                                                          \
		}                                                                                                                                                                                              \
	}                                                                                                                                                                                                  \
	BENCHMARK(BM_##NAME##immediate)->Apply(args);

#define COMMA ,

BM_PREFIX(RabinKarpHash, RabinKarpHash<uint8_t>)
BM_MIDDLE(RabinKarpHash, RabinKarpHash<uint8_t>)
BM_IMMEDIATE(RabinKarpHash, RabinKarpHash<uint8_t>)

BM_PREFIX(CyclicPoly128, CyclicPolyHash<uint8_t COMMA 128>)
BM_MIDDLE(CyclicPoly128, CyclicPolyHash<uint8_t COMMA 128>)
BM_IMMEDIATE(CyclicPoly128, CyclicPolyHash<uint8_t COMMA 128>)

BM_PREFIX(O1, O1Hash<uint8_t>)
BM_MIDDLE(O1, O1Hash<uint8_t>)
BM_IMMEDIATE(O1, O1Hash<uint8_t>)

BM_PREFIX(XXH3, XXH3Hash<uint8_t>)
BM_MIDDLE(XXH3, XXH3Hash<uint8_t>)
BM_IMMEDIATE(XXH3, XXH3Hash<uint8_t>)

BM_PREFIX(CRC64, CRC64Hash<uint8_t>)
BM_MIDDLE(CRC64, CRC64Hash<uint8_t>)
BM_IMMEDIATE(CRC64, CRC64Hash<uint8_t>)

BENCHMARK_MAIN();
