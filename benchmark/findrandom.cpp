#include "common.hpp"
#include <benchmark/benchmark.h>

using namespace zarr;
using namespace sux::util;

String<SIGMA_T> *string;
constexpr size_t N = 1ULL << 28;
xoroshiro128plus_engine rng(0x2023);

STRUCTURE_T *ds;
EnhancedSuffixArray<SIGMA_T> *reference;

static void args(benchmark::internal::Benchmark *b) {
    for (int i = 1; i <= 24; i++) b->Args({1L << i});
}

static void BM_run(benchmark::State &state) {
    size_t m = state.range(0);
    std::uniform_int_distribution<uint64_t> dist(0, N - m);
    int64_t empty = 0, errors = 0, occurrences = 0;
    for (auto _ : state) {
        state.PauseTiming();
        size_t from = dist(rng);
        auto p = string->substring(from, m);
        auto expected = reference->find(p);
        empty += expected.isEmpty();
        occurrences += expected.length();
        benchmark::DoNotOptimize(p);
        state.ResumeTiming();
        benchmark::DoNotOptimize(errors += ds->find(p) != expected);
    }
    state.counters["empty"] = empty;
    state.counters["errors"] = errors;
    state.counters["occurrences"] = occurrences;
}
BENCHMARK(BM_run)->Apply(args)->Iterations(10000);

int main(int argc, char **argv) {
    string = new String<SIGMA_T>(random(N, (const SIGMA_T*)"ACGT", 4));

    ds = new STRUCTURE_T(string->substring(0, N));
    reference = new EnhancedSuffixArray(string->substring(0, N));

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
