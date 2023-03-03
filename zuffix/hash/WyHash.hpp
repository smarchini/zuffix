#pragma once

#include <sux/util/Vector.hpp>
#include <wyhash.h>

namespace zarr {
using ::sux::util::Vector;

template <typename T, size_t C = 48> class WyHash {
  public:
    // TODO: Fix the algorithm: C multiple of 48 should be sufficient
    static_assert(C == 48, "48 is an important parameter in WyHash");

  private:
    struct WyHashState { uint64_t seed, see1, see2; };

    const T *string;
    Vector<WyHashState> statetable;

  public:
    WyHash(T *string) : string(string), statetable(1) {
        uint64_t zero = _wymix(_wyp[0], _wyp[1]);
        statetable[0] = (WyHashState) { zero, zero, zero };
    }

    uint64_t operator()(size_t to) {
        const uint8_t *str = reinterpret_cast<const uint8_t *>(string);
        const size_t length = to * sizeof(T);

        if (length <= 16) return wyhash16(str, length, statetable[0].seed, _wyp);

        const size_t tpos = length / C - (length % C == 0); // off-by-one: wyhash uses < instead of <=
        if (statetable.size() <= tpos) {
            const size_t last = statetable.size() - 1;
            uint64_t seed = statetable[last].seed;
            uint64_t see1 = statetable[last].see1;
            uint64_t see2 = statetable[last].see2;
            statetable.resize(tpos + 1);
            for (size_t i = last + 1; i <= tpos; i++) {
                const uint8_t *p = str + (i - 1) * 48;
                seed = statetable[i].seed = _wymix(_wyr8(p) ^ _wyp[1], _wyr8(p + 8) ^ seed);
                see1 = statetable[i].see1 = _wymix(_wyr8(p + 16) ^ _wyp[2], _wyr8(p + 24) ^ see1);
                see2 = statetable[i].see2 = _wymix(_wyr8(p + 32) ^ _wyp[3], _wyr8(p + 40) ^ see2);
            }
        }

        const uint8_t *p = str + tpos * C;
        size_t i = length - tpos * C;
        uint64_t seed = statetable[tpos].seed ^ statetable[tpos].see1 ^ statetable[tpos].see2;

        while (_unlikely_(i > 16)) {
            seed = _wymix(_wyr8(p) ^ _wyp[1], _wyr8(p + 8) ^ seed);
            i -= 16;
            p += 16;
        }

        uint64_t a = _wyr8(p + i - 16) ^ _wyp[1];
        uint64_t b = _wyr8(p + i - 8) ^ seed;

        _wymum(&a, &b);
        return _wymix(a ^ _wyp[0] ^ length, b ^ _wyp[1]);
    }

    uint64_t operator()(size_t from, size_t length) { return immediate(from, length); }

    uint64_t immediate(size_t from, size_t length) { return wyhash(string + from, length * sizeof(T), 0, _wyp); }

  private:
    static inline uint64_t wyhash16(const void *key, size_t len, uint64_t seed, const uint64_t *secret) {
        const uint8_t *p = (const uint8_t *)key;
        uint64_t a, b;
        if (_likely_(len >= 4)) {
            a = (_wyr4(p) << 32) | _wyr4(p + ((len >> 3) << 2));
            b = (_wyr4(p + len - 4) << 32) | _wyr4(p + len - 4 - ((len >> 3) << 2));
        } else if (_likely_(len > 0)) {
            a = _wyr3(p, len);
            b = 0;
        } else {
            a = b = 0;
        }
        a ^= secret[1];
        b ^= seed;
        _wymum(&a, &b);
        return _wymix(a ^ secret[0] ^ len, b ^ secret[1]);
    }
};

} // namespace zarr
