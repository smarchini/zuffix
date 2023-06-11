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

	ifstream file(argv[1], ios::in);
	file.seekg(0, file.end);
	size_t filesize = file.tellg();
	file.seekg(0, file.beg);
	assert(filesize % sizeof(SIGMA_T) == 0 && "Bad file size.");

	size_t size = filesize / sizeof(SIGMA_T) + 1;
    sux::util::Vector<SIGMA_T, ALLOC_TYPE> buffer(size);
	file.read((char *)&buffer, filesize);
	buffer[size - 1] = numeric_limits<SIGMA_T>::max();
	auto text = std::span<const SIGMA_T>(&buffer, size);

    SIGMA_T x = 0;
	for (size_t i = 0; i < buffer.size(); i++) x ^= buffer[i] ;
    benchmark::DoNotOptimize(x);

	STRUCTURE_T ds(text);
	cout << STRINGIFY(STRUCTURE_T) << ": " << argv[1] << "\n" << endl;

	for (int i = 2; i < argc; i++) {
		cout << "File: " << argv[i] << endl;
		ifstream file(argv[i], ios::in);

		string header;
		getline(file, header);

		auto get = [&header](string item) {
			auto from = header.find(item) + item.length() + 1;
			auto len = header.substr(from).find(" ");
			return header.substr(from, len);
		};

		auto number = atoi(get("number").c_str());
		auto length = atoi(get("length").c_str());

		sux::util::Vector<SIGMA_T, ALLOC_TYPE> patterns(length * number / sizeof(SIGMA_T));
		file.read((char *)&patterns, length * number);

		for (size_t i = 0; i < patterns.size(); i++) x ^= patterns[i];
		benchmark::DoNotOptimize(x);

		uint64_t sum = 0;
		LInterval<size_t> result;
		vector<chrono::nanoseconds::rep> time;
		time.reserve(number);

		cout << "Run `perf record -g -p " << ::getpid() << "` and press enter" << endl;
		cin.ignore();

		for (size_t i = 0; i < number; i++) {
			auto begin = chrono::high_resolution_clock::now();
			//for (size_t reps = 0; reps < (1 << 7); reps++) {
				span<SIGMA_T> p(&patterns + length * i, length);
				benchmark::DoNotOptimize(p);
				result = ds.FIND_OP(p);
				benchmark::DoNotOptimize(result);
			//}
			auto end = chrono::high_resolution_clock::now();
			sum += result.length();
			time.push_back(chrono::duration_cast<chrono::nanoseconds>(end - begin).count());
		}

		sort(begin(time), end(time));
		auto [med, devmed] = median(time);
		auto [avg, devavg] = mean(time);

		cout << "Total number of occurrences: " << sum << endl;
		cout << "Median time: " << pretty(med) << " ns +/- " << pretty(devmed) << " ns\n";
		cout << "Mean time: " << pretty(avg) << " ns +/- " << pretty(devavg) << " ns\n";
		cout << "Total time: " << accumulate(begin(time), end(time), .0) / 1000000 << " ms" << endl;
	}

	return 0;
}
