#include "common.hpp"

using namespace std;
using namespace zarr;

int main(int argc, char **argv) {
	if (argc < 2) {
		cerr << "Not enough parameters: <text>\n";
		return -1;
	}

	ifstream file(argv[1], ios::in);
	file.seekg(0, file.end);
	size_t filesize = file.tellg();
	file.seekg(0, file.beg);
	assert(filesize % sizeof(SIGMA_T) == 0 && "Bad file size.");

	size_t length = filesize / sizeof(SIGMA_T) + 1;
	sux::util::Vector<SIGMA_T, ALLOC_TYPE> buffer(length);
	file.read((char *)&buffer, filesize);
	buffer[length - 1] = std::numeric_limits<SIGMA_T>::max();

	STRUCTURE_T ds(span<SIGMA_T>(&buffer, length));

	auto begin = chrono::high_resolution_clock::now();
	// ds.dummyDFS();
	auto end = chrono::high_resolution_clock::now();
	cout << chrono::duration_cast<chrono::nanoseconds>(end - begin).count() << " ns" << endl;

	return 0;
}
