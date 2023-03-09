// #define DEBUG // Enable statistics (must be defined before common.hpp)

#include "common.hpp"
#include <benchmark/benchmark.h>
#include <unistd.h>

#define _STRINGIFY(...) #__VA_ARGS__
#define STRINGIFY(...) _STRINGIFY(__VA_ARGS__)

using namespace std;
using namespace zarr;

int main(int argc, char **argv) {
	if (argc < 3) {
		cerr << "Not enough parameters: <text.index> <pattern>\n";
		cerr << "To build text and pattern you may want to use Pizza&Chili utils: ";
		cerr << "http://pizzachili.dcc.uchile.cl/experiments.html \n";
		return -1;
	}

	std::ifstream file(argv[1], std::ios::in);
	DATASTRUCTURETYPE ds(file_to_string<SYMBOLTYPE>(argv[1]));
	cout << STRINGIFY(DATASTRUCTURETYPE) << ": " << argv[1] << "\n" << endl;

	for (int i = 2; i < argc; i++) {
		cout << "Pattern: " << argv[i] << endl;
		auto [file, number, length, p] = load_pizzachili_patterns<SYMBOLTYPE>(argv[i]);

		uint64_t sum = 0;
		vector<chrono::nanoseconds::rep> record;
		record.reserve(number);

		cout << "Run `perf record -g -p " << ::getpid() << "` and press enter" << std::endl;
		cin.ignore();

		LInterval<size_t> result;
		for (const auto &pattern : p) {
			auto begin = chrono::high_resolution_clock::now();
			for (size_t reps = 0; reps < 100; reps++) {
				benchmark::DoNotOptimize(pattern);
				result = ds.find(pattern);
				benchmark::DoNotOptimize(result);
			}
			auto end = chrono::high_resolution_clock::now();
			sum += result.length();
			// cout << result << endl; // TODO remove me
			record.push_back(chrono::duration_cast<chrono::nanoseconds>(end - begin).count());
		}

		sort(begin(record), end(record));
		auto [med, devmed] = median(record);
		auto [avg, devavg] = mean(record);

		cout << "Total number of occurrences: " << sum << endl;
		cout << "Median time: " << pretty(med) << " ns +/- " << pretty(devmed) << " ns\n";
		cout << "Mean time: " << pretty(avg) << " ns +/- " << pretty(devavg) << " ns\n";
		cout << "Total time: " << accumulate(begin(record), end(record), .0) / 1000000 << " ms" << endl;
	}

	return 0;
}
