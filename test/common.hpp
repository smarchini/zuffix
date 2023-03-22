#pragma once

#include <random>
#include <string>

#include <zuffix/EnhancedSuffixArray.hpp>
#include <zuffix/MemcmpZuffixArray.hpp>
#include <zuffix/SimpleSuffixArray.hpp>
#include <zuffix/SignatureZuffixArray.hpp>
#include <zuffix/NothingZuffixArray.hpp>

#include <zuffix/hash/WyHash.hpp>
#include <zuffix/hash/CRC32Plus32CFollyHash.hpp>
#include <zuffix/hash/CRC32CFollyHash.hpp>
#include <zuffix/hash/CRC32ZlibHash.hpp>
#include <zuffix/hash/CRC32Hash.hpp>
#include <zuffix/hash/CyclicPolyHash.hpp>
#include <zuffix/hash/O1Hash.hpp>
#include <zuffix/hash/RabinKarpHash.hpp>
#include <zuffix/hash/XXH3Hash.hpp>

#include <zuffix/random/xoroshiro128plus_engine.hpp>

template <typename T, sux::util::AllocType AT> using CyclicPoly128Hash = zarr::CyclicPolyHash<T, 128, AT>;

// TODO: remove me
std::string fibonacci(size_t n) {
	std::string prec = "a", curr = "ab";
	for (size_t i = 0; i < n; i++) {
		std::string tmp = prec;
		prec = curr;
		curr += tmp;
	}
	return curr;
}

// TODO: remove me
std::string random(std::string charset, size_t length) {
	std::string result;
    std::mt19937 rng(2023);  // fixed seed for reproducibility
	std::uniform_int_distribution<size_t> dist(0, charset.length() - 1);
	result.reserve(length);
	for (size_t i = 0; i < length; i++) result += charset[dist(rng)];
	return result;
}

template <typename T, sux::util::AllocType AT> class BadHash {
  public:
	using signature_t = uint64_t;

  private:
	const T *string;
	signature_t state = 0;
	size_t l = 0, r = 0;

  public:
	BadHash(const T *string) : string(string) {}

	signature_t operator()(size_t to) {
		for (; r < to; r++) state ^= string[r] & 0b11;
		for (; r > to; r--) state ^= string[r - 1] & 0b11;
		return state;
	}

	signature_t operator()(size_t from, size_t length) {
		for (; l < from; l++) state ^= string[l] & 0b11;
		for (; l > from; l--) state ^= string[l - 1] & 0b11;
		for (; r < from + length; r++) state ^= string[r] & 0b11;
		for (; r > from + length; r--) state ^= string[r - 1] & 0b11;
		return state;
	}

	signature_t immediate(size_t from, size_t length) const {
		signature_t result = 0;
		for (size_t i = 0; i < length; i++) result ^= string[from + i] & 0b11;
		return result;
	}
};
