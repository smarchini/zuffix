#include <benchmark/benchmark.h>
#include <random>
#include <sux/util/Vector.hpp>

std::mt19937 rng(2023);
static constexpr size_t textsize = 200ULL << 20;
const char charset[63] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
std::uniform_int_distribution<size_t> dist(0, 63 - 2);

sux::util::Vector<uint8_t> random_string(size_t n) {
    sux::util::Vector<uint8_t> string;
    string.resize(n);
    for (size_t i = 0; i < n; i++) string[i] = charset[dist(rng)];
    return string;
}

static void args(benchmark::internal::Benchmark *b) {
    for (size_t k = 1; k * 10 < textsize; k *= 10)
        for (size_t i = k; i < k * 10; i += k) b->Arg(i);
}

static void BM_memcmp(benchmark::State &state) {
    auto n = state.range(0);
    sux::util::Vector<uint8_t> s1 = random_string(n);
    sux::util::Vector<uint8_t> s2(&s1, n);
    benchmark::DoNotOptimize(&s1);
    benchmark::DoNotOptimize(&s2);
    std::uniform_int_distribution<size_t> dist(1ULL << (n - 1), 1ULL << n);
    for (auto _ : state) {
        benchmark::ClobberMemory();
        benchmark::DoNotOptimize(memcmp(&s1, &s2, n));
    }
    state.counters["length"] = n;
}
BENCHMARK(BM_memcmp)->Apply(args);

BENCHMARK_MAIN();
