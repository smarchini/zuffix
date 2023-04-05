#pragma once

#include <folly/hash/Checksum.h>
#include <folly/hash/detail/ChecksumDetail.h>
#include <sux/util/Vector.hpp>

namespace zarr {
using ::sux::util::Vector;
using ::sux::util::AllocType;

template <typename T, AllocType AT = MALLOC, size_t C = 1 << 11> class CRC32Plus32CFollyHash {
  public:
	using signature_t = uint64_t;

  private:
    const T *string;
    Vector<signature_t, AT> statetable;
    signature_t state = 0;

  public:
    CRC32Plus32CFollyHash() : string(nullptr), statetable(1) { statetable[0] = 0; }

    CRC32Plus32CFollyHash(const T *string, size_t size) : string(string) {
        statetable.reserve(size / C);
        statetable.pushBack(0);
    }

    void setString(const T *s) {
        string = s;
        statetable.resize(1);
    }

    void reserve(size_t size) { statetable.reserve(size / C); }

    signature_t operator()(size_t to) {
        const uint8_t *s = reinterpret_cast<const uint8_t *>(string);
        const size_t length = to * sizeof(T);

        size_t tpos = length / C;
        if (statetable.size() <= tpos) {
            const size_t last = statetable.size() - 1;
            signature_t hash = statetable[last];
            statetable.resize(tpos + 1);
            for (size_t i = last + 1; i <= tpos; i++) {
                uint32_t low = folly::crc32c(s + (i - 1) * C, C, hash & 0xFFFFFFFF);
                uint64_t hi = folly::crc32(s + (i - 1) * C, C, (hash & 0xFFFFFFFF00000000ULL) >> 32);
                hash = statetable[i] = (hi << 32) | low;
            }
        }

        signature_t hash = statetable[tpos];
        uint32_t low = folly::crc32c(s + tpos * C, length - tpos * C, hash & 0xFFFFFFFF);
        uint64_t hi = folly::crc32(s + tpos * C, length - tpos * C, (hash & 0xFFFFFFFF00000000ULL) >> 32);
        return (hi << 32) | low;
    }

    signature_t operator()(size_t from, size_t length) {
        const uint8_t *b = reinterpret_cast<const uint8_t *>(string + from);
        const uint8_t *e = reinterpret_cast<const uint8_t *>(string + from + length);
        uint64_t left = (*this)(from);
        uint64_t right = (*this)(from + length);
        uint32_t low = folly::crc32c_combine(left& 0xFFFFFFFF, right& 0xFFFFFFFF, e - b);
        uint64_t hi = folly::crc32_combine((left & 0xFFFFFFFF00000000ULL) >> 32, (right & 0xFFFFFFFF00000000ULL) >> 32, e - b);
        return (hi << 32) | low;
    }

    signature_t immediate(size_t from, size_t length) const {
        const uint8_t *b = reinterpret_cast<const uint8_t *>(string + from);
        const uint8_t *e = reinterpret_cast<const uint8_t *>(string + from + length);
        uint32_t low = folly::crc32c(b, e - b, 0);
        uint64_t hi = folly::crc32(b, e - b, 0);
        return (hi << 32) | low;
    }

    bool is_hw_supported() { return folly::detail::crc32_hw_supported() && folly::detail::crc32c_hw_supported(); }

    size_t bitCount() const { return sizeof(*this) * 8 + statetable.bitCount() - sizeof(statetable) * 8; }
};

} // namespace zarr
