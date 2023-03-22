#include <benchmark/benchmark.h>

#include "common.hpp"

using namespace zarr;
using namespace sux::util;

std::span<const char> text;
constexpr size_t N = 40; // ~256MB // TODO bisogna fare tentativi per capire una dimensione fattible visti i tempi di costruizione
const std::string a = "a", ab = "ab";

sux::util::Vector<char, ALLOC_TYPE> fibostring(size_t n) {
    std::string prec = a, curr = ab;
    if (n == 0) return sux::util::Vector<char, ALLOC_TYPE>(a.c_str(), a.length());
    for (size_t i = 1; i < n; i++) {
        std::string tmp = prec;
        prec = curr;
        curr += tmp;
    }
    return sux::util::Vector<char, ALLOC_TYPE>(curr.c_str(), curr.length());
}

static void args(benchmark::internal::Benchmark *b) {
    // pattern_length: i-th fibonacci word
    for (long i = 0; i < N; i++) b->Arg(i);
}

static void BM_run(benchmark::State &state) {
    static STRUCTURE_T ds(text);
    static EnhancedSuffixArray<char> reference(text);
    size_t m = state.range(0);
    auto p = fibostring(m);
    int64_t empty = 0, errors = 0, occurrences = 0;
    for (auto _ : state) {
        state.PauseTiming();
        auto pattern = std::span<const char>(&p, p.size());
        auto expected = reference.find(pattern);
        empty += expected.isEmpty();
        occurrences = expected.length(); // fixed pattern, so `=` makes more sense than `=`
        benchmark::DoNotOptimize(pattern);
        state.ResumeTiming();
        auto result = ds.find(pattern);
        benchmark::DoNotOptimize(result);
    }
    state.counters["empty"] = empty;
    state.counters["errors"] = errors;
    state.counters["occurrences"] = occurrences;
    state.counters["length"] = p.size();
}
BENCHMARK(BM_run)->Apply(args);

int main(int argc, char **argv) {
    auto string = fibostring(N);
    string.pushBack(std::numeric_limits<char>::max());
    text = std::span<const char>(&string, string.size()); // global variable

    char x = 0;
    for (auto c: text) x ^= c;
    benchmark::DoNotOptimize(x);

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
