#include <benchmark/benchmark.h>
#include <random>
#include <sux/util/Vector.hpp>
#include <zuffix/random/xoroshiro128plus_engine.hpp>
#include <zuffix/util/String.hpp>
#include <zuffix/external/sais.hxx>

zarr::String<char> randomAZ(size_t length) {
  zarr::String<char> result(length);

  static std::random_device rd;
  static zuffix::xoroshiro128plus_engine rng(rd());
  static constexpr char min = 'A';
  static constexpr char max = 'Z'; // max() is reserved for $
  std::uniform_int_distribution<char> dist(min, max);

  for (size_t i = 0; i < length; i++)
    result[i] = dist(rng);

  return result;
}

template <typename T>
static sux::util::Vector<size_t> quicksort(const zarr::String<T> &string) {
  sux::util::Vector<size_t> result(string.size() + 1);

  for (size_t i = 0; i < result.size(); i++)
    result[i] = i;

  std::sort(&result, &result + result.size(), [&string](size_t i, size_t j) {
    while (i < string.size() && j < string.size() && string[i] == string[j]) {
      i++;
      j++;
    }

    if (i == string.size()) return false;
    if (j == string.size()) return true;

    return string[i] < string[j];
  });

  return result;
}

static void BM_Quicksort(benchmark::State &state) {
  auto string = randomAZ(state.range(0));

  for (auto _ : state) {
    benchmark::DoNotOptimize(quicksort(string));
  }
}
BENCHMARK(BM_Quicksort)->Range(1 << 10, 1 << 20);

template <typename T>
static sux::util::Vector<size_t> sais(const zarr::String<T> &string) {
  sux::util::Vector<size_t> result(string.size());
  int ret = saisxx((T *)&string, (long *)&result, (long)string.size(), 256L);
  assert(ret == 0 && "Error occurred during suffix array construction");
  return result;
}

static void BM_Sais(benchmark::State &state) {
  auto string = randomAZ(state.range(0));

  for (auto _ : state) {
    benchmark::DoNotOptimize(sais(string));
  }
}
BENCHMARK(BM_Sais)->Range(1 << 10, 1 << 20);

BENCHMARK_MAIN();