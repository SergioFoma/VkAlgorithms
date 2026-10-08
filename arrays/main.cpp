#include <iostream>

#include "algorithms.hpp"

namespace {

template<size_t sz>
void PrintArr(const std::array<int, sz>& nums) {
  for (size_t ind = 0; ind < sz; ++ind) {
    std::cout << nums[ind] << ' ';
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

void MinSubarrayTest() {
  const size_t kArrSz = 7;
  std::array<int, kArrSz> arr = {2, 3, 1, 2, 4, 3};
  int target = 7;

  std::vector<int> res = MinSubarray(arr, target);
  for (int ind = 0; ind < res.size(); ++ind) {
    std::cout << res[ind] << ' ';
  }
  std::cout << '\n';
}

void SortTest() {
  const size_t kArrSz = 7;
  std::array<int, kArrSz> arr = {0, 1, 1, 0, 1, 0, 0};

  Sort(arr);

  PrintArr(arr);
}

void NetherlandsFlagTest() {
  const size_t kArrSz = 10;
  std::array<int, kArrSz> arr = {0, 2, 2, 1, 0, 1, 1, 1, 2, 0};

  NetherlandsFlag(arr);

  PrintArr(arr);
}

void SortEvenTest() {
  const size_t kArrSz = 7;
  std::array<int, kArrSz> arr = {3, 2, 4, 1, 11, 8, 9};
  //std::array<int, kArrSz> arr = {1, 3, 5, 7, 9, 11, 13};
  SortEven(arr);

  PrintArr(arr);
}

void MoveZerosTest() {
  const size_t kArrSz1 = 6;
  std::array<int, kArrSz1> arr_1 = {0, 0, 1, 0, 3, 12};
  MoveZeros(arr_1);
  std::cout << "=====Move Zeros (Test 1)=====\n";
  PrintArr(arr_1);

  const size_t kArrSz2 = 9;
  std::array<int, kArrSz2> arr_2 = {0, 33, 57, 88, 60, 0, 0, 80, 99};
  MoveZeros(arr_2);
  std::cout << "\n=====Move Zeros (Test 2)=====\n";
  PrintArr(arr_2);

  const size_t kArrSz3 = 9;
  std::array<int, kArrSz3> arr_3 = {0, 0, 0, 18, 16, 0, 0, 77, 99};
  MoveZeros(arr_3);
  std::cout << "\n=====Move Zeros (Test 3)=====\n";
  PrintArr(arr_3);
}
} // namespace

int main() {              // Task:

  TwoSumTest();           // 1.
  ReverseTest();          // 2.
  ShiftTest();            // 3.
  MergeArraysTest();      // 4.
  CheapMergeTest();       // 5.
  MinSubarrayTest();      // 6.
  SortTest();             // 7.
  NetherlandsFlagTest();  // 8.
  SortEvenTest();         // 9.
  MoveZerosTest();        // 10.
}
