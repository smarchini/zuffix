#pragma once

#include <algorithm>
#include <sux/util/Vector.hpp>
#include <string>

namespace zarr {

template <typename T, sux::util::AllocType AT = sux::util::MALLOC> class String {
public:
  static constexpr char DOLLAR = std::numeric_limits<T>::max();

private:
  sux::util::Vector<T> data;

public:
  explicit String(size_t length) : data(length + 1) {
    data[length] = DOLLAR;
  }

  explicit String(std::string string) : data(string.length() + 1) {
    for (size_t i = 0; i < string.length(); i++)
      data[i] = string[i];
    data[string.length()] = DOLLAR;
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

  /** Returns the number of elements of the underlining vector. */
  inline size_t size() const { return data.size(); }

  /** Returns the length of the string. */
  inline size_t length() const { return data.size() - 1; }
};

} // namespace zarr
