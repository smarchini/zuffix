#pragma once

#include <folly/hash/Checksum.h>
#include <folly/hash/detail/ChecksumDetail.h>
#include <sux/util/Vector.hpp>

namespace zarr {
using ::sux::util::Vector;

// https://stackoverflow.com/questions/26429360/crc32-vs-crc32c
//
// The CRC32 found in zip and a lot of other places uses the polynomial
// 0x04C11DB7; its reversed form 0xEDB88320. CRC32C uses a different polynomial
// (0x1EDC6F41, reversed 0x82F63B78) but otherwise the computation is the same.
// The results are different, naturally. This is known as the Castagnoli
// variant. It allows advanced implementations that effectively process one
// 32-bit word per cycle despite the three-cycle latency (by processing 3
// streams of data in parallel and using linear algebra to combine the results).

template <typename T, size_t C = 1 << 14> class CRC32CFollyHash {
  private:
	const T *string;
	Vector<uint64_t> statetable;
	uint32_t state = 0;

  public:
	CRC32CFollyHash(const T *string) : string(string), statetable(1) { statetable[0] = 0; }

	uint64_t operator()(size_t to) {
		const uint8_t *s = reinterpret_cast<const uint8_t *>(string);
		const size_t length = to * sizeof(T);

		size_t tpos = length / C;
		if (statetable.size() <= tpos) {
			const size_t last = statetable.size() - 1;
			uint64_t hash = statetable[last];
			statetable.resize(tpos + 1);
			for (size_t i = last + 1; i <= tpos; i++) {
				hash = folly::crc32c(s + (i - 1) * C, C, hash);
				statetable[i] = hash;
			}
		}

		uint64_t hash = statetable[tpos];
		return folly::crc32c(s + tpos * C, length - tpos * C, hash);
	}

	uint64_t operator()(size_t from, size_t length) {
		const uint8_t *b = reinterpret_cast<const uint8_t *>(string + from);
		const uint8_t *e = reinterpret_cast<const uint8_t *>(string + from + length);
		uint64_t left = (*this)(from);
		uint64_t right = (*this)(from + length);
		return folly::crc32c_combine(left, right, e - b);
	}

	uint64_t immediate(size_t from, size_t length) const {
		const uint8_t *b = reinterpret_cast<const uint8_t *>(string + from);
		const uint8_t *e = reinterpret_cast<const uint8_t *>(string + from + length);
		return folly::crc32c(b, e - b, 0);
	}

	bool is_hw_supported() { return folly::detail::crc32c_hw_supported(); }
};

} // namespace zarr
