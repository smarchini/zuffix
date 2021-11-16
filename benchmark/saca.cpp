#include <benchmark/benchmark.h>
#include <random>
#include <sais.hxx>
#include <sux/util/Vector.hpp>
#include <zuffix/random/xoroshiro128plus_engine.hpp>
#include <zuffix/util/String.hpp>
#include <zuffix/util/common.hpp>

using namespace zarr;
using namespace sux::util;

String<char> randomAZ(size_t length) {
	String<char> result(length);
	static std::random_device rd;
	static xoroshiro128plus_engine rng(rd());
	std::uniform_int_distribution<char> dist('A', 'Z');
	for (size_t i = 0; i < length; i++) result[i] = dist(rng);
	return result;
}

static void BM_Sort(benchmark::State &state) {
	auto string = randomAZ(state.range(0));

	for (auto _ : state) {
		benchmark::DoNotOptimize(SAConstructByDivSufSort(string));
	}
}
BENCHMARK(BM_Sort)->Range(1 << 10, 1 << 20);

static void BM_SAIS(benchmark::State &state) {
	auto string = randomAZ(state.range(0));

	for (auto _ : state) {
		benchmark::DoNotOptimize(SAConstructBySAIS(string));
	}
}
BENCHMARK(BM_SAIS)->Range(1 << 10, 1 << 20);

static void BM_DivSufSort(benchmark::State &state) {
	auto string = randomAZ(state.range(0));

	for (auto _ : state) {
		benchmark::DoNotOptimize(SAConstructByDivSufSort(string));
	}
}
BENCHMARK(BM_DivSufSort)->Range(1 << 10, 1 << 20);

BENCHMARK_MAIN();
