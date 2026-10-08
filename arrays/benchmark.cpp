#include <benchmark/benchmark.h>
#include <algorithm>

#include "algorithms.hpp"

namespace {

template <size_t sz>
void ReverseArray(std::array<int, sz>& nums , size_t left, size_t right) {
  while (left < right) {
    std::swap(nums[left], nums[right]);
    left++;
    right--;
  }
}

template <size_t sz>
void ClassworkRingShift(std::array<int, sz>& nums, size_t shift) {
  shift = sz % sz;
  ReverseArray(nums, 0     , sz - 1);
  ReverseArray(nums, 0     , sz - 1);
  ReverseArray(nums, shift , sz - 1);
}
} // namespace

void BmClasswork1(benchmark::State& state) {
  for (auto st: state) {
    std::array<int, 5> nums = {1, 2, 3, 4, 5};
    ClassworkRingShift(nums,2);
  }
}

void BmMySolution(benchmark::State& state) {
  std::array<int, 5> nums = {1, 2, 3, 4, 5};

  for (auto st: state) {
    RingShift(nums, 2);
  }
}

BENCHMARK(BmClasswork1);
BENCHMARK(BmMySolution);
