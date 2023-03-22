#define DEBUG // Enable statistics (must be defined before common.hpp)

#include "common.hpp"
#include <benchmark/benchmark.h>

#define _STRINGIFY(...) #__VA_ARGS__
#define STRINGIFY(...) _STRINGIFY(__VA_ARGS__)

using namespace std;
using namespace zarr;

void print_results(std::span<const char> text, const size_t *sa, LInterval<size_t> match) {
	size_t n = text.size();
	constexpr size_t LINE_LENGTH = 80;
	if (match.isEmpty()) return;
	if (match.length() <= 5) {
		for (size_t i = 0; i < match.length(); i++) {
			size_t p = sa[match.from + i];
			size_t len = min<size_t>(LINE_LENGTH, n - p);
			cout.write(text.data() + p, len);
			cout << endl;
		}
	} else {
		for (size_t i = 0; i < 2; i++) {
			size_t p = sa[match.from + i];
			size_t len = min<size_t>(LINE_LENGTH, n - p);
			cout.write(text.data() + p, len);
			cout << endl;
		}
		cout << "...\n";
		for (size_t i = 0; i < 2; i++) {
			size_t p = sa[match.to - i - 1];
			size_t len = min<size_t>(LINE_LENGTH, n - p);
			cout.write(text.data() + p, len);
			cout << endl;
		}
	}
}

int main(int argc, char **argv) {
	if (argc < 2) {
		cerr << "Not enough parameters: \n";
		cerr << argv[0] << " <file.txt>\n";
		return -1;
	}

	std::ifstream file(argv[1]);
	std::stringstream buffer;
	buffer << file.rdbuf() << std::numeric_limits<char>::max();
	std::string text = buffer.str();

	auto begin = chrono::high_resolution_clock::now();
	STRUCTURE_T ds(text);
	auto end = chrono::high_resolution_clock::now();
	auto time = chrono::duration_cast<chrono::nanoseconds>(end - begin).count();

	cout << "Data structure size: " << ds.bitCount() << " bits" << endl;
	cout << "Data structure construction time: " << time << " ns" << endl;

	while (true) {
		string pattern;
		cout << ">>> ";
		getline(cin, pattern);
		LInterval<size_t> result;
		auto begin = chrono::high_resolution_clock::now();
		for (size_t reps = 0; reps < 1000000; reps++) {
			benchmark::DoNotOptimize(pattern);
			result = ds.find(pattern);
			benchmark::DoNotOptimize(result);
		}
		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::nanoseconds>(end - begin).count();
		cout << "Result: " << result << " found " << result.length() << " occurrences in " << time << " ns" << endl;
		DEBUGDO(print_results(ds.getText(), &ds.getSA(), result));
		DEBUGDO(ds.print_stats());
		DEBUGDO(ds.reset_stats());
		cout << endl;
	}

	return 0;
}
