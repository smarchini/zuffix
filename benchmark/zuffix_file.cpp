#include "common.hpp"
#include "hash.hpp"

#include <algorithm>
#include <chrono>
#include <tuple>
#include <vector>

#include <zuffix/ZuffixArray.hpp>
#include <zuffix/util/String.hpp>

using namespace std;
using namespace zarr;

tuple<size_t, chrono::nanoseconds::rep> find(const ZuffixArray<SYMBOLTYPE, spooky_hash> &za,
                                             const String<SYMBOLTYPE> &pattern) {
  constexpr size_t reps = 5;
  uint64_t u = 0;
  size_t result;

  vector<chrono::nanoseconds::rep> time;
  time.reserve(reps);

  for (uint64_t i = 0; i < reps; i++) {
    auto begin = chrono::high_resolution_clock::now();
    u ^= za.find(pattern).length();
    auto end = chrono::high_resolution_clock::now();
    time.push_back(chrono::duration_cast<chrono::nanoseconds>(end - begin).count());
    if (i == 0) result = u;
  }

  const volatile uint64_t __attribute__((unused)) unused = u;

  sort(time.begin(), time.end());
  return make_tuple(result, time[reps / 2]);
}

tuple<chrono::nanoseconds::rep, chrono::nanoseconds::rep>
median(vector<chrono::nanoseconds::rep> record) {
  auto n = record.size();
  auto mid = record[n / 2];
  return make_tuple(mid, max(mid - record[0], record[n - 1] - mid));
}

tuple<chrono::nanoseconds::rep, chrono::nanoseconds::rep>
average(vector<chrono::nanoseconds::rep> record) {
  auto n = record.size();
  auto avg = accumulate(begin(record), end(record), .0) / n;
  return make_tuple(avg, avg - record[0]);
}

int main(int argc, char **argv) {
  if (argc < 3) {
    cerr << "Not enough parameters: <text> <pattern>\n";
    cerr << "To build text and pattern you may want to use Pizza&Chili utils: ";
    cerr << "http://pizzachili.dcc.uchile.cl/experiments.html \n";
    return -1;
  }

  ZuffixArray<SYMBOLTYPE, spooky_hash> za(file_to_string<SYMBOLTYPE>(argv[1], true));
  cout << "ZuffixArray for text: " << argv[1] << "\n" << endl;

  for (size_t i = 2; i < argc; i++) {
    auto [file, number, length, p] = load_pizzachili_patterns<SYMBOLTYPE>(argv[i]);
    cout << "Pattern: " << argv[i] << endl;

    uint64_t sum = 0;
    vector<chrono::nanoseconds::rep> record;
    record.reserve(number);

    for (const auto &pattern : p) {
      auto [count, time] = find(za, pattern);
      sum += count;
      record.push_back(time);
      cout << "\tlength: " << pattern.length() << ", count: " << count << ", time: " << pretty(time)
           << " ns" << endl;
    }

    sort(begin(record), end(record));
    auto [med, devmed] = median(record);
    auto [avg, devavg] = average(record);
    cout << "Total number of occurrences: " << sum << endl;
    cout << "Median time: " << pretty(med) << " ns +/- " << pretty(devmed) << " ns\n";
    cout << "Average time: " << pretty(avg) << " ns +/- " << pretty(devavg) << " ns\n";
    cout << "Total time: " << accumulate(begin(record), end(record), .0) / 1000000 << " ms" << endl;
  }

  return 0;
}
