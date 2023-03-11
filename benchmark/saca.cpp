#include <benchmark/benchmark.h>

#include "common.hpp"

using namespace zarr;
using namespace sux::util;

std::mt19937 rng(2023);
std::string sigma = "abcdefghijklmnopqrstuvwxyz";
std::uniform_int_distribution<uint64_t> dist(0, sigma.length() - 1);

#define BM_SA(NAME, FOO)                                                                                                                                                                               \
	static void BM_SaBy##NAME(benchmark::State &state) {                                                                                                                                               \
		auto n = state.range(0);                                                                                                                                                                       \
		std::unique_ptr<char[]> string(new char[n]);                                                                                                                                                   \
		for (size_t i = 0; i < n - 1; i++) string[i] = sigma[dist(rng)];                                                                                                                               \
		string[n - 1] = std::numeric_limits<char>::max();                                                                                                                                              \
		std::span<const char> text(string.get(), n);                                                                                                                                                   \
		for (auto _ : state) {                                                                                                                                                                         \
			benchmark::DoNotOptimize(text);                                                                                                                                                            \
			benchmark::DoNotOptimize(FOO(text));                                                                                                                                                       \
		}                                                                                                                                                                                              \
	}                                                                                                                                                                                                  \
	BENCHMARK(BM_SaBy##NAME)->Range(1 << 10, 1 << 20);

BM_SA(Sort, SAConstructBySort)
BM_SA(DivSufSort, SAConstructByDivSufSort)
BM_SA(GrebnovSAIS, SAConstructByGrebnovSAIS)

#define BM_LCP(NAME, FOO)                                                                                                                                                                              \
	static void BM_LcpBy##NAME(benchmark::State &state) {                                                                                                                                              \
		auto n = state.range(0);                                                                                                                                                                       \
		std::unique_ptr<char[]> string(new char[n]);                                                                                                                                                   \
		for (size_t i = 0; i < n - 1; i++) string[i] = sigma[dist(rng)];                                                                                                                               \
		string[n - 1] = std::numeric_limits<char>::max();                                                                                                                                              \
		std::span<const char> text(string.get(), n);                                                                                                                                                   \
		auto sa = SAConstructByGrebnovSAIS(text);                                                                                                                                                      \
		for (auto _ : state) {                                                                                                                                                                         \
			benchmark::DoNotOptimize(text);                                                                                                                                                            \
			benchmark::DoNotOptimize(sa);                                                                                                                                                              \
			benchmark::DoNotOptimize(FOO(text, sa));                                                                                                                                                   \
		}                                                                                                                                                                                              \
	}                                                                                                                                                                                                  \
	BENCHMARK(BM_LcpBy##NAME)->Range(1 << 10, 1 << 20);

BM_LCP(Strcmp, LCPConstructByStrcmp)
BM_LCP(KarkkainenPsi, LCPConstructByKarkkainenPsi)
BM_LCP(GrebnovSAIS, LCPConstructByGrebnovSAIS)

std::string fibostring(size_t n) {
	std::string prec = "a", curr = "ab";
	for (size_t i = 0; i < n; i++) {
		std::string tmp = prec;
		prec = curr;
		curr += tmp;
	}
	return curr;
}

#define BM_LCP_FIBONACCI(NAME, FOO)                                                                                                                                                                    \
	static void BM_LcpBy##NAME##_fibonacci(benchmark::State &state) {                                                                                                                                  \
		auto n = state.range(0);                                                                                                                                                                       \
		auto string = fibostring(n);                                                                                                                                                                   \
		string.push_back(std::numeric_limits<char>::max());                                                                                                                                            \
		std::span<const char> text(string);                                                                                                                                                   \
		auto sa = SAConstructByDivSufSort(text);                                                                                                                                                       \
		for (auto _ : state) {                                                                                                                                                                         \
			benchmark::DoNotOptimize(text);                                                                                                                                                            \
			benchmark::DoNotOptimize(sa);                                                                                                                                                              \
			benchmark::DoNotOptimize(FOO(text, sa));                                                                                                                                                   \
		}                                                                                                                                                                                              \
	}                                                                                                                                                                                                  \
	BENCHMARK(BM_LcpBy##NAME##_fibonacci)->Range(10, 20);

BM_LCP_FIBONACCI(Strcmp, LCPConstructByStrcmp)
BM_LCP_FIBONACCI(KarkkainenPsi, LCPConstructByKarkkainenPsi)
BM_LCP_FIBONACCI(GrebnovSAIS, LCPConstructByGrebnovSAIS)

static void BM_CtByAbouelhoda(benchmark::State &state) {
	auto n = state.range(0);
	std::unique_ptr<char[]> string(new char[n]);
	for (size_t i = 0; i < n - 1; i++) string[i] = sigma[dist(rng)];
	string[n - 1] = std::numeric_limits<char>::max();
	std::span<const char> text(string.get(), n);
	auto sa = SAConstructByGrebnovSAIS(text);
	auto lcp = LCPConstructByKarkkainenPsi(text, sa);
	for (auto _ : state) {
		benchmark::DoNotOptimize(lcp);
		benchmark::DoNotOptimize(CTConstructByAbouelhoda(lcp));
	}
}
BENCHMARK(BM_CtByAbouelhoda)->Range(1 << 10, 1 << 18);

BENCHMARK_MAIN();
