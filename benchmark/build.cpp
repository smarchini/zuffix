#include "common.hpp"

using namespace std;
using namespace zarr;

int main(int argc, char **argv) {
	if (argc < 2) {
		cerr << "Not enough parameters: <text>\n";
		return -1;
	}

	DATASTRUCTURETYPE ds(file_to_string<SYMBOLTYPE>(argv[1]));
	cout << ds;

#if defined(DEBUG_STATS)
	ds.print_debug_stats();
#endif

	return 0;
}
