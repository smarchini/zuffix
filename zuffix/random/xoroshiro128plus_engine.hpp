#pragma once

#include <cstdint>
#include <iostream>
#include <limits>

namespace zarr {

class xoroshiro128plus_engine {
  public:
	// types
	typedef std::uint_fast64_t result_type;

	// engine characteristics
	static constexpr std::size_t word_size = 64;
	static constexpr result_type min() { return 0; }
	static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
	static constexpr result_type default_seed[] = {0x333e2c3815b27604, 0x47ed6e7691d8f09f}; // TODO: are these okay?
	static constexpr result_type const_jump[] = {0xdf900294d8f554a5, 0x170865df4b3201fc};
	static constexpr result_type const_long_jump[] = {0xd2a98b26625eee7b, 0xdddf9b1090aa7ac1};

  private:
	// internal state
	result_type s[2];

  public:
	explicit xoroshiro128plus_engine(result_type value = 0) { seed(value); }

	template <class Sseq> explicit xoroshiro128plus_engine(Sseq &q) { seed(q); }

	void seed(result_type value = 0) {
		if (value == 0) {
			s[0] = default_seed[0];
			s[1] = default_seed[1];
		} else {
			// TODO
			s[0] = default_seed[0];
			s[1] = default_seed[1];
		}
	}

	template <class Sseq> void seed(Sseq &q) {
		// TODO I don't even know how it is supposed to work
		s[0] = default_seed[0];
		s[1] = default_seed[1];
	}

	// generating functions
	result_type operator()() {
		const result_type s0 = s[0];
		result_type s1 = s[1];
		const result_type result = s0 + s1;

		s1 ^= s0;
		s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
		s[1] = rotl(s1, 37);                   // c

		return result;
	}

	void discard(unsigned long long z) {
		while (z-- != 0) this->operator()();
	}

	/* It performs discard(2^64) and it can be used to generate 2^64 non-overlapping subsequences
	 * for parallel computations. */
	void jump() {
		result_type s0 = 0, s1 = 0;

		for (std::size_t i = 0; i < sizeof(const_jump) / sizeof(*const_jump); i++)
			for (std::size_t b = 0; b < 64; b++) {
				if (const_jump[i] & UINT64_C(1) << b) {
					s0 ^= s[0];
					s1 ^= s[1];
				}
				this->operator()();
			}

		s[0] = s0;
		s[1] = s1;
	}

	/* It performs discard(2^96) and it can be used to generate 2^32 starting points, from each of
	 * which jump() will generate 2^32 non-overlapping subsequences for parallel distributed
	 * computations. */
	void long_jump() {
		result_type s0 = 0, s1 = 0;

		for (std::size_t i = 0; i < sizeof(const_long_jump) / sizeof(*const_long_jump); i++)
			for (std::size_t b = 0; b < 64; b++) {
				if (const_long_jump[i] & UINT64_C(1) << b) {
					s0 ^= s[0];
					s1 ^= s[1];
				}
				this->operator()();
			}

		s[0] = s0;
		s[1] = s1;
	}

  private:
	static inline result_type rotl(const result_type x, int k) { return (x << k) | (x >> (32 - k)); }

	bool operator==(const xoroshiro128plus_engine &oth) const { return s[0] == oth.s[0] && s[1] == oth.s[1]; }

	bool operator!=(const xoroshiro128plus_engine &oth) const { return s[0] != oth.s[0] || s[1] != oth.s[1]; }

	template <class charT, class traits> friend std::basic_ostream<charT, traits> &operator<<(std::basic_ostream<charT, traits> &os, const xoroshiro128plus_engine &x) {
		os.flags(std::ios_base::dec | std::ios_base::left);
		os.fill(os.widen(' '));
		os << x.s[0] << x.s[1];
		return os;
	}

	template <class charT, class traits> friend std::basic_istream<charT, traits> &operator>>(std::basic_istream<charT, traits> &is, xoroshiro128plus_engine &x) {
		is.flags(std::ios_base::dec | std::ios_base::skipws);
		result_type s0, s1;

		is >> s0;
		if (!is.fail()) x.s[0] = s0;

		is >> s1;
		if (!is.fail()) x.s[0] = s1;
		return is;
	}
};

} // namespace zarr
