#include "common.hpp"
#include "hash.hpp"

#include <algorithm>
#include <chrono>
#include <vector>

#include <zuffix/ZuffixArray.hpp>

using namespace std;
using namespace zarr;

chrono::nanoseconds::rep find(const ZuffixArray<SYMBOLTYPE, spooky_hash> &za,
                              const String<SYMBOLTYPE> &pattern) {
  constexpr size_t reps = 5;
  uint64_t u = 0;

  vector<chrono::nanoseconds::rep> time;
  time.reserve(reps);

  for (uint64_t i = 0; i < reps; i++) {
    auto begin = chrono::high_resolution_clock::now();
    u ^= za.find(pattern).length();
    auto end = chrono::high_resolution_clock::now();
    time.push_back(chrono::duration_cast<chrono::nanoseconds>(end - begin).count());
  }

  const volatile uint64_t __attribute__((unused)) unused = u;

  sort(time.begin(), time.end());
  return time[reps / 2];
}

int main(int argc, char **argv) {

  if (argc < 3) {
    cerr << "Not enough parameters: <text> <pattern 1> <pattern 2> ... <pattern n> \n";
    return -1;
  }

  ZuffixArray<SYMBOLTYPE, spooky_hash> za(file_to_string(argv[1]));
  cout << "ZuffixArray for text: " << argv[1] << endl;

  for (size_t i = 2; i < argc; i++) {
    auto pattern = file_to_string(argv[i]);
    cout << "Pattern: " << argv[i];
    cout << ", find: " << pretty(find(za, pattern)) << " ns" << endl;
  }

  return 0;
}
