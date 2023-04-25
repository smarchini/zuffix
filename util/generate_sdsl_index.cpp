#include <zuffix/MemcmpZSdsl.hpp>

using namespace std;
using namespace zarr;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " " << "<input text file> <output index file>\n";
        return -1;
    }

	SDSL_STRUCTURE_T ds;
    sdsl::construct(ds, argv[1], sizeof(SIGMA_T));
    sdsl::store_to_file(ds, argv[2]);

    return 0;
}
