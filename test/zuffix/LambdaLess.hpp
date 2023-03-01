#pragma once

#include "../common.hpp"

TEST(LambdaLess, random) {
    using namespace zarr;
    std::string text = random("acgt", 10 * 1024 * 1024);

    ExactZuffixArray<char, XXH3Hash> zuffix{String<char>(text, true)};

    for (size_t m = 10; m < 1024 * 1024; m *= 10) {
        for (int i = 1; i < 10; i++) {
            auto p = String<char>(random("acgt", m));
            EXPECT_EQ(zuffix.fatBinarySearch_lambdaless(p), zuffix.fatBinarySearch(p));
        }
    }
}
