#define DEBUG // Enable statistics (must be defined before common.hpp)

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
	std::cout << name << ": " << result << " found " << result.length() << " occurrences in " << time << " ns" << std::endl;
	for (int i = 0; i < 5; i++) std::cout << ds.getText().substring(ds.getSA()[result.to - i - 1], 10);
	for (int i = 0; i < 5; i++) std::cout << ds.getText().substring(ds.getSA()[result.to - i - 1], 10);
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
	ZuffixArray<char, XXH3Hash> xxh3(file_to_string<char>(argv[1]));
	//ZuffixArray<char, CyclicPoly128Hash> cyclicpoly128(file_to_string<char>(argv[1]));

	while (true) {
		std::string input;
		std::cout << ">>> ";
		std::getline(std::cin, input);
		zarr::String<char> pattern(input);
		run("Simple       ", simple, pattern);
		// run("CyclicPoly128", cyclicpoly128, pattern);
		run("Enhanced     ", enhanced, pattern);
		run("XXH3         ", xxh3, pattern);
		std::cout << std::endl;
	}

	return 0;
}
