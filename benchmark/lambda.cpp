#include <benchmark/benchmark.h>

#include "common.hpp"

using namespace zarr;
using namespace sux::util;

std::random_device rd;
zarr::xoroshiro128plus_engine rng(rd());

size_t lambdabased(int64_t n) {
	std::uniform_int_distribution<int64_t> offset(0, 10);
	int64_t cnt = 0;
	int64_t l = 0, r = n;
	while (l < r) {
		cnt++;
		size_t f = twoFattestR(l, r);
		int64_t x = 1 + offset(rng);
		if (rng() & 1) {
			r = f - x;
		} else {
			l = f + x;
		}
	}
	return cnt;
}

size_t lambdaless(size_t n) {
	std::uniform_int_distribution<int64_t> offset(0, 10);
	int64_t cnt = 0;
	int64_t l = 0, r = n;
	int64_t m = -1ULL << 63;
	while (l < r) {
		while ((m & l) == (m & r)) m >>= 1;
		cnt++;
		size_t f = m & r;
		// assert(twoFattestR(l, r) == f);
		int64_t x = 1 + offset(rng);
		if (rng() & 1) {
			r = f - x;
		} else {
			l = f + x;
		}
	}
	return cnt;
}

static void args(benchmark::internal::Benchmark *b) { b->Args({10})->Args({20})->Args({30})->Args({40})->Args({50})->Args({60}); }

static void BM_lambdabased(benchmark::State &state) {
	auto n = state.range(0);
	std::uniform_int_distribution<size_t> dist(1ULL << (n - 1), 1ULL << n);
	for (auto _ : state) {
		benchmark::DoNotOptimize(lambdabased(dist(rng)));
	}
}
BENCHMARK(BM_lambdabased)->Apply(args);

static void BM_lambdaless(benchmark::State &state) {
	auto n = state.range(0);
	std::uniform_int_distribution<size_t> dist(1ULL << (n - 1), 1ULL << n);
	for (auto _ : state) {
		benchmark::DoNotOptimize(lambdaless(dist(rng)));
	}
}
BENCHMARK(BM_lambdaless)->Apply(args);

BENCHMARK_MAIN();
