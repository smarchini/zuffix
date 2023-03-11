// TODO: remove me
#include <benchmark/benchmark.h>

#include "common.hpp"

using namespace zarr;
using namespace sux::util;

std::mt19937 rng(2023);

constexpr uint8_t charset[] = {0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
							   32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
							   64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
							   96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
							   128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
							   160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
							   192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
							   224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254};

static void args(benchmark::internal::Benchmark *b) {
	// { text_length, pattern_length }
	for (int i = 1; i < 10; i++) b->Args({1L << 25, 1L << i});
}

#define BM(NAME, DS, SIGMA_T)                                                                                                                                                                            \
	static void BM_##NAME(benchmark::State &state) {                                                                                                                                                   \
		size_t n = state.range(0), m = state.range(1);                                                                                                                                                 \
		auto t = random(n, charset, SIGMA_T);                                                                                                                                                            \
		DS ds(t.substring(0, n));                                                                                                                                                                      \
		std::uniform_int_distribution<uint64_t> dist(0, n - m);                                                                                                                                        \
		int64_t nonempty = 0;                                                                                                                                                                          \
		for (auto _ : state) {                                                                                                                                                                         \
			state.PauseTiming();                                                                                                                                                                       \
			size_t from = dist(rng);                                                                                                                                                                   \
            auto p = t.substring(from, m);                                                                                                                                                             \
            state.ResumeTiming();                                                                                                                                                                      \
            benchmark::DoNotOptimize(nonempty += !ds.find(p).isEmpty());                                                                                                                               \
        }                                                                                                                                                                                              \
		state.counters["nonempty"] = nonempty;                                                                                                                                                         \
	}                                                                                                                                                                                                  \
	BENCHMARK(BM_##NAME)->Apply(args);

#define COMMA ,
#define ALPHABET 2

BM(Simple, SimpleSuffixArray<uint8_t>, ALPHABET)
BM(Enhanced, EnhancedSuffixArray<uint8_t>, ALPHABET)

BM(MemcmpZuffixXXH3, MemcmpZuffixArray<uint8_t COMMA XXH3Hash>, ALPHABET)
BM(SignatureZuffixXXH3, SignatureZuffixArray<uint8_t COMMA XXH3Hash>, ALPHABET)

BM(MemcmpZuffixCRC32CFolly, MemcmpZuffixArray<uint8_t COMMA CRC32CFollyHash>, ALPHABET)
BM(SignatureZuffixCRC32CFolly, SignatureZuffixArray<uint8_t COMMA CRC32CFollyHash>, ALPHABET)

BM(MemcmpZuffixCRC32Zlib, MemcmpZuffixArray<uint8_t COMMA CRC32ZlibHash>, ALPHABET)
BM(SignatureZuffixCRC32Zlib, SignatureZuffixArray<uint8_t COMMA CRC32ZlibHash>, ALPHABET)

BM(MemcmpZuffixCRC32Plus32CFolly, MemcmpZuffixArray<uint8_t COMMA CRC32Plus32CFollyHash>, ALPHABET)
BM(SignatureZuffixCRC32Plus32CFolly, SignatureZuffixArray<uint8_t COMMA CRC32Plus32CFollyHash>, ALPHABET)

// BM(MemcmpZuffixRabinKarp, MemcmpZuffixArray<uint8_t COMMA RabinKarpHash>, ALPHABET)
// BM(SignatureZuffixRabinKarp, SignatureZuffixArray<uint8_t COMMA RabinKarpHash>, ALPHABET)
//
// BM(MemcmpZuffixCyclicPoly128, MemcmpZuffixArray<uint8_t COMMA CyclicPoly128Hash>, ALPHABET)
// BM(SignatureZuffixCyclicPoly128, SignatureZuffixArray<uint8_t COMMA CyclicPoly128Hash>, ALPHABET)

// BM(ZuffixO1, MemcmpZuffixArray<uint8_t COMMA O1Hash>, 4)

BENCHMARK_MAIN();
