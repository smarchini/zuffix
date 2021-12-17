#include "common.hpp"
#include <unistd.h>

#define _STRINGIFY(...) #__VA_ARGS__
#define STRINGIFY(...) _STRINGIFY(__VA_ARGS__)

using namespace std;
using namespace zarr;

tuple<size_t, chrono::nanoseconds::rep> find(DATASTRUCTURETYPE &ds, const String<SYMBOLTYPE> &p) {
	size_t reps = 1;
	uint64_t u = 0;
	size_t result;

	vector<chrono::nanoseconds::rep> time;
	time.reserve(reps);

	for (uint64_t i = 0; i < reps; i++) {
		auto begin = chrono::high_resolution_clock::now();
		u ^= ds.find(p).length();
		auto end = chrono::high_resolution_clock::now();
		time.push_back(chrono::duration_cast<chrono::nanoseconds>(end - begin).count());
		if (i == 0) result = u;
	}

	const volatile uint64_t __attribute__((unused)) unused = u;

	sort(time.begin(), time.end());
	return make_tuple(result, time[reps / 2]);
}

int main(int argc, char **argv) {
	if (argc < 3) {
		cerr << "Not enough parameters: <text.index> <pattern>\n";
		cerr << "To build text and pattern you may want to use Pizza&Chili utils: ";
		cerr << "http://pizzachili.dcc.uchile.cl/experiments.html \n";
		return -1;
	}

	std::ifstream file(argv[1], std::ios::in);
	DATASTRUCTURETYPE ds;
	file >> ds;
	cout << STRINGIFY(DATASTRUCTURETYPE) << ": " << argv[1] << "\n" << endl;

	for (int i = 2; i < argc; i++) {
		cout << "Pattern: " << argv[i] << endl;
		auto [file, number, length, p] = load_pizzachili_patterns<SYMBOLTYPE>(argv[i]);

		uint64_t sum = 0;
		vector<chrono::nanoseconds::rep> record;
		record.reserve(number);

		cout << "Run `perf record -g -p " << ::getpid() << "` and press enter" << std::endl;
		cin.ignore();

		for (const auto &pattern : p) {
			auto [count, time] = find(ds, pattern);
			sum += count;
			record.push_back(time);
			cout << "\tlength: " << pattern.length() << ", count: " << count << ", time: " << pretty(time) << " ns" << endl;
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
