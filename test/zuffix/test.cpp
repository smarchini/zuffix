#include <gtest/gtest.h>

#include "SimpleSuffixArray.hpp"

#include "EnhancedSuffixArray.hpp"

#include "EnhancedZuffixArray.hpp"

#include "AllTheSame.hpp"

#include "zuffix.hpp"


int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
