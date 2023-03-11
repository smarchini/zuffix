#pragma once

#include "../common.hpp"

TEST(SimpleSuffixArray, abracadabra) {
    using namespace zarr;
    std::string text = "ABRACADABRA";
    text.push_back(std::numeric_limits<char>::max());
    SimpleSuffixArray<char> simple{text};

    EXPECT_EQ(simple.getSA().size(), 12);
    const size_t sa[12] = {0, 7, 3, 5, 10, 1, 8, 4, 6, 2, 9, 11};
    for (size_t i = 0; i < 12; i++) EXPECT_EQ(simple.getSA()[i], sa[i]) << "at index " << i;

    // NOTE: string literals are null-terminated, they don't exist in the text
    EXPECT_EQ(simple.find(std::string("ABRACADABRA")), LInterval<size_t>(0, 1));
    EXPECT_EQ(simple.find(std::string("ABRACAD")), LInterval<size_t>(0, 1));
    EXPECT_EQ(simple.find(std::string("0")), LInterval<size_t>(1, 0));
    EXPECT_EQ(simple.find(std::string("Z")), LInterval<size_t>(1, 0));
    EXPECT_EQ(simple.find(std::string("ABRACADABR0")), LInterval<size_t>(1, 0));
    EXPECT_EQ(simple.find(std::string("ABRACADABRZ")), LInterval<size_t>(1, 0));

    std::string patterns[] = { "A", "B", "C", "D", "R",
                               "AB", "BR", "CA", "DA", "RA",
                               "ABR", "BRA", "CAD", "DAB",
                               "ABRA", "BRAC", "CADA", "DABR",
                               "ABRAC", "BRACA", "CADAB", "DABRA",
                               "ABRACA", "BRACAD", "CADABR",
                               "ABRACAD", "BRACADA", "CADABRA",
                               "ABRACADA", "BRACADAB",
                               "ABRACADAB", "ABRACADABR", "ABRACADABRA" };

    for (auto p : patterns) {
        auto match = simple.find(p);
        EXPECT_LE(match.from, match.to) << " on pattern: " << p;
        for (size_t i = match.from; i < match.to; i++) EXPECT_EQ(text.substr(sa[i], p.length()), p) << text.substr(sa[i], p.length()) << " != " << p;
    }
}
