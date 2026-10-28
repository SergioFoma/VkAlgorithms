#ifndef ALGORITHMS_HPP_
#define ALGORITHMS_HPP_

#include <type_traits>
#include <stdexcept>
#include <cmath>
#include <cstdint>
#include <vector>
#include <string>

namespace algorithms {

template <typename T>
int FindSqrt(T num) {
  static_assert(std::is_arithmetic_v<T>);
  if (num < 0) {
    throw std::runtime_error("FindSqrt:Argument must be a non-negative!");
  }

  int64_t left = 0, right = num + 1;
  int64_t nearest_left = left, nearest_right = right;

  while (left <= right) {
    int64_t middle = (left + right) / 2;
    int64_t square = middle * middle;

    if (square < num) {
      nearest_left = middle;
      left = middle + 1;
    } else if (square > num) {
      nearest_right = middle;
      right = middle - 1;
    } else {
      return middle;
    }
  }

  int64_t nearest_left_square = nearest_left * nearest_left;
  int64_t nearest_right_square = nearest_right * nearest_right;
  return (std::abs(num - nearest_left_square) <= std::abs(num - nearest_right_square) ?
            nearest_left :
            nearest_right);
}

size_t PrintPages(size_t n, size_t x, size_t y);

size_t FeedAnimals(std::vector<size_t>& animals,
                   std::vector<size_t>& food);

char ExtraLetter(const std::string& a,
                 const std::string& b);

std::vector<int> TwoSum(const std::vector<int>& nums, int target);

template <typename T>
void ShellSort(std::vector<T>& arr) {
  size_t n = arr.size();
  size_t gap = n / 2;

  while (gap > 0) {
    for (size_t curr_pos = gap; curr_pos< n; ++curr_pos) {
      size_t m_gap = curr_pos;
      while (m_gap >= gap && arr[m_gap] < arr[m_gap - gap]) {
        std::swap(arr[m_gap], arr[m_gap - gap]);
        m_gap -= gap;
      }
    }
    gap /= 2;
  }
}

std::vector<std::vector<std::string>> GroupAnagrams(
  const std::vector<std::string>& words
);

} // namespace algorithms

#endif
