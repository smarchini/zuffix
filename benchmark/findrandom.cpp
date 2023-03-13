#include "common.hpp"
#include <benchmark/benchmark.h>

using namespace zarr;
using namespace sux::util;

std::mt19937 rng(2023);
std::span<const SIGMA_T> text;

static void args(benchmark::internal::Benchmark *b) {
    for (size_t k = 10; k * 10 < 10 * 1000 * 1000; k *= 10)
        for (int i = k; i < k * 10; i += k)
            b->Arg(i);
}

static void BM_run(benchmark::State &state) {
    static STRUCTURE_T ds(text);
    static EnhancedSuffixArray<SIGMA_T> reference(text);
    size_t m = state.range(0);
    std::uniform_int_distribution<uint64_t> dist(0, text.size() - m - 1);
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
    std::ifstream file(argv[1], std::ios::in);
    file.seekg(0, file.end);
    size_t filesize = file.tellg();
    file.seekg(0, file.beg);
    assert(filesize % sizeof(SIGMA_T) == 0 && "Bad file size.");
    std::unique_ptr<SIGMA_T[]> buffer(new SIGMA_T[(filesize + 1) / sizeof(SIGMA_T)]);
    file.read((char *)buffer.get(), filesize);
    buffer[filesize] = std::numeric_limits<SIGMA_T>::max();
    text = std::span<const SIGMA_T>(buffer.get(), filesize + 1);

    // Google Benchmark's stuff
    char arg0_default[] = "benchmark";
    char *args_default = arg0_default;
    if (!argv) {
        argc = 1;
        argv = &args_default;
    }
    ::benchmark::Initialize(&argc, argv);
    // if (::benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
    ::benchmark::RunSpecifiedBenchmarks();
    ::benchmark::Shutdown();
    return 0;
}
