#pragma once

#include "../common.hpp"

TEST(saca, sa) {
	using namespace zarr;
	std::string text = "ABRACADABRA";
	text.push_back(std::numeric_limits<char>::max());

	auto sort = SAConstructBySort<char>(text);
	auto sais = SAConstructByGrebnovSAIS<char>(text);

	for (size_t i = 0; i < sort.size(); i++) {
		EXPECT_EQ(sort[i], sais[i]) << "at index " << i;
	}
}

TEST(saca, lcp) {
	using namespace zarr;
	std::string text = "ABRACADABRA";
	text.push_back(std::numeric_limits<char>::max());

	auto sasort = SAConstructBySort<char>(text);
	auto lcpstrcmp = LCPConstructByStrcmp<char>(text, sasort);
	auto lcppsi = LCPConstructByKarkkainenPsi<char>(text, sasort);
	auto lcpsais = LCPConstructByGrebnovSAIS<char>(text, sasort);

	for (size_t i = 0; i < lcpstrcmp.size(); i++) {
		EXPECT_EQ(lcpstrcmp[i], lcppsi[i]) << "at index " << i;
		EXPECT_EQ(lcpstrcmp[i], lcpsais[i]) << "at index " << i;
	}
}
