#pragma once

#include <random>
#include <string>

#include <zuffix/EnhancedSuffixArray.hpp>
#include <zuffix/ZuffixArray.hpp>
#include <zuffix/SimpleSuffixArray.hpp>

#include <zuffix/hash/CRC64Hash.hpp>
#include <zuffix/hash/CyclicPolyHash.hpp>
#include <zuffix/hash/O1Hash.hpp>
#include <zuffix/hash/RabinKarpHash.hpp>
#include <zuffix/hash/XXH3Hash.hpp>

#include <zuffix/random/xoroshiro128plus_engine.hpp>

template <typename T> using CyclicPoly128Hash = zarr::CyclicPolyHash<T, 128>;

std::string fibonacci(size_t n) {
	std::string prec = "a", curr = "ab";
	for (size_t i = 0; i < n; i++) {
		std::string tmp = prec;
		prec = curr;
		curr += tmp;
	}
	return curr;
}

std::string random(std::string charset, size_t length) {
	std::string result;
	zarr::xoroshiro128plus_engine rng(0); // fixed seed for reproducibility
	std::uniform_int_distribution<size_t> dist(0, charset.length() - 1);
	result.reserve(length);
	for (size_t i = 0; i < length; i++) result += charset[dist(rng)];
	return result;
}

template <typename T> class BadHash {
  private:
	T *string;
	uint64_t state = 0;
	size_t l = 0, r = 0;

  public:
	BadHash(T *string) : string(string) {}
	BadHash(T *string, size_t length) : string(string) { /* discard length */ }

	uint64_t operator()(size_t to) {
		for (; r < to; r++) state ^= string[r] & 0b11;
		for (; r > to; r--) state ^= string[r - 1] & 0b11;
		return state;
	}

	uint64_t operator()(size_t from, size_t length) {
		for (; l < from; l++) state ^= string[l] & 0b11;
		for (; l > from; l--) state ^= string[l - 1] & 0b11;
		for (; r < from + length; r++) state ^= string[r] & 0b11;
		for (; r > from + length; r--) state ^= string[r - 1] & 0b11;
		return state;
	}

	uint64_t immediate(size_t from, size_t length) const {
		uint64_t result = 0;
		for (size_t i = 0; i < length; i++) result ^= string[from + i] & 0b11;
		return result;
	}
};
