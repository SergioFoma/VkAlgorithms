#include <gtest/gtest.h>

#include <array>
#include <initializer_list>
#include <vector>

#include "algorithms.hpp"

namespace {

// Helper: builds a std::array<int, sz> from an initializer list.
template <size_t sz>
std::array<int, sz> MakeArray(std::initializer_list<int> init) {
  std::array<int, sz> arr{};
  size_t i = 0;
  for (int v : init) {
    arr[i++] = v;
  }
  return arr;
}

// ===================== 1. TwoSum =====================

TEST(TwoSum, FindsPair) {
  auto nums = MakeArray<6>({1, 2, 4, 5, 6, 9});
  TwoSumRes res = TwoSum(nums, 10);
  EXPECT_EQ(res.first, 1);
  EXPECT_EQ(res.second, 9);
}

TEST(TwoSum, FindsAdjacentPair) {
  auto nums = MakeArray<4>({3, 5, 7, 10});
  TwoSumRes res = TwoSum(nums, 12);
  EXPECT_EQ(res.first, 5);
  EXPECT_EQ(res.second, 7);
}

TEST(TwoSum, PairOfEqualHalves) {
  auto nums = MakeArray<4>({1, 4, 6, 8});
  TwoSumRes res = TwoSum(nums, 10);
  EXPECT_EQ(res.first, 4);
  EXPECT_EQ(res.second, 6);
}

TEST(TwoSum, NegativeTarget) {
  auto nums = MakeArray<4>({-8, -3, -1, 2});
  TwoSumRes res = TwoSum(nums, -4);
  EXPECT_EQ(res.first, -3);
  EXPECT_EQ(res.second, -1);
}

TEST(TwoSum, NoPairReturnsDefault) {
  auto nums = MakeArray<3>({1, 2, 3});
  TwoSumRes res = TwoSum(nums, 100);
  EXPECT_EQ(res.first, 0);
  EXPECT_EQ(res.second, 0);
}

TEST(TwoSum, EmptyArrayReturnsDefault) {
  auto nums = MakeArray<0>({});
  TwoSumRes res = TwoSum(nums, 5);
  EXPECT_EQ(res.first, 0);
  EXPECT_EQ(res.second, 0);
}

// ===================== 2. Reverse =====================

TEST(Reverse, EvenLength) {
  auto nums = MakeArray<4>({1, 2, 3, 4});
  Reverse(nums);
  EXPECT_EQ(nums, MakeArray<4>({4, 3, 2, 1}));
}

TEST(Reverse, OddLength) {
  auto nums = MakeArray<5>({1, 2, 3, 4, 5});
  Reverse(nums);
  EXPECT_EQ(nums, MakeArray<5>({5, 4, 3, 2, 1}));
}

TEST(Reverse, SingleElementUnchanged) {
  auto nums = MakeArray<1>({42});
  Reverse(nums);
  EXPECT_EQ(nums, MakeArray<1>({42}));
}

TEST(Reverse, EmptyArrayUnchanged) {
  auto nums = MakeArray<0>({});
  Reverse(nums);
  EXPECT_EQ(nums, MakeArray<0>({}));
}

TEST(Reverse, DoubleReverseIsIdentity) {
  auto nums = MakeArray<6>({7, 3, 9, 0, 1, 5});
  Reverse(nums);
  Reverse(nums);
  EXPECT_EQ(nums, MakeArray<6>({7, 3, 9, 0, 1, 5}));
}

// ===================== 3. RingShift =====================

TEST(RingShift, ShiftRightByTwo) {
  auto nums = MakeArray<5>({1, 2, 3, 4, 5});
  RingShift(nums, 2);
  EXPECT_EQ(nums, MakeArray<5>({4, 5, 1, 2, 3}));
}

TEST(RingShift, ShiftByOne) {
  auto nums = MakeArray<4>({10, 20, 30, 40});
  RingShift(nums, 1);
  EXPECT_EQ(nums, MakeArray<4>({40, 10, 20, 30}));
}

TEST(RingShift, ShiftBySizeIsIdentity) {
  auto nums = MakeArray<5>({1, 2, 3, 4, 5});
  RingShift(nums, 5);
  EXPECT_EQ(nums, MakeArray<5>({1, 2, 3, 4, 5}));
}

TEST(RingShift, ShiftLargerThanSize) {
  auto nums = MakeArray<5>({1, 2, 3, 4, 5});
  RingShift(nums, 7);  // equivalent to shift right by 2
  EXPECT_EQ(nums, MakeArray<5>({4, 5, 1, 2, 3}));
}

TEST(RingShift, ShiftRightByThree) {
  auto nums = MakeArray<5>({1, 2, 3, 4, 5});
  RingShift(nums, 3);
  EXPECT_EQ(nums, MakeArray<5>({3, 4, 5, 1, 2}));
}

// ===================== 4. MergeArrays =====================

TEST(MergeArrays, Interleaved) {
  auto a = MakeArray<3>({1, 3, 5});
  auto b = MakeArray<3>({2, 4, 6});
  auto merged = MergeArrays(a, b);
  EXPECT_EQ(merged, MakeArray<6>({1, 2, 3, 4, 5, 6}));
}

TEST(MergeArrays, DisjointRanges) {
  auto a = MakeArray<2>({1, 2});
  auto b = MakeArray<3>({5, 6, 7});
  auto merged = MergeArrays(a, b);
  EXPECT_EQ(merged, MakeArray<5>({1, 2, 5, 6, 7}));
}

TEST(MergeArrays, WithDuplicates) {
  auto a = MakeArray<3>({1, 2, 2});
  auto b = MakeArray<3>({2, 3, 4});
  auto merged = MergeArrays(a, b);
  EXPECT_EQ(merged, MakeArray<6>({1, 2, 2, 2, 3, 4}));
}

TEST(MergeArrays, EqualElements) {
  auto a = MakeArray<2>({1, 1});
  auto b = MakeArray<2>({1, 1});
  auto merged = MergeArrays(a, b);
  EXPECT_EQ(merged, MakeArray<4>({1, 1, 1, 1}));
}

TEST(MergeArrays, EmptySecond) {
  auto a = MakeArray<3>({1, 2, 3});
  auto b = MakeArray<0>({});
  auto merged = MergeArrays(a, b);
  EXPECT_EQ(merged, MakeArray<3>({1, 2, 3}));
}

TEST(MergeArrays, EmptyFirst) {
  auto a = MakeArray<0>({});
  auto b = MakeArray<2>({8, 9});
  auto merged = MergeArrays(a, b);
  EXPECT_EQ(merged, MakeArray<2>({8, 9}));
}

// ===================== 5. FindZero =====================

TEST(FindZero, ZeroAtFront) {
  auto nums = MakeArray<3>({0, 1, 2});
  EXPECT_EQ(FindZero(nums), 0);
}

TEST(FindZero, LastValidBeforeZeroZone) {
  auto nums = MakeArray<5>({1, 2, 0, 3, 4});
  EXPECT_EQ(FindZero(nums), 1);
}

TEST(FindZero, NoZeroReturnsLastIndex) {
  auto nums = MakeArray<3>({1, 2, 3});
  EXPECT_EQ(FindZero(nums), 2);
}

TEST(FindZero, SingleValidElementThenZeros) {
  auto nums = MakeArray<4>({5, 0, 0, 1});
  EXPECT_EQ(FindZero(nums), 0);
}

// ===================== 6. IsValid / CheapMergeArrays =====================

TEST(CheapMergeArrays, AppendsGreaterElement) {
  std::array<int, 4> arr_1 = {1, 2, 3, 0};
  const std::array<int, 1> arr_2 = {4};
  CheapMergeArrays(arr_1, arr_2);
  EXPECT_EQ(arr_1, MakeArray<4>({1, 2, 3, 4}));
}

TEST(CheapMergeArrays, MergesGreaterRange) {
  std::array<int, 6> arr_1 = {1, 3, 5, 0, 0, 0};
  const std::array<int, 3> arr_2 = {6, 7, 8};
  CheapMergeArrays(arr_1, arr_2);
  EXPECT_EQ(arr_1, MakeArray<6>({1, 3, 5, 6, 7, 8}));
}

TEST(CheapMergeArrays, Interleaves) {
  std::array<int, 6> arr_1 = {1, 3, 5, 0, 0, 0};
  const std::array<int, 3> arr_2 = {2, 4, 6};
  CheapMergeArrays(arr_1, arr_2);
  EXPECT_EQ(arr_1, MakeArray<6>({1, 2, 3, 4, 5, 6}));
}

TEST(CheapMergeArrays, MergingEmptyArrayChangesNothing) {
  std::array<int, 3> arr_1 = {0, 0, 0};
  const std::array<int, 0> arr_2 = {};
  CheapMergeArrays(arr_1, arr_2);
  EXPECT_EQ(arr_1, MakeArray<3>({0, 0, 0}));
}

TEST(CheapMergeArrays, ThrowsWhenArr2DoesNotFit) {
  std::array<int, 2> arr_1 = {1, 2};
  const std::array<int, 3> arr_2 = {4, 5, 6};
  EXPECT_THROW(CheapMergeArrays(arr_1, arr_2), std::runtime_error);
}

TEST(CheapMergeArrays, PreconditionViolatedWithoutZeroMarker) {
  std::array<int, 4> arr_1 = {1, 2, 3, 4};
  const std::array<int, 1> arr_2 = {5};
  CheapMergeArrays(arr_1, arr_2);
  EXPECT_EQ(arr_1, MakeArray<4>({5, 5, 5, 5}));
}

TEST(IsValid, ThrowsOnInsufficientSpace) {
  std::array<int, 3> arr_1 = {1, 2, 3};
  EXPECT_THROW(IsValid(arr_1, 4), std::runtime_error);
}

TEST(IsValid, ThrowsWhenArraysBothEmpty) {
  std::array<int, 0> arr_1 = {};
  EXPECT_THROW(IsValid(arr_1, 0), std::runtime_error);
}

TEST(IsValid, ReturnsLastValidIndexBeforeZeroZone) {
  // Contract (via FindZero): cursor to the last real element.
  std::array<int, 5> arr_1 = {1, 2, 0, 0, 0};
  EXPECT_EQ(IsValid(arr_1, 3), 1);
}

// ===================== 7. MinSubarray =====================

TEST(MinSubarray, TypicalCase) {
  auto nums = MakeArray<6>({2, 3, 1, 2, 4, 3});
  std::vector<int> expected = {4, 3};
  EXPECT_EQ(MinSubarray(nums, 7), expected);
}

TEST(MinSubarray, SingleElementEnough) {
  auto nums = MakeArray<4>({1, 2, 8, 4});
  std::vector<int> expected = {8};
  EXPECT_EQ(MinSubarray(nums, 8), expected);
}

TEST(MinSubarray, WholeArrayNeeded) {
  auto nums = MakeArray<3>({1, 2, 3});
  std::vector<int> expected = {1, 2, 3};
  EXPECT_EQ(MinSubarray(nums, 6), expected);
}

TEST(MinSubarray, TargetReachedAtTail) {
  auto nums = MakeArray<5>({1, 1, 1, 5, 3});
  std::vector<int> expected = {5};
  EXPECT_EQ(MinSubarray(nums, 5), expected);
}

TEST(MinSubarray, NoSolutionReturnsEmpty) {
  auto nums = MakeArray<2>({1, 2});
  std::vector<int> result = MinSubarray(nums, 100);
  EXPECT_TRUE(result.empty());
}

// ===================== 8. Sort =====================

TEST(Sort, MixedZerosAndOnes) {
  auto nums = MakeArray<6>({1, 0, 1, 0, 1, 0});
  Sort(nums);
  EXPECT_EQ(nums, MakeArray<6>({0, 0, 0, 1, 1, 1}));
}

TEST(Sort, AlreadySorted) {
  auto nums = MakeArray<4>({0, 0, 1, 1});
  Sort(nums);
  EXPECT_EQ(nums, MakeArray<4>({0, 0, 1, 1}));
}

TEST(Sort, OnlyOnes) {
  auto nums = MakeArray<3>({1, 1, 1});
  Sort(nums);
  EXPECT_EQ(nums, MakeArray<3>({1, 1, 1}));
}

TEST(Sort, OnlyZeros) {
  auto nums = MakeArray<3>({0, 0, 0});
  Sort(nums);
  EXPECT_EQ(nums, MakeArray<3>({0, 0, 0}));
}

TEST(Sort, Reversed) {
  auto nums = MakeArray<4>({1, 1, 0, 0});
  Sort(nums);
  EXPECT_EQ(nums, MakeArray<4>({0, 0, 1, 1}));
}

// ===================== 9. NetherlandsFlag =====================

TEST(NetherlandsFlag, TypicalCase) {
  auto nums = MakeArray<8>({2, 0, 2, 1, 1, 0, 1, 2});
  NetherlandsFlag(nums);
  EXPECT_EQ(nums, MakeArray<8>({0, 0, 1, 1, 1, 2, 2, 2}));
}

TEST(NetherlandsFlag, Reversed) {
  auto nums = MakeArray<3>({2, 1, 0});
  NetherlandsFlag(nums);
  EXPECT_EQ(nums, MakeArray<3>({0, 1, 2}));
}

TEST(NetherlandsFlag, AlreadySorted) {
  auto nums = MakeArray<5>({0, 0, 1, 2, 2});
  NetherlandsFlag(nums);
  EXPECT_EQ(nums, MakeArray<5>({0, 0, 1, 2, 2}));
}

TEST(NetherlandsFlag, OnlyOnes) {
  auto nums = MakeArray<4>({1, 1, 1, 1});
  NetherlandsFlag(nums);
  EXPECT_EQ(nums, MakeArray<4>({1, 1, 1, 1}));
}

TEST(NetherlandsFlag, EachValueOnce) {
  auto nums = MakeArray<3>({1, 2, 0});
  NetherlandsFlag(nums);
  EXPECT_EQ(nums, MakeArray<3>({0, 1, 2}));
}

// ===================== 10. SortEven =====================

TEST(SortEven, BasicPartition) {
  auto nums = MakeArray<4>({1, 2, 3, 4});
  SortEven(nums);
  EXPECT_EQ(nums, MakeArray<4>({2, 4, 3, 1}));
}

TEST(SortEven, ReversePartition) {
  auto nums = MakeArray<5>({7, 3, 2, 8, 5});
  SortEven(nums);
  EXPECT_EQ(nums, MakeArray<5>({2, 8, 7, 3, 5}));
}

TEST(SortEven, AllEvenUnchanged) {
  auto nums = MakeArray<3>({2, 6, 8});
  SortEven(nums);
  EXPECT_EQ(nums, MakeArray<3>({2, 6, 8}));
}

TEST(SortEven, AllOddUnchanged) {
  auto nums = MakeArray<3>({1, 3, 5});
  SortEven(nums);
  EXPECT_EQ(nums, MakeArray<3>({1, 3, 5}));
}

TEST(SortEven, EvensEndUpBeforeOdds) {
  auto nums = MakeArray<6>({5, 1, 6, 3, 8, 7});
  SortEven(nums);
  auto is_even = [](int x) { return x % 2 == 0; };
  EXPECT_TRUE(is_even(nums[0]));
  EXPECT_TRUE(is_even(nums[1]));
  EXPECT_FALSE(is_even(nums[2]));
  EXPECT_FALSE(is_even(nums[3]));
  EXPECT_FALSE(is_even(nums[4]));
  EXPECT_FALSE(is_even(nums[5]));
}

// ===================== 11. MoveZeros =====================

TEST(MoveZeros, TypicalCase) {
  auto nums = MakeArray<5>({0, 1, 0, 3, 12});
  MoveZeros(nums);
  EXPECT_EQ(nums, MakeArray<5>({1, 3, 12, 0, 0}));
}

TEST(MoveZeros, NoZerosUnchanged) {
  auto nums = MakeArray<3>({1, 2, 3});
  MoveZeros(nums);
  EXPECT_EQ(nums, MakeArray<3>({1, 2, 3}));
}

TEST(MoveZeros, AllZerosUnchanged) {
  auto nums = MakeArray<4>({0, 0, 0, 0});
  MoveZeros(nums);
  EXPECT_EQ(nums, MakeArray<4>({0, 0, 0, 0}));
}

TEST(MoveZeros, ZerosInFrontShiftToBack) {
  auto nums = MakeArray<4>({0, 0, 1, 2});
  MoveZeros(nums);
  EXPECT_EQ(nums, MakeArray<4>({1, 2, 0, 0}));
}

TEST(MoveZeros, SingleZeroInMiddle) {
  auto nums = MakeArray<5>({0, 4, 1, 9, 2});
  MoveZeros(nums);
  EXPECT_EQ(nums, MakeArray<5>({4, 1, 9, 2, 0}));
}

}  // namespace
