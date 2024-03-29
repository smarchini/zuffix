#include <fstream>
#include <iostream>
#include <memory>
#include <random>

using namespace std;

int main(int argc, char const *argv[]) {
	random_device rd;
    std::mt19937 rng(rd());

	if (argc < 4) {
		cerr << "Not enough parameters: <text file> <bytes per symbol> <symbols>\n";
		return -1;
	}

	std::string filename = argv[1];
	int bytes_per_symbol = atoi(argv[2]);
	int symbols = atoi(argv[3]);

	ifstream text(argv[1], ios::in);

	if (!text) {
		cerr << "Bad input file\n";
		return -1;
	}

	text.seekg(0, text.end);
	int filesize = text.tellg();

	if (filesize % bytes_per_symbol != 0) {
		cerr << "Wrong bytes per symbol: text file can't contain half-symbols\n";
		return -1;
	}

	if (filesize / bytes_per_symbol < symbols) {
		cerr << "Sampling too many symbols: text file not big enough\n";
		return -1;
	}

	uniform_int_distribution<size_t> dist(0, filesize / bytes_per_symbol - symbols);
	text.seekg(dist(rng) * bytes_per_symbol, text.beg);

	size_t size = symbols * bytes_per_symbol;
	std::unique_ptr<char[]> pattern(new char[size]);
	text.read(pattern.get(), size);
	cout.write(pattern.get(), size);

	return 0;
}
