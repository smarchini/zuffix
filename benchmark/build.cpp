#include "common.hpp"

using namespace std;
using namespace zarr;

int main(int argc, char **argv) {
	if (argc < 2) {
		cerr << "Not enough parameters: <text>\n";
		return -1;
	}

	STRUCTURE_T ds(file_to_string<SIGMA_T>(argv[1]));
	cout << ds;

	return 0;
}
