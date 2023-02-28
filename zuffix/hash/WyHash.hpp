#pragma once

#include <sux/util/Vector.hpp>
#include <wyhash.h>

namespace zarr {
using ::sux::util::Vector;

template <typename T, size_t C = 48> class WyHash {
  public:
    static_assert(C == 48, "48 is an important parameter in WyHash");

  private:
    //struct WyHashState { uint64_t seed, see1, see2;  };

    const T *string;
    Vector<uint64_t> seedtable, see1table, see2table;

  public:
    WyHash(T *string) : string(string), seedtable(1), see1table(1), see2table(1) { seedtable[0] = see1table[0] = see2table[0] = _wymix(_wyp[0], _wyp[1]); }

    uint64_t operator()(size_t to) {
        const uint8_t *str = reinterpret_cast<const uint8_t *>(string);
        const size_t length = to * sizeof(T);

        if (length <= 16) return wyhash16(str, length, seedtable[0], _wyp);

        const size_t tpos = length / C - (length % C == 0); // WyHash uses < instead of <=, thus
        if (seedtable.size() <= tpos) {
            const size_t last = seedtable.size() - 1;
            uint64_t seed = seedtable[last];
            uint64_t see1 = see1table[last];
            uint64_t see2 = see2table[last];
            seedtable.resize(tpos + 1);
            see1table.resize(tpos + 1);
            see2table.resize(tpos + 1);
            for (size_t i = last + 1; i <= tpos; i++) {
                const uint8_t *p = str + (i - 1) * 48;
                seed = seedtable[i] = _wymix(_wyr8(p) ^ _wyp[1], _wyr8(p + 8) ^ seed);
                see1 = see1table[i] = _wymix(_wyr8(p + 16) ^ _wyp[2], _wyr8(p + 24) ^ see1);
                see2 = see2table[i] = _wymix(_wyr8(p + 32) ^ _wyp[3], _wyr8(p + 40) ^ see2);
            }
        }

        const uint8_t *p = str + tpos * C;
        size_t i = length - tpos * C;
        uint64_t seed = seedtable[tpos] ^ see1table[tpos] ^ see2table[tpos];

        while (_unlikely_(i > 16)) {
            seed = _wymix(_wyr8(p) ^ _wyp[1], _wyr8(p + 8) ^ seed);
            i -= 16;
            p += 16;
        }

        uint64_t a = _wyr8(p + i - 16);
        uint64_t b = _wyr8(p + i - 8);

        a ^= _wyp[1];
        b ^= seed;
        _wymum(&a, &b);
        return _wymix(a ^ _wyp[0] ^ length, b ^ _wyp[1]);
    }

    uint64_t operator()(size_t from, size_t length) { return immediate(from, length); }

    uint64_t immediate(size_t from, size_t length) { return wyhash(string + from, length * sizeof(T), 0, _wyp); }

  private:
    static inline uint64_t stateinit(uint64_t seed, const uint64_t *secret) { return seed ^ _wymix(seed ^ secret[0], secret[1]); }

    static inline uint64_t hashstate(const void *key, size_t len, uint64_t seed, const uint64_t *secret) {
        const uint8_t *p = (const uint8_t *)key;
        if (_unlikely_(len > 48)) {
            uint64_t see1 = seed, see2 = seed;
            do {
                seed = _wymix(_wyr8(p) ^ secret[1], _wyr8(p + 8) ^ seed);
                see1 = _wymix(_wyr8(p + 16) ^ secret[2], _wyr8(p + 24) ^ see1);
                see2 = _wymix(_wyr8(p + 32) ^ secret[3], _wyr8(p + 40) ^ see2);
                p += 48;
                len -= 48;
            } while (_likely_(len > 48));
            seed ^= see1 ^ see2;
        }
        while (_unlikely_(len > 16)) {
            seed = _wymix(_wyr8(p) ^ secret[1], _wyr8(p + 8) ^ seed);
            len -= 16;
            p += 16;
        }
        return seed;
    }

    static inline uint64_t hashfinalize(const void *key, size_t len, uint64_t seed, const uint64_t *secret) {
        const uint8_t *p = (const uint8_t *)key;
        uint64_t a, b;
        size_t i;
        p = p + len - 16;
        i = 16;

        while (_unlikely_(i > 16)) {
            seed = _wymix(_wyr8(p) ^ secret[1], _wyr8(p + 8) ^ seed);
            i -= 16;
            p += 16;
        }

        // finalization
        a = _wyr8(p + i - 16);
        b = _wyr8(p + i - 8);

        a ^= secret[1];
        b ^= seed;
        _wymum(&a, &b);
        return _wymix(a ^ secret[0] ^ len, b ^ secret[1]);
    }

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

    static inline uint64_t realthing(const void *key, size_t len, uint64_t seed, const uint64_t *secret) {
        const uint8_t *p = (const uint8_t *)key;
        seed ^= _wymix(seed ^ secret[0], secret[1]);
        uint64_t a, b;
        if (_likely_(len <= 16)) {
            if (_likely_(len >= 4)) {
                a = (_wyr4(p) << 32) | _wyr4(p + ((len >> 3) << 2));
                b = (_wyr4(p + len - 4) << 32) | _wyr4(p + len - 4 - ((len >> 3) << 2));
            } else if (_likely_(len > 0)) {
                a = _wyr3(p, len);
                b = 0;
            } else
                a = b = 0;
        } else {
            size_t i = len;
            if (_unlikely_(i > 48)) {
                uint64_t see1 = seed, see2 = seed;
                do {
                    seed = _wymix(_wyr8(p) ^ secret[1], _wyr8(p + 8) ^ seed);
                    see1 = _wymix(_wyr8(p + 16) ^ secret[2], _wyr8(p + 24) ^ see1);
                    see2 = _wymix(_wyr8(p + 32) ^ secret[3], _wyr8(p + 40) ^ see2);
                    p += 48;
                    i -= 48;
                } while (_likely_(i > 48));
                std::cout << "real_seed = " << seed << "\n";
                std::cout << "real_see1 = " << see1 << "\n";
                std::cout << "real_see2 = " << see2 << "\n";
                seed ^= see1 ^ see2;
                std::cout << "real_seed_after_xor = " << seed << "\n";
            }
            std::cout << "real i = " << i << ", "
                      << "len = " << len << "\n";
            std::cout << "real p - key = " << p - (const uint8_t *)key << "\n";
            while (_unlikely_(i > 16)) {
                seed = _wymix(_wyr8(p) ^ secret[1], _wyr8(p + 8) ^ seed);
                std::cout << "real_seed_after_unlikely = " << seed << "\n";
                i -= 16;
                p += 16;
            }
            a = _wyr8(p + i - 16);
            b = _wyr8(p + i - 8);
        }
        a ^= secret[1];
        b ^= seed;
        _wymum(&a, &b);
        // return _wymix(a ^ secret[0] ^ len, b ^ secret[1]);
        std::cout << "seed = " << seed << "\n";
        return seed;
    }
};

} // namespace zarr
