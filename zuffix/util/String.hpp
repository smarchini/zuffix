#pragma once

#include <algorithm>
#include <cstring>
#include <fstream>
#include <string>
#include <sux/util/Vector.hpp>

namespace zarr {

template <typename T, sux::util::AllocType AT = sux::util::MALLOC> class String {
  public:
	// static constexpr char DOLLAR = std::numeric_limits<T>::max();
	static constexpr char DOLLAR = '~';

  private:
	sux::util::Vector<T, AT> data;

  public:
	String() {}

	explicit String(size_t length, bool dollar = false) : data(length + dollar) {
		if (dollar) data[data.size() - 1] = DOLLAR;
	}

	// Prefix-free strings are $ terminated, otherwise there is no ending delimiter
	explicit String(std::string string, bool dollar = false) : String(string.length(), dollar) {
		for (size_t i = 0; i < string.length(); i++) data[i] = string[i];
		if (dollar) data[data.size() - 1] = DOLLAR;
	}

	explicit String(const void *buffer, size_t bytes, bool dollar = false) : String(bytes, dollar) {
		assert(bytes % sizeof(T) == 0 && "Bad size: bytes should be a multiple of sizeof(T)");
		std::memcpy(&data, buffer, bytes);
	}

	String<T> substring(size_t from, size_t length) {
		String<T> result(length);
		std::memcpy(&result.data, &data + from, length * sizeof(T));
		return result;
	}

	// Delete copy operators
	String(const String &) = delete;
	String &operator=(const String &) = delete;

	// Define move operators
	String(String<T, AT> &&oth) : data(std::exchange(oth.data, sux::util::Vector<T>())) {}

	String<T, AT> &operator=(String<T, AT> &&oth) {
		swap(*this, oth);
		return *this;
	}

	friend void swap(String<T, AT> &first, String<T, AT> &second) noexcept { std::swap(first.data, second.data); }

	/** Returns a pointer at the start of the backing array. */
	inline T *operator&() const { return &data; }

	/** Returns the given element of the string. */
	inline const T &operator[](size_t i) const { return data[i]; };

	/** Returns the given element of the string. */
	inline T &operator[](size_t i) { return data[i]; };

	/** Returns the length of the string. */
	inline size_t length() const { return data.size(); }

  private:
	friend std::ostream &operator<<(std::ostream &os, const String<T, AT> &s) { return os << s.data; }
	friend std::istream &operator>>(std::istream &is, String<T, AT> &s) { return is >> s.data; }
};

} // namespace zarr
