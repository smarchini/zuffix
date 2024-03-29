#include "common.hpp"
#include <benchmark/benchmark.h>

using namespace zarr;
using namespace sux::util;

std::span<const SIGMA_T> text;
static constexpr size_t textsize = 200ULL << 20; // TODO: trovare un modo di prenderlo da text (all'interno di args)

static void args(benchmark::internal::Benchmark *b) {
    for (size_t k = 1; k * 10 < textsize; k *= 10)
        for (size_t i = k; i < k * 10; i += k) b->Arg(i);
}

static void BM_run(benchmark::State &state) {
    static bool is_built = false;
    static auto begin = std::chrono::high_resolution_clock::now();
    static STRUCTURE_T ds(text);
    static auto end = std::chrono::high_resolution_clock::now();
    static auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
    if (!is_built)
        std::cout << "Data structure size: " << ds.bitCount() << " bits"
                  << ", construction time: " << time << " ns" << std::endl;
    is_built = true;
    static EnhancedSuffixArray<SIGMA_T, ALLOC_TYPE> reference(text);
    size_t m = state.range(0);
    std::uniform_int_distribution<uint64_t> dist(0, text.size() - m - 1);
    std::uniform_int_distribution<uint64_t> mdist(0, m - 1);
    int64_t errors = 0;
    for (auto _ : state) {
        state.PauseTiming();
        size_t from = dist(rng);
        sux::util::Vector<SIGMA_T, ALLOC_TYPE> buffer(text.data() + from, m);
        buffer[mdist(rng)] = 0;
        auto p = std::span<const SIGMA_T>(&buffer, m);
        benchmark::DoNotOptimize(p);
        state.ResumeTiming();
        benchmark::DoNotOptimize(errors += !ds.find(p).isEmpty());
    }
    state.counters["errors"] = errors;
    state.counters["length"] = m;
}
BENCHMARK(BM_run)->Apply(args)->Iterations(10000);

int main(int argc, char **argv) {
    std::ifstream file(argv[1], std::ios::in);
    file.seekg(0, file.end);
    size_t filesize = file.tellg();
    file.seekg(0, file.beg);
    assert(filesize % sizeof(SIGMA_T) == 0 && "Bad file size.");

    size_t length = filesize / sizeof(SIGMA_T) + 1;
    sux::util::Vector<SIGMA_T, ALLOC_TYPE> buffer(length);
    file.read((char *)&buffer, filesize);
    buffer[length - 1] = std::numeric_limits<SIGMA_T>::max();
    text = std::span<const SIGMA_T>(&buffer, length); // global variable

    SIGMA_T x = 0;
    for (auto c : text) x ^= c;
    benchmark::DoNotOptimize(x);

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
