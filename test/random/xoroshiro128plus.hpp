#pragma once

#include "../common.hpp"

// NOTE: This is not testing, we are just making sure it works

TEST(xoroshiro128plus, uniform_int_0_100) {
	std::random_device rd;
	zarr::xoroshiro128plus_engine rng(rd());
	std::uniform_int_distribution<int> dist(0, 100);

	for (int i = 0; i < 100; ++i) {
		int x = dist(rng);
		EXPECT_TRUE(0 <= x);
		EXPECT_TRUE(x <= 100);
	}
}

TEST(xoroshiro128plus, uniform_real_0_1) {
	std::random_device rd;
	zarr::xoroshiro128plus_engine rng(rd());
	std::uniform_real_distribution<double> dist(0.0, 1.0);

	for (int i = 0; i < 100; ++i) {
		double x = dist(rng);
		EXPECT_TRUE(0.0 <= x);
		EXPECT_TRUE(x < 1.0);
	}
}

TEST(xoroshiro128plus, uniform_int_50_100) {
	std::random_device rd;
	zarr::xoroshiro128plus_engine rng(rd());
	std::uniform_int_distribution<int> dist(50, 100);

	for (int i = 0; i < 100; ++i) {
		int x = dist(rng);
		EXPECT_TRUE(50 <= x);
		EXPECT_TRUE(x <= 100);
	}
}

TEST(xoroshiro128plus, uniform_real_50_100) {
	std::random_device rd;
	zarr::xoroshiro128plus_engine rng(rd());
	std::uniform_real_distribution<double> dist(50.0, 100.0);

	for (int i = 0; i < 100; ++i) {
		double x = dist(rng);
		EXPECT_TRUE(50.0 <= x);
		EXPECT_TRUE(x < 100.0);
	}
}
