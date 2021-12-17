#pragma once

#include <chrono>
#include <fstream>
#include <memory>
#include <random>
#include <sstream>
#include <string>

#include <zuffix/random/xoroshiro128plus_engine.hpp>

#include <zuffix/util/String.hpp>

// #include <zuffix/hash/CRC64Hash.hpp>
#include <zuffix/hash/CyclicPolyHash.hpp>
#include <zuffix/hash/O1Hash.hpp>
#include <zuffix/hash/RabinKarpHash.hpp>
#include <zuffix/hash/XXH3Hash.hpp>
#include <zuffix/hash/functions.hpp>

#include <zuffix/EnhancedSuffixArray.hpp>
#include <zuffix/EnhancedZuffixArray.hpp>
#include <zuffix/SimpleSuffixArray.hpp>

template <typename T> using CyclicPoly4Hash = zarr::CyclicPolyHash<T, 4>;
template <typename T> using CyclicPoly128Hash = zarr::CyclicPolyHash<T, 128>;

zarr::String<uint8_t> random(size_t n, const uint8_t *charset, size_t sigma) {
	zarr::String<uint8_t> result(n);
	static std::random_device rd;
	static zarr::xoroshiro128plus_engine rng(rd());
	std::uniform_int_distribution<uint8_t> dist(0, sigma - 1);
	for (size_t i = 0; i < n; i++) result[i] = charset[dist(rng)];
	return result;
}

zarr::String<char> fibonacci(size_t n, bool dollar = false) {
	std::string prec = "a", curr = "ab";
	for (size_t i = 0; i < n; i++) {
		std::string tmp = prec;
		prec = curr;
		curr += tmp;
	}
	return zarr::String<char>(curr, dollar);
}

template <class T> std::string pretty(T value) {
	std::stringstream ss;
	ss.imbue(std::locale(""));
	ss << std::fixed << value;
	return ss.str();
}

template <typename T, sux::util::AllocType AT = sux::util::MALLOC> std::tuple<std::string, size_t, size_t, std::vector<zarr::String<T, AT>>> load_pizzachili_patterns(std::string filename) {
	std::vector<zarr::String<T, AT>> res;
	std::ifstream file(filename, std::ios::in);

	std::string header;
	std::getline(file, header);

	auto get = [&header](std::string item) {
		auto from = header.find(item) + item.length() + 1;
		auto len = header.substr(from).find(" ");
		return header.substr(from, len);
	};

	auto number = std::atoi(get("number").c_str());
	auto length = std::atoi(get("length").c_str());

	res.reserve(number);
	std::unique_ptr<char[]> buffer(new char[length]);

	for (int i = 0; i < number; i++) {
		file.read(buffer.get(), length);
		res.push_back(zarr::String<T, AT>(buffer.get(), length));
	}

	return std::make_tuple(get("file"), number, length, std::move(res));
}

template <typename T, sux::util::AllocType AT = sux::util::MALLOC> zarr::String<T, AT> file_to_string(std::string filename, bool dollar = false) {
	std::ifstream file(filename, std::ios::in);
	file.seekg(0, file.end);
	int filesize = file.tellg();
	file.seekg(0, file.beg);

	assert(filesize % sizeof(T) == 0 && "Bad size: bytes should be a multiple of sizeof(T)");

	std::unique_ptr<char[]> buffer(new char[filesize]);
	file.read(buffer.get(), filesize);
	return zarr::String<T, AT>(buffer.get(), filesize, dollar);
}

std::tuple<std::chrono::nanoseconds::rep, std::chrono::nanoseconds::rep> median(std::vector<std::chrono::nanoseconds::rep> record) {
	auto n = record.size();
	auto mid = record[n / 2];
	return std::make_tuple(mid, std::max(mid - record[0], record[n - 1] - mid));
}

std::tuple<std::chrono::nanoseconds::rep, std::chrono::nanoseconds::rep> average(std::vector<std::chrono::nanoseconds::rep> record) {
	auto n = record.size();
	auto avg = accumulate(begin(record), end(record), .0) / n;
	return std::make_tuple(avg, avg - record[0]);
}
