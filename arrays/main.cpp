#include <iostream>

#include "algorithms.hpp"

namespace {

template<size_t sz>
void PrintArr(const std::array<int, sz>& numbers) {
  for (size_t ind = 0; ind < sz; ++ind) {
    std::cout << numbers[ind] << ' ';
  }
  std::cout << '\n';
}

void TwoSumTest() {
  const size_t kSz = 5;
  std::array<int, kSz> arr = {1, 2, 3, 4, 5};

  TwoSumRes res = TwoSum(arr, 12);
  std::cout << res.first << res.second << '\n';
}

void ReverseTest() {
  const size_t kSz = 5;
  std::array<int, kSz> arr = {1, 2, 3, 4, 5};

  Reverse(arr);
  PrintArr(arr);
}

void ShiftTest() {
  const size_t kSz = 7;
  size_t k = 3;
  std::array<int, kSz> arr = {1, 2, 3, 4, 5, 6, 7};

  RingShift(arr, k);
  PrintArr(arr);
}

void MergeArraysTest() {
  const size_t kArrSz1 = 4;
  const size_t kArrSz2 = 3;

  std::array<int, kArrSz1> arr_1 = {3, 8, 10, 11};
  std::array<int, kArrSz2> arr_2 = {1, 7, 9};

  std::array<int, kArrSz1 + kArrSz2> merge_res = MergeArrays(arr_1, arr_2);

  PrintArr(merge_res);
}

void CheapMergeTest() {
  const size_t kArrSz1 = 7;
  const size_t kArrSz2 = 3;

  std::array<int, kArrSz1> arr_1 = {3, 8, 10, 11};
  std::array<int, kArrSz2> arr_2 = {1, 7, 9};

  CheapMergeArrays(arr_1, arr_2);

  PrintArr(arr_1);
}
} // namespace

int main() {            // Task:

  TwoSumTest();         // 1.

  ReverseTest();        // 2.

  ShiftTest();          // 3.

  MergeArraysTest();    // 4.

  CheapMergeTest();     // 5.
}
