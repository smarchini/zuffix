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
	unique_ptr<SIGMA_T[]> text(new SIGMA_T[(filesize + 1) / sizeof(SIGMA_T)]);
	file.read((char *)text.get(), filesize);
	text[filesize] = numeric_limits<SIGMA_T>::max();

	STRUCTURE_T ds(span<SIGMA_T>(text.get(), filesize + 1));
	// cout << ds; // TODO: Do it properly

	return 0;
}
