// #define DEBUG // Enable statistics (must be defined before common.hpp)

#include "common.hpp"
#include <unistd.h>

#define _STRINGIFY(...) #__VA_ARGS__
#define STRINGIFY(...) _STRINGIFY(__VA_ARGS__)

using namespace std;
using namespace zarr;

template <typename T> void run(const char *name, T &ds, const String<char> &p) {
	auto begin = chrono::high_resolution_clock::now();
	auto result = ds.find(p);
	auto end = chrono::high_resolution_clock::now();
	auto time = chrono::duration_cast<chrono::nanoseconds>(end - begin).count();
	cout << name << ": " << result << " found " << result.length() << " occurrences in " << time << " ns" << endl;
	if (!result.isEmpty()) {
		auto m = min(result.length() / 2, 3UL);
		for (int i = 0; i < m; i++) {
			cout.write(&ds.getText() + ds.getSA()[result.from + i], 50);
			cout << "\n";
		}
		cout << "\t...\n";
		for (int i = 0; i < m; i++) {
			cout.write(&ds.getText() + ds.getSA()[result.to - i - 1], 50);
			cout << "\n";
		}
	}
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
	ZuffixArray<char, CRC32ZlibHash> crc32zlib(file_to_string<char>(argv[1]));
	EnhancedSuffixArray<char> enhanced(file_to_string<char>(argv[1]));
	ZuffixArray<char, XXH3Hash> xxh3(file_to_string<char>(argv[1]));

	while (true) {
		string input;
		cout << ">>> ";
		getline(cin, input);
		String<char> pattern(input);
		run("Simple    ", simple, pattern);
		run("CRC32Zlib ", crc32zlib, pattern);
		run("Enhanced  ", enhanced, pattern);
		run("XXH3      ", xxh3, pattern);
		cout << endl;
	}

	return 0;
}
