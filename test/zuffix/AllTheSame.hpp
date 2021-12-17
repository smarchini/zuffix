#pragma once

#include "../common.hpp"

TEST(AllTheSame, fibonacci) {
	using namespace zarr;
	std::string text = fibonacci(10);

	std::cerr << "Simple" << std::endl;
	SimpleSuffixArray<char> simple{String<char>(text, true)};
	std::cerr << "Enhanced" << std::endl;
	EnhancedSuffixArray<char> enhanced{String<char>(text, true)};
	std::cerr << "RabinKarp" << std::endl;
	EnhancedZuffixArray<char, RabinKarpHash> rabinkarp{String<char>(text, true)};
	std::cerr << "CyclicPoly128" << std::endl;
	EnhancedZuffixArray<char, CyclicPoly128Hash> cyclicpoly{String<char>(text, true)};
	std::cerr << "O1" << std::endl;
	EnhancedZuffixArray<char, O1Hash> o1{String<char>(text, true)};
	std::cerr << "XXH3" << std::endl;
	EnhancedZuffixArray<char, XXH3Hash> xxh3{String<char>(text, true)};
	// std::cerr << "CRC64" << std::endl;
	// EnhancedZuffixArray<char, CRC64Hash> crc64{String<char>(text, true)};

	std::cerr << "BadHash" << std::endl;
	EnhancedZuffixArray<char, BadHash> bad{String<char>(text, true)};

	for (int i = 2; i < 10; i++) {
		String<char> p = String<char>(fibonacci(i));
		std::cerr << "Simple" << std::endl;
		auto x = simple.find(p);
		std::cerr << "Enhanced" << std::endl;
		EXPECT_EQ(x, enhanced.find(p));
		std::cerr << "RabinKarp" << std::endl;
		EXPECT_EQ(x, rabinkarp.find(p));
		std::cerr << "CyclicPoly128" << std::endl;
		EXPECT_EQ(x, cyclicpoly.find(p));
		std::cerr << "O1" << std::endl;
		EXPECT_EQ(x, o1.find(p));
		std::cerr << "XXH3" << std::endl;
		EXPECT_EQ(x, xxh3.find(p));
		// std::cerr << "CRC64" << std::endl;
		// EXPECT_EQ(x, crc64.find(p));
		std::cerr << "BadHash" << std::endl;
		EXPECT_EQ(x, bad.find(p));
	}
}

inline void fixlen(std::string charset, size_t n, size_t m) {
	using namespace zarr;
	std::string text = random(charset, n);

	std::cerr << "Simple" << std::endl;
	SimpleSuffixArray<char> simple{String<char>(text, true)};
	std::cerr << "Enhanced" << std::endl;
	EnhancedSuffixArray<char> enhanced{String<char>(text, true)};
	std::cerr << "RabinKarp" << std::endl;
	EnhancedZuffixArray<char, RabinKarpHash> rabinkarp{String<char>(text, true)};
	std::cerr << "CyclicPoly128" << std::endl;
	EnhancedZuffixArray<char, CyclicPoly128Hash> cyclicpoly128{String<char>(text, true)};
	std::cerr << "O1" << std::endl;
	EnhancedZuffixArray<char, O1Hash> o1{String<char>(text, true)};
	std::cerr << "XXH3" << std::endl;
	EnhancedZuffixArray<char, XXH3Hash> xxh3{String<char>(text, true)};
	// std::cerr << "CRC64" << std::endl;
	// EnhancedZuffixArray<char, CRC64Hash> crc64{String<char>(text, true)};

	std::cerr << "BadHash" << std::endl;
	EnhancedZuffixArray<char, BadHash> bad{String<char>(text, true)};

	for (int i = 0; i < 100; i++) {
		auto p = String<char>(random(charset, m));
		std::cerr << "Simple" << std::endl;
		auto x = simple.find(p);
		std::cerr << "Enhanced" << std::endl;
		EXPECT_EQ(x, enhanced.find(p));
		std::cerr << "RabinKarp" << std::endl;
		EXPECT_EQ(x, rabinkarp.find(p));
		std::cerr << "CyclicPoly128" << std::endl;
		EXPECT_EQ(x, cyclicpoly128.find(p));
		std::cerr << "O1" << std::endl;
		EXPECT_EQ(x, o1.find(p));
		std::cerr << "XXH3" << std::endl;
		EXPECT_EQ(x, xxh3.find(p));
		// std::cerr << "CRC64" << std::endl;
		// EXPECT_EQ(x, crc64.find(p));
		std::cerr << "BadHash" << std::endl;
		EXPECT_EQ(x, bad.find(p));
	}
}

inline void battery(std::string charset) {
	for (size_t n = 10; n < 10000; n *= 10) {
		for (size_t m = 1; m < n; m *= 10) {
			fixlen(charset, n, m);
		}
	}
}

TEST(AllTheSame, ab) { battery("ab"); }
TEST(AllTheSame, ACGT) { battery("ACGT"); }
TEST(AllTheSame, az) { battery("abcdefghijklmnopqrstuvwxyz"); }
TEST(AllTheSame, azAZ09) { battery("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"); }
