#pragma once

#include "../common.hpp"

TEST(AllTheSame, fibonacci) {
	using namespace zarr;
	std::string text = fiboword(10);
	text[text.length() - 1] = std::numeric_limits<char>::max();

	SimpleSuffixArray<char> simple{text};
	EnhancedSuffixArray<char> enhanced{text};

	MemcmpZuffixArray<char, WyHash> wyhash{text};
	MemcmpZuffixArray<char, XXH3Hash> xxh3{text};
	MemcmpZuffixArray<char, CRC32ZlibHash> crc32{text};
	MemcmpZuffixArray<char, CRC32CFollyHash> crc32c{text};
	MemcmpZuffixArray<char, CRC32Plus32CFollyHash> crc32crc32c{text};

	MemcmpZuffixArray<char, RabinKarpHash> rabinkarp{text};
	MemcmpZuffixArray<char, CyclicPoly128Hash> cyclicpoly{text};
	MemcmpZuffixArray<char, O1Hash> o1{text};

	MemcmpZuffixArray<char, BadHash> bad{text};

	// Testing signature & nothing vairants with 64 bits hash functions should be fine
	SignatureZuffixArray<char, WyHash> pwyhash{text};
	NothingZuffixArray<char, WyHash> nwyhash{text};

	for (int i = 2; i < 10; i++) {
		std::string p = fiboword(i);
		auto expected = simple.find(p);

		EXPECT_EQ(expected, enhanced.find(p));

		EXPECT_EQ(expected, wyhash.find(p));
		EXPECT_EQ(expected, xxh3.find(p));
		EXPECT_EQ(expected, crc32.find(p));
		EXPECT_EQ(expected, crc32c.find(p));
		EXPECT_EQ(expected, crc32crc32c.find(p));

		EXPECT_EQ(expected, rabinkarp.find(p));
		EXPECT_EQ(expected, cyclicpoly.find(p));
		EXPECT_EQ(expected, o1.find(p));

		EXPECT_EQ(expected, bad.find(p));

		EXPECT_EQ(expected, pwyhash.find(p));
		EXPECT_EQ(expected, nwyhash.find(p));
	}
}

inline void fixlen(size_t n, size_t m, std::string charset) {
	using namespace zarr;
	std::string text = randword(n, charset);
	text[text.length() - 1] = std::numeric_limits<char>::max();

	SimpleSuffixArray<char> simple{text};
	EnhancedSuffixArray<char> enhanced{text};

	MemcmpZuffixArray<char, WyHash> wyhash{text};
	MemcmpZuffixArray<char, XXH3Hash> xxh3{text};
	MemcmpZuffixArray<char, CRC32ZlibHash> crc32{text};
	MemcmpZuffixArray<char, CRC32CFollyHash> crc32c{text};
	MemcmpZuffixArray<char, CRC32Plus32CFollyHash> crc32crc32c{text};

	MemcmpZuffixArray<char, RabinKarpHash> rabinkarp{text};
	MemcmpZuffixArray<char, CyclicPoly128Hash> cyclicpoly{text};
	MemcmpZuffixArray<char, O1Hash> o1{text};

	MemcmpZuffixArray<char, BadHash> bad{text};

	// Testing signature & nothing vairants with 128 bits hash functions should be fine
	SignatureZuffixArray<char, XXH3Hash> pxxh3{text};
	NothingZuffixArray<char, XXH3Hash> nxxh3{text};

	for (int i = 0; i < 100; i++) {
		auto p = randword(m, charset);
		auto expected = simple.find(p);

		EXPECT_EQ(expected, enhanced.find(p));

		EXPECT_EQ(expected, wyhash.find(p));
		EXPECT_EQ(expected, xxh3.find(p));
		EXPECT_EQ(expected, crc32.find(p));
		EXPECT_EQ(expected, crc32c.find(p));
		EXPECT_EQ(expected, crc32crc32c.find(p));

		EXPECT_EQ(expected, rabinkarp.find(p));
		EXPECT_EQ(expected, cyclicpoly.find(p));
		EXPECT_EQ(expected, o1.find(p));

		EXPECT_EQ(expected, bad.find(p));

		EXPECT_EQ(expected, pxxh3.find(p));
		EXPECT_EQ(expected, nxxh3.find(p));
	}
}

inline void battery(std::string charset) {
	for (size_t n = 1000; n < 100000; n *= 10) {
		for (size_t m = 1; m < n; m *= 10) {
			fixlen(n, m, charset);
		}
	}
}

TEST(AllTheSame, ab) { battery("ab"); }
TEST(AllTheSame, ACGT) { battery("ACGT"); }
TEST(AllTheSame, az) { battery("abcdefghijklmnopqrstuvwxyz"); }
TEST(AllTheSame, azAZ09) { battery("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"); }
