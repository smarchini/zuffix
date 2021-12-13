#include <benchmark/benchmark.h>

#include "common.hpp"

using namespace zarr;
using namespace sux::util;

#define BM_SA(NAME, FOO)                                                                                                                                                                               \
	static void BM_SaBy##NAME(benchmark::State &state) {                                                                                                                                               \
		auto n = state.range(0);                                                                                                                                                                       \
		uint8_t charset[] = "abcdefghijklmnopqrstuvwxyz";                                                                                                                                              \
		auto string = random(n, charset, 26);                                                                                                                                                          \
		for (auto _ : state) {                                                                                                                                                                         \
			benchmark::DoNotOptimize(FOO(string));                                                                                                                                                     \
		}                                                                                                                                                                                              \
	}                                                                                                                                                                                                  \
	BENCHMARK(BM_SaBy##NAME)->Range(1 << 10, 1 << 20);

BM_SA(Sort, SAConstructBySort)
BM_SA(DivSufSort, SAConstructByDivSufSort)
BM_SA(GrebnovSAIS, SAConstructByGrebnovSAIS)

#define BM_LCP(NAME, FOO)                                                                                                                                                                              \
	static void BM_LcpBy##NAME(benchmark::State &state) {                                                                                                                                              \
		auto n = state.range(0);                                                                                                                                                                       \
		uint8_t charset[] = "abcdefghijklmnopqrstuvwxyz";                                                                                                                                              \
		auto string = random(n, charset, 26);                                                                                                                                                          \
		auto sa = SAConstructByGrebnovSAIS(string);                                                                                                                                                    \
		for (auto _ : state) {                                                                                                                                                                         \
			benchmark::DoNotOptimize(FOO(string, sa));                                                                                                                                                 \
		}                                                                                                                                                                                              \
	}                                                                                                                                                                                                  \
	BENCHMARK(BM_LcpBy##NAME)->Range(1 << 10, 1 << 20);

BM_LCP(Strcmp, LCPConstructByStrcmp)
BM_LCP(KarkkainenPsi, LCPConstructByKarkkainenPsi)

#define BM_LCP_FIBONACCI(NAME, FOO)                                                                                                                                                                    \
	static void BM_LcpBy##NAME##_fibonacci(benchmark::State &state) {                                                                                                                                  \
		auto n = state.range(0);                                                                                                                                                                       \
		auto string = fibonacci(n);                                                                                                                                                                    \
		auto sa = SAConstructByDivSufSort(string);                                                                                                                                                     \
		for (auto _ : state) {                                                                                                                                                                         \
			benchmark::DoNotOptimize(FOO(string, sa));                                                                                                                                                 \
		}                                                                                                                                                                                              \
	}                                                                                                                                                                                                  \
	BENCHMARK(BM_LcpBy##NAME##_fibonacci)->Range(10, 20);

BM_LCP_FIBONACCI(Strcmp, LCPConstructByStrcmp)
BM_LCP_FIBONACCI(KarkkainenPsi, LCPConstructByKarkkainenPsi)

static void BM_CtByAbouelhoda(benchmark::State &state) {
	auto n = state.range(0);
	uint8_t charset[] = "abcdefghijklmnopqrstuvwxyz";
	auto string = random(n, charset, 26);
	auto sa = SAConstructByGrebnovSAIS(string);
	auto lcp = LCPConstructByKarkkainenPsi(string, sa);
	for (auto _ : state) {
		benchmark::DoNotOptimize(CTConstructByAbouelhoda(lcp));
	}
}
BENCHMARK(BM_CtByAbouelhoda)->Range(1 << 10, 1 << 18);

BENCHMARK_MAIN();
