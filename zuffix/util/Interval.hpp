#pragma once

#include <iostream>
#include <utility>

namespace zarr {

/** Immutable pair of values to represent an interval [from, to].
 *
 * @tparam T the data type of the interval.
 */
template <class T> class Interval {
public:
  const T from, to;

  explicit Interval(T point) : Interval(point, point) {}
  Interval(T from, T to) : from(std::move(from)), to(std::move(to)) {}

  /** Length of the interval */
  T length() const { return to - from + 1; }

  /** Empty interval
   *
   * @return an interval
   */
  static Interval<T> empty() { return Interval(1, 0); }

  /** Check if the interval is empty
   *
   * @return true if the interval is empty, false otherwise
   */
  bool isEmpty() const { return to < from; }

  /** Check if the interval is a point interval
   *
   * @return true if the interval is in a single point, false otherwise
   */
  bool isPoint() const { return to == from; }

  /** Check if two interval are equals. Two empty intervals are always equals.
   *
   * @return true if the other interval is equals to this, false otherwise.
   */
  bool operator==(const Interval<T>& oth) const {
    if (isEmpty() && oth.isEmpty()) return true;
    return from == oth.from && to == oth.to;
  }

  /** Check if two interval are different.
   *
   * @return false if the other interval is equals to this, true otherwise.
   */
  bool operator!=(const Interval<T>& oth) const {
    return !(this == oth);
  }

  /** Serialize the interval in a human-readable format */
  friend std::ostream &operator<<(std::ostream &os, const Interval<T> &interval) {
    if (interval.isEmpty()) return os << "[]";
    if (interval.isPoint()) return os << "[" << interval.from << "]";
    return os << "[" << interval.from << ", " << interval.to << "]";
  }
};

} // namespace zarr
