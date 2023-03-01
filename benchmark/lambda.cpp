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


constexpr uint8_t charset[] = {0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
							   32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
							   64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
							   96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
							   128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
							   160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
							   192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
							   224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254};

static void args2(benchmark::internal::Benchmark *b) {
	// { text_length, pattern_length }
	for (int i = 5; i < 10; i++) b->Args({1L << 15, 1L << i});
	for (int i = 15; i < 20; i++) b->Args({1L << 21, 1L << i});
}

#define BM(NAME, OP)                                                                                                                                                                                   \
static void BM_zuffix_##NAME(benchmark::State &state) { \
	size_t n = state.range(0), m = state.range(1); \
	auto t = random(n, charset, 255); \
	zarr::ExactZuffixArray<uint8_t, zarr::WyHash> ds(t.substring(0, n)); \
	static std::random_device rd; \
	static zarr::xoroshiro128plus_engine rng(rd()); \
	std::uniform_int_distribution<uint64_t> dist(0, n - m); \
	for (auto _ : state) { \
		state.PauseTiming(); \
		size_t from = dist(rng); \
		auto p = t.substring(from, m); \
		state.ResumeTiming(); \
		benchmark::DoNotOptimize(ds.OP(p)); \
	} \
} \
BENCHMARK(BM_zuffix_##NAME)->Apply(args2);

BM(lambda, fatBinarySearch)
BM(lambdaless, fatBinarySearch_lambdaless)
BM(quasilambdaless, fatBinarySearch_quasilambdaless)

BENCHMARK_MAIN();
