#pragma once

#include <folly/hash/Checksum.h>
#include <folly/hash/detail/ChecksumDetail.h>
#include <sux/util/Vector.hpp>

namespace zarr {
using ::sux::util::AllocType;
using ::sux::util::Vector;

template <typename T, AllocType AT = MALLOC, size_t C = 1 << 9> class CRC32CFollyHash {
  public:
    using signature_t = uint32_t;

  private:
    const T *string;
    Vector<signature_t, AT> statetable;
    uint32_t state = 0;

  public:
    CRC32CFollyHash() : string(nullptr), statetable(1) { statetable[0] = 0; }

    CRC32CFollyHash(const T *string, size_t size) : string(string) {
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
                hash = crc32c(s + (i - 1) * C, C, hash);
                statetable[i] = hash;
            }
        }

        signature_t hash = statetable[tpos];
        return crc32c(s + tpos * C, length - tpos * C, hash);
    }

    signature_t operator()(size_t from, size_t length) {
        const uint8_t *b = reinterpret_cast<const uint8_t *>(string + from);
        const uint8_t *e = reinterpret_cast<const uint8_t *>(string + from + length);
        signature_t left = (*this)(from);
        signature_t right = (*this)(from + length);
        return crc32c_combine(left, right, e - b);
    }

    signature_t immediate(size_t from, size_t length) const {
        const uint8_t *b = reinterpret_cast<const uint8_t *>(string + from);
        const uint8_t *e = reinterpret_cast<const uint8_t *>(string + from + length);
        return crc32c(b, e - b, 0);
    }

    bool is_hw_supported() { return folly::detail::crc32c_hw_supported(); }

    size_t bitCount() const { return sizeof(*this) * 8 + statetable.bitCount() - sizeof(statetable) * 8; }

  private:
    // NOTE: we force the hardware-accelerated implementation, without runtime tests.
    uint32_t crc32c(const uint8_t *data, size_t nbytes, uint32_t startingChecksum = ~0U) {
#ifdef __SSE4_2__
        return folly::detail::crc32c_hw(data, nbytes, startingChecksum);
#else
        return folly::crc32c(data, nbytes, startingChecksum);
#endif
    }

    // NOTE: we force the hardware-accelerated implementation, without runtime tests.
    uint32_t crc32c_combine(uint32_t crc1, uint32_t crc2, size_t crc2len) {
#ifdef __SSE4_2__
        uint8_t data[4] = {0, 0, 0, 0};
        auto len = crc2len & 3;
        if (crc2len) crc1 = folly::detail::crc32c_hw(data, len, crc1);
        return folly::detail::crc32c_combine_hw(crc1, crc2, crc2len - len);
#else
        return folly::crc32c_combine(crc1, crc2, crc2len);
#endif
    }
};

} // namespace zarr
