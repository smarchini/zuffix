#include <gtest/gtest.h>

#include "xoroshiro128plus.hpp"

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
