#pragma once

#include <algorithm>
#include <cstring>
#include <string>
#include <sux/util/Vector.hpp>

namespace zarr {

template <typename T, sux::util::AllocType AT = sux::util::MALLOC> class String {
public:
  static constexpr char DOLLAR = std::numeric_limits<T>::max();

private:
  sux::util::Vector<T> data;

public:
  explicit String(size_t length, bool prefixfree = false) : data(length + prefixfree) {
    if (prefixfree) data[length] = DOLLAR;
  }

  // prefix free strings are $ terminated, otherwise there is no special (e.g., \0) ending symbols
  explicit String(std::string string, bool prefixfree = false)
      : String(string.length(), prefixfree) {
    for (size_t i = 0; i < string.length(); i++)
      data[i] = string[i];
    if (prefixfree) data[string.length()] = DOLLAR;
  }

  String(const void *buffer, size_t bytes, bool prefixfree = false) : String(bytes, prefixfree) {
    assert(bytes % sizeof(T) == 0 || "Bad size: the string can't be made by half-symbols");
    std::memcpy(&data, buffer, bytes);
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

  friend void swap(String<T, AT> &first, String<T, AT> &second) noexcept {
    std::swap(first.data, second.data);
  }

  /** Returns a pointer at the start of the backing array. */
  inline T *operator&() const { return &data; }

  /** Returns the given element of the string. */
  inline const T &operator[](size_t i) const { return data[i]; };

  /** Returns the given element of the string. */
  inline T &operator[](size_t i) { return data[i]; };

  /** Returns the length of the string. */
  inline size_t length() const { return data.size(); }
};

} // namespace zarr
