#pragma once

#include <cstdint>

namespace zuffix {

	unsigned int murmur_hash_3(unsigned int x) {
		x ^= x >> 16;
		x *= 0x85ebca6b;
		x ^= x >> 13;
		x *= 0xc2b2ae35;
		x ^= x >> 16;
		return x;
	}

}
