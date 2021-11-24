#include <benchmark/benchmark.h>

#include "common.hpp"

using namespace zarr;
using namespace sux::util;

static void BM_RabinKarpHash(benchmark::State &state) {
	auto n = state.range(0);
	static std::random_device rd;
	static xoroshiro128plus_engine rng(rd());
	std::uniform_int_distribution<size_t> dist(0, n - 1);
	uint8_t charset[] = "abcdefghijklmnopqrstuvwxyz";
	auto string = random(n, charset, 26);

	RabinKarpHash<uint8_t> h(&string);

	for (auto _ : state) {
		size_t a = dist(rng), b = dist(rng);
		size_t from = std::min(a, b), len = n - std::max(a, b);
		benchmark::DoNotOptimize(h(from, len));
	}
}
BENCHMARK(BM_RabinKarpHash)
	->Args({1 << 10})
	->Args({2 << 10})
	->Args({3 << 10})
	->Args({4 << 10})
	->Args({5 << 10})
	->Args({6 << 10})
	->Args({7 << 10})
	->Args({8 << 10})
	->Args({9 << 10})
	->Args({1 << 20})
	->Args({2 << 20})
	->Args({3 << 20})
	->Args({4 << 20})
	->Args({5 << 20})
	->Args({6 << 20})
	->Args({7 << 20})
	->Args({8 << 20})
	->Args({9 << 20});

static void BM_CyclicPoly128(benchmark::State &state) {
	auto n = state.range(0);
	static std::random_device rd;
	static xoroshiro128plus_engine rng(rd());
	std::uniform_int_distribution<size_t> dist(0, n - 1);
	uint8_t charset[] = "abcdefghijklmnopqrstuvwxyz";
	auto string = random(n, charset, 26);

	CyclicPolyHash<uint8_t, 128> h(&string);

	for (auto _ : state) {
		size_t a = dist(rng), b = dist(rng);
		size_t from = std::min(a, b), len = n - std::max(a, b);
		benchmark::DoNotOptimize(h(from, len));
	}
}
BENCHMARK(BM_CyclicPoly128)
	->Args({1 << 10})
	->Args({2 << 10})
	->Args({3 << 10})
	->Args({4 << 10})
	->Args({5 << 10})
	->Args({6 << 10})
	->Args({7 << 10})
	->Args({8 << 10})
	->Args({9 << 10})
	->Args({1 << 20})
	->Args({2 << 20})
	->Args({3 << 20})
	->Args({4 << 20})
	->Args({5 << 20})
	->Args({6 << 20})
	->Args({7 << 20})
	->Args({8 << 20})
	->Args({9 << 20});

static void BM_O1(benchmark::State &state) {
	auto n = state.range(0);
	static std::random_device rd;
	static xoroshiro128plus_engine rng(rd());
	std::uniform_int_distribution<size_t> dist(0, n - 1);
	uint8_t charset[] = "abcdefghijklmnopqrstuvwxyz";
	auto string = random(n, charset, 26);

	O1Hash<uint8_t> h(&string);

	for (auto _ : state) {
		size_t a = dist(rng), b = dist(rng);
		size_t from = std::min(a, b), len = n - std::max(a, b);
		benchmark::DoNotOptimize(h(from, len));
	}
}
BENCHMARK(BM_O1)
	->Args({1 << 10})
	->Args({2 << 10})
	->Args({3 << 10})
	->Args({4 << 10})
	->Args({5 << 10})
	->Args({6 << 10})
	->Args({7 << 10})
	->Args({8 << 10})
	->Args({9 << 10})
	->Args({1 << 20})
	->Args({2 << 20})
	->Args({3 << 20})
	->Args({4 << 20})
	->Args({5 << 20})
	->Args({6 << 20})
	->Args({7 << 20})
	->Args({8 << 20})
	->Args({9 << 20});

static void BM_XXH3(benchmark::State &state) {
	auto n = state.range(0);
	static std::random_device rd;
	static xoroshiro128plus_engine rng(rd());
	std::uniform_int_distribution<size_t> dist(0, n - 1);
	uint8_t charset[] = "abcdefghijklmnopqrstuvwxyz";
	auto string = random(n, charset, 26);

	XXH3Hash<uint8_t> h(&string);

	for (auto _ : state) {
		size_t a = dist(rng), b = dist(rng);
		size_t from = std::min(a, b), len = n - std::max(a, b);
		benchmark::DoNotOptimize(h(from, len));
	}
}
BENCHMARK(BM_XXH3)
	->Args({1 << 10})
	->Args({2 << 10})
	->Args({3 << 10})
	->Args({4 << 10})
	->Args({5 << 10})
	->Args({6 << 10})
	->Args({7 << 10})
	->Args({8 << 10})
	->Args({9 << 10})
	->Args({1 << 20})
	->Args({2 << 20})
	->Args({3 << 20})
	->Args({4 << 20})
	->Args({5 << 20})
	->Args({6 << 20})
	->Args({7 << 20})
	->Args({8 << 20})
	->Args({9 << 20});
BENCHMARK_MAIN();
