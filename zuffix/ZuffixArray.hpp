#pragma once

#include "hash/Spooky.h"
#include "util/Interval.hpp"

#include <limits>
#include <stack>
#include <sux/support/common.hpp>
#include <sux/util/Vector.hpp>
#include <unordered_map>

namespace zarr {
using ::sux::util::Vector;

template <typename T> class ZuffixArray {

public:
  static constexpr char DOLLAR = std::numeric_limits<T>::max();

private:
  const Vector<T> String;
  const Vector<size_t> Suffix, LCP;
  Vector<size_t> Up, Down, Next;
  std::unordered_map<size_t, size_t> ZMap;

public:
  explicit ZuffixArray(Vector<T> string)
      : String(std::move(string)), Suffix(buildSuffixArray(String)), LCP(buildLCP(String, Suffix)),
        Up(String.size() + 1), Down(String.size() + 1), Next(String.size() + 1) {
    std::stack<size_t> stack;
    stack.push(0);

    size_t last = -1ULL;
    for (size_t i = 1; i <= String.size(); i++) {
      while (LCP[i] < LCP[stack.top()]) {
        last = stack.top();
        stack.pop();
        size_t top = stack.top();

        if (LCP[i] <= LCP[top] && LCP[top] != LCP[last]) Down[top] = last;
      }

      if (last != -1ULL) {
        Up[i] = last;
        last = -1ULL;
      }

      stack.push(i);
    }

    stack = std::stack<size_t>();
    stack.push(0);

    for (size_t i = 1; i <= String.size(); i++) {
      while (LCP[i] < LCP[stack.top()])
        stack.pop();

      if (LCP[i] == LCP[stack.top()]) {
        last = stack.top();
        stack.pop();
        Next[last] = i;
      }

      stack.push(i);
    }

    buildZMap(0, String.size(), 0);
  }

  Vector<size_t> getChildren(size_t i, size_t j) const {
    Vector<size_t> res;
    res.pushBack(i);

    if (i == 0 && j == Up.size() - 1) {
      int k = 0;
      while (Next[k] != 0) {
        res.pushBack(Next[k]);
        k = Next[k];
      }
    } else {
      size_t k = Up[j + 1];
      if (k <= i || k > j) k = Down[i];
      res.pushBack(k);
      while (Next[k] != 0) {
        res.pushBack(Next[k]);
        k = Next[k];
      }
    }

    // if (i == 0 && j == Up.size() - 1) {
    //   for (size_t k = Next[0]; k != 0; k = Next[k])
    //     res.pushBack(Next[k]);
    // } else {
    //   for (size_t k = Up[j + 1] <= i || Up[j + 1] > j ? Up[j + 1] : Down[i]; k != 0; k = Next[k])
    //     res.pushBack(Next[k]);
    // }

    res.trimToFit();
    return res;
  }

  size_t extentLength(size_t node) const { return extentLength(node >> 32, node); }
  size_t extentLength(uint32_t i, uint32_t j) const {
    if (i == j) return String.size() - Suffix[i];

    if (i == 0 && j == Up.size() - 1) return LCP[Next[0]];

    size_t k = Up[j + 1];
    if (j < k || k <= i) k = Down[i];

    return LCP[k];
  }

  void visitPre(size_t i, size_t j, size_t name_len, size_t d) {
    using namespace std;
    cout << string(d, '\t') << "[" << i << ", " << j << "] ";

    assert((i == 0 && j == String.size() && name_len == 0) ||
           ((i != 0 || j != String.size()) && name_len == 1 + max(LCP[i], LCP[j + 1])));

    if (i == j) {
      size_t extent_len = String.size() + 1 - Suffix[i];
      size_t handle_len = twoFattest(name_len - 1, extent_len);
      cout << " skip: "
           << "[" << name_len << ", " << extent_len << "] ";

      if (name_len != extent_len) {
        cout << "(" << handle_len << ") ";

        for (size_t k = Suffix[i]; k < Suffix[i] + handle_len; k++)
          cout << String[k];
      }

      cout << endl;
    } else {
      auto children = getChildren(i, j);
      size_t extent_len = LCP[children[1]];
      size_t handle_len = twoFattest(name_len - 1, extent_len);

      cout << " skip: "
           << "[" << name_len << ", " << extent_len << "] "
           << "(" << handle_len << ") ";

      for (size_t k = Suffix[i]; k < Suffix[i] + handle_len; k++)
        cout << String[k];

      cout << endl;

      for (size_t p = 0; p < children.size() - 1; p++)
        visitPre(children[p], children[p + 1] - 1, extent_len + 1, d + 1);

      visitPre(children[children.size() - 1], j, extent_len + 1, d + 1);
    }
  }

  size_t fatBinarySearch(const Vector<T> &v, int64_t from, int64_t to) const {
#ifdef DEBUG
    std::cout << "fatBinarySearch([ ";
    for (size_t i = 0; i < v.size(); i++)
      std::cout << v[i] << " ";
    std::cout << "], (" << from << ".." << to << "))" << std::endl;
#endif
    to--;

    assert(from < to && "from < to");

    size_t top = -1ULL;
    if (from == -1) {
      // ALERT: this should return NULL in certain cases
      top = String.size(); // root
      from = 0;            // extent length
    }

    int64_t check_mask = -1ULL << sux::ceil_log2(to - from);

    while (to - from > 0) {
      assert(check_mask != 0);

#ifdef DEBUG
      std::cout << "(" << from << ".." << (to + 1) << ")" << std::endl;
#endif

      int64_t f = to & check_mask;

      if ((from & check_mask) != f) {
        auto it = ZMap.find(hash(v, 0, f));
        size_t node = it == std::end(ZMap) ? -1ULL : it->second;
        size_t g;

        if (node == -1ULL || (g = extentLength(node)) < f) {
#ifdef DEBUG
          std::cout << "Missing" << std::endl;
#endif
          to = f - 1;
        } else {
#ifdef DEBUG
          std::cout << "Found extent of length " << g << std::endl;
#endif
          top = node;
          from = g;
        }
      }

      check_mask >>= 1;
    }

#ifdef DEBUG
    std::cout << "Final interval: (" << from << ".." << (to + 1) << "); top: ["
              << interval(top).first << ", " << interval(top).second << "]";
#endif
    return top;
  }

  Interval<size_t> findExact(const Vector<T> &v) const {
    size_t left = 0;
    size_t right = String.size();
    size_t m = v.size();

    for (size_t i = 0;; i++) {
      size_t extent_len = extentLength(left, right);
      size_t limit = std::min(m, extent_len);

      for (; i < limit; i++) {
        if (v[i] != String[Suffix[left] + i]) break;
      }

      if (i == m) return Interval(left, right);
      if (i < extent_len) return Interval<size_t>::empty();

      assert(i == extent_len && "i != extent_len");
      assert(m > extent_len && "m > extent_len");

      if (left == right) return Interval<size_t>::empty();

      auto children = getChildren(left, right);

      size_t c;
      for (c = 0; c < children.size(); c++) {
        if (c == children.size() - 1 && children[c] == right &&
            extent_len + 1 == String.size() + 1 - Suffix[children[c]]) {
#ifdef DEBUG
          std::cout << "Skipping fake child: (" << right << ", " << right << ")" << std::endl;
#endif
          continue;
        }

        if (String[Suffix[children[c]] + extent_len] == v[i]) break;
      }

      if (c == children.size()) return Interval<size_t>::empty();

      left = children[c];
      if (c != children.size() - 1) right = children[c + 1] - 1;
    }
  }

  Interval<size_t> find(const Vector<T> &v) const {
    size_t m = v.size();
    size_t pos = fatBinarySearch(v, -1ULL, m);
    size_t left = pos >> 32;
    size_t right = pos & 0xFFFFFFFF;

    size_t extent_len = extentLength(left, right);
    size_t name_len = pos == String.size() ? 0 : 1 + std::max(LCP[left], LCP[right + 1]);

    size_t limit = std::min(m, extent_len);

    size_t i;
    for (i = 0; i < limit; i++) {
      if (v[i] != String[Suffix[left] + i]) break;
    }

    if (i < name_len) return findExact(v);

    if (i == m) return Interval(left, right);
    if (i < extent_len) return Interval<size_t>::empty();

    auto children = getChildren(left, right);

    size_t c;
    for (c = 0; c < children.size(); c++) {
      if (c == children.size() - 1 && children[c] == right &&
          extent_len + 1 == String.size() + 1 - Suffix[children[c]]) {
#ifdef DEBUG
        std::cout << "Skipping fake child: (" << right << ", " << right << ")" << std::endl;
#endif
        continue;
      }

      if (String[Suffix[children[c]] + extent_len] == v[i]) break;
    }

    if (c == children.size()) return Interval<size_t>::empty();

    size_t child_left = children[c];
    size_t child_right = c == children.size() - 1 ? right : children[c + 1] - 1;
    size_t start = Suffix[child_left];
    size_t child_extent_lenght = extentLength(child_left, child_right);

    size_t child_limit = std::min(m, child_extent_lenght);
    for (i++; i < child_limit; i++) {
      if (v[i] != String[start + i]) break;
    }

    if (i == m) return Interval(child_left, child_right);
    if (i < m) return Interval<size_t>::empty();

    assert(i == child_extent_lenght && "i == child_extent_lenght");
    assert(m > child_extent_lenght && "m > child_extent_lenght");
    return findExact(v);
  }

  const T *getString() const { return &String; }

  const size_t *getSuffix() const { return &Suffix; }

  const size_t *getLCP() const { return &LCP; }

private:
  static Vector<size_t> buildSuffixArray(const Vector<T> &string) {
    Vector<size_t> suffix(string.size() + 1);

    for (size_t i = 0; i < suffix.size(); i++)
      suffix[i] = i;

    std::sort(&suffix, &suffix + suffix.size(), [&string](size_t i, size_t j) {
      while (i < string.size() && j < string.size() && string[i] == string[j]) {
        i++;
        j++;
      }

      if (i == string.size()) return false;
      if (j == string.size()) return true;

      return string[i] < string[j];
    });

    return suffix;
  }

  static Vector<size_t> buildLCP(const Vector<T> &string, const Vector<size_t> &suffix) {
    Vector<size_t> lcp(suffix.size() + 1);
    size_t length = suffix.size();

    for (size_t i = 1; i < length; i++) {
      while (suffix[i - 1] + lcp[i] < length - 1 && suffix[i] + lcp[i] < length - 1 &&
             string[suffix[i - 1] + lcp[i]] == string[suffix[i] + lcp[i]])
        lcp[i]++;
    }

    return lcp;
  }

  void buildZMap(size_t i, size_t j, size_t name_len) {
    if (i == j) return;

    assert((i == 0 && j == String.size() && name_len == 0) ||
           ((i != 0 || j != String.size()) && name_len == 1 + std::max(LCP[i], LCP[j + 1])));

    auto children = getChildren(i, j);
    size_t extent_len = LCP[children[1]];
    size_t handle_len = twoFattest(name_len - 1, extent_len);

    auto [it, res] = ZMap.insert(std::pair(hash(String, Suffix[i], handle_len), interval(i, j)));

    assert(res && "The element alaredy exists");

    for (size_t p = 0; p < children.size() - 1; p++)
      buildZMap(children[p], children[p + 1] - 1, extent_len + 1);

    buildZMap(children[children.size() - 1], j, extent_len + 1);
  }

  int64_t twoFattest(size_t a, size_t b) {
    return (int64_t)(1L << 63) >> __builtin_clzll(a ^ b) & b;
  }

  static size_t hash(const Vector<T> &string, size_t start, size_t length) {
    return SpookyHash::Hash64(&string + start, length, 0);
  }

  static Interval<size_t> interval(size_t pos) { return Interval(pos >> 32, pos); }

  static size_t interval(size_t i, size_t j) { return i << 32 | j; }
};

} // namespace zarr
