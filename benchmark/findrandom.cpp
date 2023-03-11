#include "common.hpp"
#include <benchmark/benchmark.h>

using namespace zarr;
using namespace sux::util;

std::span<const SIGMA_T> text;
constexpr size_t N = 1ULL << 28;
std::mt19937 rng(2023);

static void args(benchmark::internal::Benchmark *b) {
    for (int i = 1; i <= 24; i++) b->Args({1L << i});
}

static void BM_run(benchmark::State &state) {
    static STRUCTURE_T ds(text);
    static EnhancedSuffixArray<SIGMA_T> reference(text);
    size_t m = state.range(0);
    std::uniform_int_distribution<uint64_t> dist(0, N - m - 1);
    int64_t empty = 0, errors = 0, occurrences = 0;
    for (auto _ : state) {
        state.PauseTiming();
        size_t from = dist(rng);
        auto p = text.subspan(from, m);
        auto expected = reference.find(p);
        empty += expected.isEmpty();
        occurrences += expected.length();
        benchmark::DoNotOptimize(p);
        state.ResumeTiming();
        benchmark::DoNotOptimize(errors += ds.find(p) != expected);
    }
    state.counters["empty"] = empty;
    state.counters["errors"] = errors;
    state.counters["occurrences"] = occurrences;
}
BENCHMARK(BM_run)->Apply(args)->Iterations(10000);

int main(int argc, char **argv) {
    std::string sigma = "ACGT";
    std::uniform_int_distribution<uint64_t> dist(0, sigma.length() - 1);
    std::unique_ptr<SIGMA_T[]> string(new SIGMA_T[N]);
    for (size_t i = 0; i < N - 1; i++) string[i] = (SIGMA_T)sigma[dist(rng)];
    string[N - 1] = std::numeric_limits<SIGMA_T>::max();
    text = std::span<const SIGMA_T>(string.get(), N); // global variable

    // Google Benchmark's stuff
    char arg0_default[] = "benchmark";
    char *args_default = arg0_default;
    if (!argv) {
        argc = 1;
        argv = &args_default;
    }
    ::benchmark::Initialize(&argc, argv);
    if (::benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
    ::benchmark::RunSpecifiedBenchmarks();
    ::benchmark::Shutdown();
    return 0;
}
