#pragma once

#include "util.hpp"

void fixedLength(std::string t, const char *charset, size_t length) {
	zarr::SimpleSuffixArray<char> simple{zarr::String<char>(t, true)};
	zarr::EnhancedSuffixArray<char> enhanced{zarr::String<char>(t, true)};

    for (int i = 0; i < 100; i++) {
      auto p = stdToZarr(randstring(charset, length));
      EXPECT_EQ(simple.find(p), enhanced.find(p));
    }
}

TEST(AllTheSame, rand3) {
  fixedLength("abracadabra", "ab", 3);
}
