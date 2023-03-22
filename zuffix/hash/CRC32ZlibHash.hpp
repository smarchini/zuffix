#pragma once

#include <sux/util/Vector.hpp>
#include <zlib.h>

namespace zarr {
using ::sux::util::Vector;
using ::sux::util::AllocType;

template <typename T, AllocType AT = MALLOC, size_t C = 1 << 13> class CRC32ZlibHash {
  public:
	using signature_t = uint32_t;

  private:
	const T *string;
	Vector<signature_t, AT> statetable;
	signature_t state = 0;

  public:
	CRC32ZlibHash(const T *string) : string(string), statetable(1) { statetable[0] = 0; }

	signature_t operator()(size_t to) {
		const uint8_t *s = reinterpret_cast<const uint8_t *>(string);
		const size_t length = to * sizeof(T);

		size_t tpos = length / C;
		if (statetable.size() <= tpos) {
			const size_t last = statetable.size() - 1;
			signature_t hash = statetable[last];
			statetable.resize(tpos + 1);
			for (size_t i = last + 1; i <= tpos; i++) {
				hash = crc32_z(hash, s + (i - 1) * C, C);
				statetable[i] = hash;
			}
		}

		signature_t hash = statetable[tpos];
		return crc32_z(hash, s + tpos * C, length - tpos * C);
	}

	signature_t operator()(size_t from, size_t length) {
		const uint8_t *b = reinterpret_cast<const uint8_t *>(string + from);
		const uint8_t *e = reinterpret_cast<const uint8_t *>(string + from + length);
		signature_t left = (*this)(from);
		signature_t right = (*this)(from + length);
		return crc32_combine(left, right, e - b);
	}

	signature_t immediate(size_t from, size_t length) const {
		const uint8_t *b = reinterpret_cast<const uint8_t *>(string + from);
		const uint8_t *e = reinterpret_cast<const uint8_t *>(string + from + length);
		return crc32_z(0, b, e - b);
	}

    size_t bitCount() const { return sizeof(*this) * 8 + statetable.bitCount() - sizeof(statetable) * 8; }
};

} // namespace zarr
