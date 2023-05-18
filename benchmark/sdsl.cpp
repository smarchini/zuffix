#include "common.hpp"
#include <benchmark/benchmark.h>
#include <unistd.h>

using namespace zarr;
using namespace sux::util;

std::mt19937 rng(2023);
std::span<SIGMA_T> text;
static constexpr size_t textsize = 200ULL << 20; // TODO: trovare un modo di prenderlo da text (all'interno di args)
// static constexpr size_t textsize = 14930352;   // fibonacci-34
std::string idxfile;

static void args(benchmark::internal::Benchmark *b) {
    for (size_t k = 1; k * 10 < textsize; k *= 10)
        for (size_t i = k; i < k * 10; i += k) b->Arg(i);
}

static void BM_run(benchmark::State &state) {
    static bool is_built = false;
    static auto begin = std::chrono::high_resolution_clock::now();
    static SDSL_STRUCTURE_T ds(idxfile, text);
    static auto end = std::chrono::high_resolution_clock::now();
    static auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
    if (!is_built) {
        std::cout << "Data structureconstruction time: " << time << " ns" << std::endl;
        is_built = true;
        // std::cout << "Run `perf record -g -p " << ::getpid() << "` and press enter" << std::endl;
        // std::cin.ignore();
    }
    static EnhancedSuffixArray<SIGMA_T> reference(text);
    size_t m = state.range(0);
    std::uniform_int_distribution<uint64_t> dist(0, text.size() - m - 1);
    int64_t empty = 0, errors = 0, occurrences = 0;
    for (auto _ : state) {
        state.PauseTiming();
        size_t from = dist(rng);
        auto p = text.subspan(from, m - 1); // TODO: ricordarsi di cambiarlo anche negli altri
        auto expected = reference.find(p).length();
        empty += expected == 0;
        occurrences += expected;
        benchmark::DoNotOptimize(p);
        state.ResumeTiming();
        benchmark::DoNotOptimize(errors += ds.SDSL_COUNT_OP(p) != expected);
        if (errors == 1) {
            std::cout << "Error on pattern: ";
            for (SIGMA_T car : p) std::cout << (char)car;
            std::cout << "\n";
            exit(1);
        }
    }
    state.counters["empty"] = empty;
    state.counters["errors"] = errors;
    state.counters["occurrences"] = occurrences;
    state.counters["length"] = m;
}
BENCHMARK(BM_run)->Apply(args); //->Iterations(10000);

int main(int argc, char **argv) {
    std::ifstream file(argv[1], std::ios::in);
    idxfile = argv[2];
    file.seekg(0, file.end);
    size_t filesize = file.tellg();
    file.seekg(0, file.beg);
    assert(filesize % sizeof(SIGMA_T) == 0 && "Bad file size.");

    size_t length = filesize / sizeof(SIGMA_T) + 1;
    sux::util::Vector<SIGMA_T> buffer(length);
    file.read((char *)&buffer, filesize);
    buffer[length - 1] = std::numeric_limits<SIGMA_T>::max();
    text = std::span<SIGMA_T>(&buffer, length); // global variable

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
