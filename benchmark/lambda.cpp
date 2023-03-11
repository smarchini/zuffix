#include <benchmark/benchmark.h>

#include "common.hpp"

using namespace zarr;
using namespace sux::util;

std::mt19937 rng(2023);

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

static void BM_dummy_lambdabased(benchmark::State &state) {
	auto n = state.range(0);
	std::uniform_int_distribution<size_t> dist(1ULL << (n - 1), 1ULL << n);
	for (auto _ : state) {
		benchmark::DoNotOptimize(lambdabased(dist(rng)));
	}
}
BENCHMARK(BM_dummy_lambdabased)->Apply(args);

static void BM_dummy_lambdaless(benchmark::State &state) {
	auto n = state.range(0);
	std::uniform_int_distribution<size_t> dist(1ULL << (n - 1), 1ULL << n);
	for (auto _ : state) {
		benchmark::DoNotOptimize(lambdaless(dist(rng)));
	}
}
BENCHMARK(BM_dummy_lambdaless)->Apply(args);

static void args2(benchmark::internal::Benchmark *b) {
	// { text_length, pattern_length }
	for (int i = 5; i < 10; i++) b->Args({1L << 15, 1L << i});
	for (int i = 15; i < 20; i++) b->Args({1L << 21, 1L << i});
}

// TODO: Don't use a random-generated text!
#define BM(NAME, OP)                                                                                                                                                                                   \
	static void BM_zuffix_##NAME(benchmark::State &state) {                                                                                                                                            \
		size_t n = state.range(0), m = state.range(1);                                                                                                                                                 \
		std::string sigma = "ACGT";                                                                                                                                                                    \
		std::uniform_int_distribution<uint64_t> sigmadist(0, sigma.length() - 1);                                                                                                                      \
		std::unique_ptr<char[]> string(new char[n]);                                                                                                                                                   \
		for (size_t i = 0; i < n - 1; i++) string[i] = sigma[sigmadist(rng)];                                                                                                                          \
		string[n - 1] = std::numeric_limits<char>::max();                                                                                                                                              \
		auto text = std::span<char>(string.get(), n);                                                                                                                                                  \
		zarr::MemcmpZuffixArray<char, zarr::WyHash> ds(text);                                                                                                                                          \
		std::uniform_int_distribution<size_t> dist(0, n - m - 1);                                                                                                                                      \
		for (auto _ : state) {                                                                                                                                                                         \
			state.PauseTiming();                                                                                                                                                                       \
			size_t from = dist(rng);                                                                                                                                                                   \
			auto pattern = text.subspan(from, m);                                                                                                                                                      \
			benchmark::DoNotOptimize(pattern);                                                                                                                                                         \
			state.ResumeTiming();                                                                                                                                                                      \
			benchmark::DoNotOptimize(ds.OP(pattern));                                                                                                                                                  \
		}                                                                                                                                                                                              \
	}                                                                                                                                                                                                  \
	BENCHMARK(BM_zuffix_##NAME)->Apply(args2);

BM(lambdabased, fatBinarySearch_lambdabased)
BM(lambdaless, fatBinarySearch_lambdaless)
BM(quasilambdaless, fatBinarySearch_quasilambdaless)
BM(final, fatBinarySearch)

BENCHMARK_MAIN();
