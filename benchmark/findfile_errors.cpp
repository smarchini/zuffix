#include "common.hpp"
#include <benchmark/benchmark.h>

using namespace std;
using namespace zarr;

int main(int argc, char **argv) {
    if (argc < 3) {
        cerr << "Not enough parameters: <text> <pattern>\n";
        cerr << "To build text and pattern you may want to use Pizza&Chili utils: ";
        cerr << "http://pizzachili.dcc.uchile.cl/experiments.html \n";
        return -1;
    }

    std::ifstream file(argv[1], std::ios::in);

    // TODO: switch to SimpleSuffixArray (when I'll be sure it's 100% correct)
    EnhancedSuffixArray<SIGMA_T> enhanced(file_to_string<SIGMA_T>(argv[1]));
    size_t err_xxh3 = 0, err_crc32 = 0, err_crc32c = 0, err_crc32pluscrc32c = 0;

    for (int i = 2; i < argc; i++) {
        cout << "Pattern: " << argv[i] << endl;
        auto [file, number, length, p] = load_pizzachili_patterns<SIGMA_T>(argv[i]);

        for (const auto &pattern : p) {
            auto expected = enhanced.find(pattern);

            // TODO not like this

            {
                SignatureZuffixArray<SIGMA_T, XXH3Hash> xxh3(file_to_string<SIGMA_T>(argv[1]));
                err_xxh3 += expected != xxh3.find(pattern);
            }

            {
                SignatureZuffixArray<SIGMA_T, CRC32CFollyHash> crc32c(file_to_string<SIGMA_T>(argv[1]));
                err_crc32c += expected != crc32c.find(pattern);
            }

            {
                SignatureZuffixArray<SIGMA_T, CRC32ZlibHash> crc32(file_to_string<SIGMA_T>(argv[1]));
                err_crc32 += expected != crc32.find(pattern);
            }

            {
                SignatureZuffixArray<SIGMA_T, CRC32Plus32CFollyHash> crc32plus32c(file_to_string<SIGMA_T>(argv[1]));
                err_crc32pluscrc32c += expected != crc32plus32c.find(pattern);
            }
        }

        cout << "Total number of errors in probabilistic data structures: \n";
        cout << "- xxh3: " << err_xxh3 << "\n";
        cout << "- crc32: " << err_crc32 << "\n";
        cout << "- crc32c: " << err_crc32c << "\n";
        cout << "- crc32|crc32c: " << err_crc32pluscrc32c << "\n";
    }

    return 0;
}
