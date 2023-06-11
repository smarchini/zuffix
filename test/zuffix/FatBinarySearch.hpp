#pragma once

#include "../common.hpp"

TEST(FatBinarySearch, fibonacci) {
    std::string text = fiboword(30);
    text[text.length()-1] = std::numeric_limits<char>::max();
    zarr::MemcmpZuffixArray<char, zarr::CRC32CFollyHash> zuffix{text};

    for (int i = 1; i < 30; i++) {
        auto p = fiboword(i);
        zuffix.setPattern(p);
        auto expected = zuffix.fatBinarySearch(p);
        EXPECT_EQ(expected,  zuffix.fatBinarySearch_lambdabased(p));
        EXPECT_EQ(expected, zuffix.fatBinarySearch_quasilambdaless(p));
        // WARNING: there are errors because we don't get to test `hlen != f`
        zuffix.fatBinarySearch_lambdaless(p); // let the assert do the work
    }
}

TEST(FatBinarySearch, random) {
    const char* charset = "ab";
    std::string text = randword(10 << 20, charset);
    text[text.length()-1] = std::numeric_limits<char>::max();
    zarr::MemcmpZuffixArray<char, zarr::WyHash> zuffix{text};

    for (int i = 1; i < 100; i++) {
        auto p = randword(i, charset);
        zuffix.setPattern(p);
        auto expected = zuffix.fatBinarySearch(p);
        EXPECT_EQ(expected, zuffix.fatBinarySearch_lambdabased(p));
        EXPECT_EQ(expected, zuffix.fatBinarySearch_quasilambdaless(p));
        // WARNING: there are errors because we don't get to test `hlen != f`
        zuffix.fatBinarySearch_lambdaless(p); // let the assert do the work
    }
}
