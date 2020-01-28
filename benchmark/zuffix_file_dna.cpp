#include "common.hpp"
#include <algorithm>
#include <chrono>
#include <vector>

using namespace std;
using namespace zarr;

chrono::nanoseconds::rep find(const ZuffixArray<char, spooky_hash> &za, size_t m, size_t reps) {
  uint64_t u = 0;

  auto pattern = random_dna(m);

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

chrono::nanoseconds::rep find_exact(const ZuffixArray<char, spooky_hash> &za, size_t m,
                                    size_t reps) {
  uint64_t u = 0;

  auto pattern = random_dna(m);

  vector<chrono::nanoseconds::rep> time;
  time.reserve(reps);

  for (uint64_t i = 0; i < reps; i++) {
    auto begin = chrono::high_resolution_clock::now();
    u ^= za.findExact(pattern).length();
    auto end = chrono::high_resolution_clock::now();
    time.push_back(chrono::duration_cast<chrono::nanoseconds>(end - begin).count());
  }

  const volatile uint64_t __attribute__((unused)) unused = u;

  sort(time.begin(), time.end());
  return time[reps / 2];
}

int main(int argc, char **argv) {
  if (argc < 3) {
    cerr << "Not enough parameters: <text file> <m_1> <m_2> ... <m_k> \n";
    return -1;
  }

  ZuffixArray<char, spooky_hash> za(file_to_string(argv[1]));
  cout << "ZuffixArray for file: " << argv[1] << endl;

  constexpr size_t reps = 5;
  for (size_t i = 2; i < argc; i++) {
    size_t m = stoul(argv[i]);
    cout << "Pattern length: " << m;
    cout << ", find: " << pretty(find(za, m, reps)) << " ns";
    cout << ", find exact: " << pretty(find_exact(za, m, reps)) << " ns" << endl;
  }

  return 0;
}
