#define DEBUG // Enable statistics (must be defined before common.hpp)

#include "common.hpp"
#include <benchmark/benchmark.h>

#define _STRINGIFY(...) #__VA_ARGS__
#define STRINGIFY(...) _STRINGIFY(__VA_ARGS__)

using namespace std;
using namespace zarr;

void print_results(LInterval<size_t> match, const char *text, const size_t *sa, size_t n) {
	if (match.isEmpty()) return;
	if (match.length() <= 5) {
		for (size_t i = 0; i < match.length(); i++) {
			size_t p = sa[match.from + i];
			size_t len = min<size_t>(80, n - p);
			cout.write(text + p, len);
			cout << endl;
		}
	} else {
		for (size_t i = 0; i < 2; i++) {
			size_t p = sa[match.from + i];
			size_t len = min<size_t>(80, n - p);
			cout.write(text + p, len);
			cout << endl;
		}
		cout << "...\n";
		for (size_t i = 0; i < 2; i++) {
			size_t p = sa[match.to - i - 1];
			size_t len = min<size_t>(80ULL, n - p);
			cout.write(text + p, len);
			cout << endl;
		}
	}
}

template <typename T> void run(const char *name, T &ds, const String<char> &p) {
	auto begin = chrono::high_resolution_clock::now();
	benchmark::DoNotOptimize(p);
	auto result = ds.find(p);
	auto end = chrono::high_resolution_clock::now();
	auto time = chrono::duration_cast<chrono::nanoseconds>(end - begin).count();
	cout << name << ": " << result << " found " << result.length() << " occurrences in " << time << " ns" << endl;
	DEBUGDO(print_results(result, &ds.getText(), &ds.getSA(), ds.getText().length()));
	DEBUGDO(ds.print_stats());
	DEBUGDO(ds.reset_stats());
}

int main(int argc, char **argv) {
	if (argc < 2) {
		cerr << "Not enough parameters: \n";
		cerr << argv[0] << " [file.txt]\n";
		return -1;
	}

	SimpleSuffixArray<char> simple(file_to_string<char>(argv[1]));
	EnhancedSuffixArray<char> enhanced(file_to_string<char>(argv[1]));

	ExactZuffixArray<char, XXH3Hash> e_xxh3(file_to_string<char>(argv[1]));
	ProbabilisticZuffixArray<char, XXH3Hash> p_xxh3(file_to_string<char>(argv[1]));

	ExactZuffixArray<char, CRC32CFollyHash> e_crc32cfolly(file_to_string<char>(argv[1]));
	ProbabilisticZuffixArray<char, CRC32CFollyHash> p_crc32cfolly(file_to_string<char>(argv[1]));

	ExactZuffixArray<char, CRC32ZlibHash> e_crc32zlib(file_to_string<char>(argv[1]));
	ProbabilisticZuffixArray<char, CRC32ZlibHash> p_crc32zlib(file_to_string<char>(argv[1]));

	ExactZuffixArray<char, CRC32Plus32CFollyHash> e_crc32crc32c(file_to_string<char>(argv[1]));
	ProbabilisticZuffixArray<char, CRC32Plus32CFollyHash> p_crc32crc32c(file_to_string<char>(argv[1]));

	while (true) {
		string input;
		cout << ">>> ";
		getline(cin, input);
		String<char> pattern(input);
		run("Simple                    ", simple, pattern);
		run("Enhanced                  ", enhanced, pattern);
		run("ExactXXH3                 ", e_xxh3, pattern);
		run("ProbabilisticXXH3         ", p_xxh3, pattern);
		run("ExactCRC32CFolly          ", e_crc32cfolly, pattern);
		run("ProbabilisticCRC32CFolly  ", p_crc32cfolly, pattern);
		run("ExactCRC32Zlib            ", e_crc32zlib, pattern);
		run("ProbabilisticCRC32Zlib    ", p_crc32zlib, pattern);
		run("ExactCRC32+CRC32C         ", e_crc32zlib, pattern);
		run("ProbabilisticCRC32+CRC32C ", e_crc32zlib, pattern);
		cout << endl;
	}

	return 0;
}
