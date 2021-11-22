#include <benchmark/benchmark.h>

#include "common.hpp"

using namespace zarr;
using namespace sux::util;

static void BM_Sort(benchmark::State &state) {
	auto n = state.range(0);
	auto string = random(n, "abcdefghijklmnopqrstuvwxyz", 26);

	for (auto _ : state) {
		benchmark::DoNotOptimize(SAConstructByDivSufSort(string));
	}
}
BENCHMARK(BM_Sort)->Range(1 << 10, 1 << 20);

static void BM_SAIS(benchmark::State &state) {
	auto n = state.range(0);
	auto string = random(n, "abcdefghijklmnopqrstuvwxyz", 26);

	for (auto _ : state) {
		benchmark::DoNotOptimize(SAConstructBySAIS(string));
	}
}
BENCHMARK(BM_SAIS)->Range(1 << 10, 1 << 20);

static void BM_DivSufSort(benchmark::State &state) {
	auto n = state.range(0);
	auto string = random(n, "abcdefghijklmnopqrstuvwxyz", 26);

	for (auto _ : state) {
		benchmark::DoNotOptimize(SAConstructByDivSufSort(string));
	}
}
BENCHMARK(BM_DivSufSort)->Range(1 << 10, 1 << 20);

static void BM_LCPStrcmp(benchmark::State &state) {
	auto n = state.range(0);
	auto string = random(n, "abcdefghijklmnopqrstuvwxyz", 26);
	auto sa = SAConstructByDivSufSort(string);

	for (auto _ : state) {
		benchmark::DoNotOptimize(LCPConstructByStrcmp(string, sa));
	}
}
BENCHMARK(BM_LCPStrcmp)->Range(1 << 10, 1 << 20);

static void BM_CTAbouelhoda(benchmark::State &state) {
	auto n = state.range(0);
	auto string = random(n, "abcdefghijklmnopqrstuvwxyz", 26);
	auto sa = SAConstructByDivSufSort(string);
	auto lcp = LCPConstructByStrcmp(string, sa);

	for (auto _ : state) {
		benchmark::DoNotOptimize(CTConstructByAbouelhoda(lcp));
	}
}
BENCHMARK(BM_CTAbouelhoda)->Range(1 << 10, 1 << 20);

BENCHMARK_MAIN();
