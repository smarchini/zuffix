#include <benchmark/benchmark.h>

#include "common.hpp"

using namespace zarr;
using namespace sux::util;

static void args(benchmark::internal::Benchmark *b) {
	// for (int i = 5; i < 10; i++) b->Args({1L << 10, 1L << i}); // { text_length, pattern_length }
	for (int i = 10; i < 15; i++)
		b->Args({1L << 15, 1L << i}); // { text_length, pattern_length }
									  // for (int i = 10; i < 25; i++) b->Args({1L << 20, 1L << i}); // { text_length, pattern_length }
}

static void BM_Simple(benchmark::State &state) {
	size_t n = state.range(0), m = state.range(1);
	auto t = random(n, "ACGT", 4);
	SimpleSuffixArray<char> simple(t.substring(0, n));

	static std::random_device rd;
	static zarr::xoroshiro128plus_engine rng(rd());
	std::uniform_int_distribution<uint8_t> dist(0, n - m);

	int64_t nonempty = 0;
	for (auto _ : state) {
		size_t from = dist(rng);
		auto p = t.substring(from, m);
		benchmark::DoNotOptimize(nonempty += !simple.find(p).isEmpty());
	}

	state.counters["nonempty"] = nonempty;
}
BENCHMARK(BM_Simple)->Apply(args);

static void BM_Enhanced(benchmark::State &state) {
	size_t n = state.range(0), m = state.range(1);
	auto t = random(n, "ACGT", 4);
	EnhancedSuffixArray<char> enhanced(t.substring(0, n));

	static std::random_device rd;
	static zarr::xoroshiro128plus_engine rng(rd());
	std::uniform_int_distribution<uint8_t> dist(0, n - m);

	int64_t nonempty = 0;
	for (auto _ : state) {
		size_t from = dist(rng);
		auto p = t.substring(from, m);
		benchmark::DoNotOptimize(nonempty += !enhanced.find(p).isEmpty());
	}

	state.counters["nonempty"] = nonempty;
}
BENCHMARK(BM_Enhanced)->Apply(args);

static void BM_ZuffixRabinKarp(benchmark::State &state) {
	size_t n = state.range(0), m = state.range(1);
	auto t = random(n, "ACGT", 4);
	EnhancedZuffixArray<char, RabinKarpHash> zuffix(t.substring(0, n));

	static std::random_device rd;
	static zarr::xoroshiro128plus_engine rng(rd());
	std::uniform_int_distribution<uint8_t> dist(0, n - m);

	int64_t nonempty = 0;
	for (auto _ : state) {
		size_t from = dist(rng);
		auto p = t.substring(from, m);
		benchmark::DoNotOptimize(nonempty += !zuffix.find(p).isEmpty());
	}

	state.counters["nonempty"] = nonempty;
}
BENCHMARK(BM_ZuffixRabinKarp)->Apply(args);

static void BM_ZuffixCyclicPoly128(benchmark::State &state) {
	size_t n = state.range(0), m = state.range(1);
	auto t = random(n, "ACGT", 4);
	EnhancedZuffixArray<char, CyclicPoly128Hash> zuffix(t.substring(0, n));

	static std::random_device rd;
	static zarr::xoroshiro128plus_engine rng(rd());
	std::uniform_int_distribution<uint8_t> dist(0, n - m);

	int64_t nonempty = 0;
	for (auto _ : state) {
		size_t from = dist(rng);
		auto p = t.substring(from, m);
		benchmark::DoNotOptimize(nonempty += !zuffix.find(p).isEmpty());
	}

	state.counters["nonempty"] = nonempty;
}
BENCHMARK(BM_ZuffixCyclicPoly128)->Apply(args);

static void BM_ZuffixO1(benchmark::State &state) {
	size_t n = state.range(0), m = state.range(1);
	auto t = random(n, "ACGT", 4);
	EnhancedZuffixArray<char, O1Hash> zuffix(t.substring(0, n));

	static std::random_device rd;
	static zarr::xoroshiro128plus_engine rng(rd());
	std::uniform_int_distribution<uint8_t> dist(0, n - m);

	int64_t nonempty = 0;
	for (auto _ : state) {
		size_t from = dist(rng);
		auto p = t.substring(from, m);
		benchmark::DoNotOptimize(nonempty += !zuffix.find(p).isEmpty());
	}

	state.counters["nonempty"] = nonempty;
}
BENCHMARK(BM_ZuffixO1)->Apply(args);

static void BM_ZuffixXXH3(benchmark::State &state) {
	size_t n = state.range(0), m = state.range(1);
	auto t = random(n, "ACGT", 4);
	EnhancedZuffixArray<char, XXH3Hash> zuffix(t.substring(0, n));

	static std::random_device rd;
	static zarr::xoroshiro128plus_engine rng(rd());
	std::uniform_int_distribution<uint8_t> dist(0, n - m);

	int64_t nonempty = 0;
	for (auto _ : state) {
		size_t from = dist(rng);
		auto p = t.substring(from, m);
		benchmark::DoNotOptimize(nonempty += !zuffix.find(p).isEmpty());
	}

	state.counters["nonempty"] = nonempty;
}
BENCHMARK(BM_ZuffixXXH3)->Apply(args);

BENCHMARK_MAIN();
