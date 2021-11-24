#pragma once

#include "../common.hpp"

TEST(saca, sa) {
	using namespace zarr;
	String<char> t(std::string("ABRACADABRA"), true);

	auto sort = SAConstructBySort(t);
	auto divsufsort = SAConstructByDivSufSort(t);
	auto sais = SAConstructByGrebnovSAIS(t);

	for (size_t i = 0; i < sort.size(); i++) {
		EXPECT_EQ(sort[i], divsufsort[i]) << "at index " << i;
		EXPECT_EQ(sort[i], sais[i]) << "at index " << i;
	}
}

// TEST(saca, lcp) {
//   using namespace zarr;
// 	 String<char> t(std::string("ABRACADABRA"), true);
// 	 auto sasort = SAConstructBySort(t);
// 	 auto lcpstrcmp = LCPConstructByStrcmp(t, sasort);
//
//	 Vector<size_t> sa;
//	 Vector<ssize_t> lcp;
//	 SALCPConstructByDivSufSort(sa, lcp, t);
//
//	 for (size_t i = 0; i < sasort.size(); i++) EXPECT_EQ(sasort[i], sa[i]) << "at index " << i;
//	 for (size_t i = 0; i < lcpstrcmp.size(); i++) EXPECT_EQ(lcpstrcmp[i], lcp[i]) << "at index " << i;
// }

TEST(saca, lcp) {
	using namespace zarr;
	String<char> t(std::string("ABRACADABRA"), true);
	auto sasort = SAConstructBySort(t);
	auto lcpstrcmp = LCPConstructByStrcmp(t, sasort);
	auto lcp = LCPConstructionByKarkkainenPsi(t, sasort);
	for (size_t i = 0; i < lcpstrcmp.size(); i++) EXPECT_EQ(lcpstrcmp[i], lcp[i]) << "at index " << i;
}
