#pragma once

#include <string>
#include <sux/util/Vector.hpp>

template <typename T> sux::util::Vector<T> arrayToVector(const T *array, size_t length) {
	sux::util::Vector<T> result(length);
	for (size_t i = 0; i < length; i++) result[i] = array[i];
	return result;
}

sux::util::Vector<char> stringToVector(std::string string) {
	size_t length = string.length();
	sux::util::Vector<char> result(length);
	for (size_t i = 0; i < length; i++) result[i] = string[i];
	return result;
}
