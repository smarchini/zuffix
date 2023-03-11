#include <gtest/gtest.h>

#include "../common.hpp"

#include "SimpleSuffixArray.hpp"
#include "EnhancedSuffixArray.hpp"
#include "MemcmpZuffixArray.hpp"
#include "AllTheSame.hpp"
//#include "FatBinarySearch.hpp"

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
