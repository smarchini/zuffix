#include <benchmark/benchmark.h>

#include "common.hpp"

using namespace zarr;
using namespace sux::util;

static void BM_SABySort(benchmark::State &state) {
	auto n = state.range(0);
	uint8_t charset[] = "abcdefghijklmnopqrstuvwxyz";
	auto string = random(n, charset, 26);

	for (auto _ : state) {
		benchmark::DoNotOptimize(SAConstructByDivSufSort(string));
	}
}
BENCHMARK(BM_SABySort)->Range(1 << 10, 1 << 20);

static void BM_SAByGrebnovSAIS(benchmark::State &state) {
	auto n = state.range(0);
	uint8_t charset[] = "abcdefghijklmnopqrstuvwxyz";
	auto string = random(n, charset, 26);

	for (auto _ : state) {
		benchmark::DoNotOptimize(SAConstructByGrebnovSAIS(string));
	}
}
BENCHMARK(BM_SAByGrebnovSAIS)->Range(1 << 10, 1 << 20);

static void BM_SAByDivSufSort(benchmark::State &state) {
	auto n = state.range(0);
	uint8_t charset[] = "abcdefghijklmnopqrstuvwxyz";
	auto string = random(n, charset, 26);

	for (auto _ : state) {
		benchmark::DoNotOptimize(SAConstructByDivSufSort(string));
	}
}
BENCHMARK(BM_SAByDivSufSort)->Range(1 << 10, 1 << 20);

static void BM_LCPByStrcmp(benchmark::State &state) {
	auto n = state.range(0);
	uint8_t charset[] = "abcdefghijklmnopqrstuvwxyz";
	auto string = random(n, charset, 26);
	auto sa = SAConstructByDivSufSort(string);

	for (auto _ : state) {
		benchmark::DoNotOptimize(LCPConstructByStrcmp(string, sa));
	}
}
BENCHMARK(BM_LCPByStrcmp)->Range(1 << 10, 1 << 20);

static void BM_LCPByFisherPsi(benchmark::State &state) {
	auto n = state.range(0);
	uint8_t charset[] = "abcdefghijklmnopqrstuvwxyz";
	auto string = random(n, charset, 26);
	auto sa = SAConstructByDivSufSort(string);

	for (auto _ : state) {
		benchmark::DoNotOptimize(LCPConstructionByKarkkainenPsi(string, sa));
	}
}
BENCHMARK(BM_LCPByFisherPsi)->Range(1 << 10, 1 << 20);

static void BM_LCPByStrcmpFibonacci(benchmark::State &state) {
	auto n = state.range(0);
	auto string = fibonacci(n);
	auto sa = SAConstructByDivSufSort(string);

	for (auto _ : state) {
		benchmark::DoNotOptimize(LCPConstructByStrcmp(string, sa));
	}
}
BENCHMARK(BM_LCPByStrcmpFibonacci)->Range(10, 20);

static void BM_LCPByFisherPsiFibonacci(benchmark::State &state) {
	auto n = state.range(0);
	auto string = fibonacci(n);
	auto sa = SAConstructByDivSufSort(string);

	for (auto _ : state) {
		benchmark::DoNotOptimize(LCPConstructionByKarkkainenPsi(string, sa));
	}
}
BENCHMARK(BM_LCPByFisherPsiFibonacci)->Range(10, 20);

static void BM_CTAbouelhoda(benchmark::State &state) {
	auto n = state.range(0);
	uint8_t charset[] = "abcdefghijklmnopqrstuvwxyz";
	auto string = random(n, charset, 26);
	auto sa = SAConstructByDivSufSort(string);
	auto lcp = LCPConstructByStrcmp(string, sa);

	for (auto _ : state) {
		benchmark::DoNotOptimize(CTConstructByAbouelhoda(lcp));
	}
}
BENCHMARK(BM_CTAbouelhoda)->Range(1 << 10, 1 << 20);

BENCHMARK_MAIN();
