#include <cstring>
#include <iostream>
#include <random>

int main(int argc, char const *argv[]) {
    std::random_device rd;
    std::mt19937 rng(rd());

    if (argc < 2) {
        std::cerr << "Not enough parameters: <length>\n";
        return -1;
    }

    size_t n = atoi(argv[1]);
    std::string prec = "a", curr = "ab";

    if (n == 0) {
        std::cout << prec;
    } else {
        for (size_t i = 1; i < n; i++) {
            std::string tmp = prec;
            prec = curr;
            curr += tmp;
        }
        std::cout << curr;
    }

    return 0;
}
