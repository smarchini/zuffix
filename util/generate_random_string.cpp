#include <iostream>
#include <random>
#include <cstring>

int main(int argc, char const *argv[]) {
    std::random_device rd;
    std::mt19937 rng(rd());

    if (argc < 2) {
        std::cerr << "Not enough parameters: <length> <sigma>\n";
        return -1;
    }

    size_t length = atoi(argv[1]);
    const char *sigma = argv[2];
    std::uniform_int_distribution<size_t> dist(0, std::strlen(sigma) - 1);
    for (size_t i = 0; i < length; i++) std::cout << sigma[dist(rng)];

    return 0;
}
