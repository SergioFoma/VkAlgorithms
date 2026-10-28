#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "algorithms.hpp"

namespace {

size_t CopyTime(size_t n, size_t x, size_t y) {
  return algorithms::PrintPages(n, x, y);
}

size_t Feed(std::vector<size_t> animals, std::vector<size_t> food) {
  return algorithms::FeedAnimals(animals, food);
}

std::vector<int> SortedTwoSum(const std::vector<int>& nums, int target) {
  std::vector<int> result = algorithms::TwoSum(nums, target);
  std::sort(result.begin(), result.end());
  return result;
}

void ExpectValidTwoSum(const std::vector<int>& nums, int target) {
  std::vector<int> result = algorithms::TwoSum(nums, target);
  ASSERT_EQ(result.size(), 2u);
  EXPECT_EQ(result[0] + result[1], target);

  std::vector<int> pool = nums;
  auto it = std::find(pool.begin(), pool.end(), result[0]);
  ASSERT_NE(it, pool.end());
  pool.erase(it);
  it = std::find(pool.begin(), pool.end(), result[1]);
  ASSERT_NE(it, pool.end());
}

template <typename T>
void ExpectShellSorted(std::vector<T> values) {
  std::vector<T> expected = values;
  std::sort(expected.begin(), expected.end());
  algorithms::ShellSort(values);
  EXPECT_EQ(values, expected);
}

using Groups = std::vector<std::vector<std::string>>;

Groups NormalizeGroups(Groups groups) {
  for (auto& group : groups) {
    std::sort(group.begin(), group.end());
  }
  std::sort(groups.begin(), groups.end());
  return groups;
}

void ExpectAnagramGroups(const std::vector<std::string>& words,
                         Groups expected) {
  const Groups actual =
      NormalizeGroups(algorithms::GroupAnagrams(words));
  EXPECT_EQ(actual, NormalizeGroups(std::move(expected)));
}

}

TEST(FindSqrtTest, Zero) {
  EXPECT_EQ(algorithms::FindSqrt(0), 0);
}

TEST(FindSqrtTest, One) {
  EXPECT_EQ(algorithms::FindSqrt(1), 1);
}

TEST(FindSqrtTest, PerfectSquares) {
  const std::vector<std::pair<int, int>> cases{
      {0, 0},    {1, 1},     {4, 2},     {9, 3},     {16, 4},
      {25, 5},   {36, 6},    {49, 7},    {100, 10},  {10'000, 100},
  };

  for (const auto& [number, expected] : cases) {
    EXPECT_EQ(algorithms::FindSqrt(number), expected) << "n = " << number;
  }
}

TEST(FindSqrtTest, RoundsDownWhenLowerSquareIsCloser) {
  EXPECT_EQ(algorithms::FindSqrt(2), 1);
  EXPECT_EQ(algorithms::FindSqrt(5), 2);
  EXPECT_EQ(algorithms::FindSqrt(10), 3);
  EXPECT_EQ(algorithms::FindSqrt(17), 4);
  EXPECT_EQ(algorithms::FindSqrt(26), 5);
  EXPECT_EQ(algorithms::FindSqrt(37), 6);
}

TEST(FindSqrtTest, RoundsUpWhenUpperSquareIsCloser) {
  EXPECT_EQ(algorithms::FindSqrt(3), 2);
  EXPECT_EQ(algorithms::FindSqrt(8), 3);
  EXPECT_EQ(algorithms::FindSqrt(15), 4);
  EXPECT_EQ(algorithms::FindSqrt(24), 5);
  EXPECT_EQ(algorithms::FindSqrt(35), 6);
  EXPECT_EQ(algorithms::FindSqrt(48), 7);
}

TEST(FindSqrtTest, ValuesJustBelowAndAboveASquare) {
  EXPECT_EQ(algorithms::FindSqrt(99), 10);
  EXPECT_EQ(algorithms::FindSqrt(101), 10);
  EXPECT_EQ(algorithms::FindSqrt(9'999), 100);
  EXPECT_EQ(algorithms::FindSqrt(10'001), 100);
}

TEST(FindSqrtTest, LargePerfectSquare) {
  EXPECT_EQ(algorithms::FindSqrt(900'000'000), 30'000);
}

TEST(FindSqrtTest, LargeNonSquare) {
  EXPECT_EQ(algorithms::FindSqrt(1'000'000'000), 31'623);
}

TEST(FindSqrtTest, VeryLargeValues) {
  EXPECT_EQ(algorithms::FindSqrt(2'000'000'000), 44'721);
  EXPECT_EQ(algorithms::FindSqrt(2'147'395'600), 46'340);
  EXPECT_EQ(algorithms::FindSqrt(std::numeric_limits<int>::max()), 46'341);
}

TEST(FindSqrtTest, NegativeArgumentThrows) {
  EXPECT_THROW(algorithms::FindSqrt(-1), std::runtime_error);
  EXPECT_THROW(algorithms::FindSqrt(-100), std::runtime_error);
}

TEST(PrintPagesTest, ZeroCopiesNeedNoTime) {
  EXPECT_EQ(CopyTime(0, 2, 3), 0u);
  EXPECT_EQ(CopyTime(0, 1, 100), 0u);
}

TEST(PrintPagesTest, OneCopyUsesTheFasterCopier) {
  EXPECT_EQ(CopyTime(1, 5, 3), 3u);
  EXPECT_EQ(CopyTime(1, 3, 5), 3u);
  EXPECT_EQ(CopyTime(1, 4, 4), 4u);
}

TEST(PrintPagesTest, TwoCopies) {
  EXPECT_EQ(CopyTime(2, 1, 2), 2u);
  EXPECT_EQ(CopyTime(2, 2, 3), 4u);
  EXPECT_EQ(CopyTime(2, 1, 10), 2u);
  EXPECT_EQ(CopyTime(2, 4, 4), 8u);
}

TEST(PrintPagesTest, EqualCopiers) {
  EXPECT_EQ(CopyTime(3, 1, 1), 2u);
  EXPECT_EQ(CopyTime(5, 1, 1), 3u);
  EXPECT_EQ(CopyTime(3, 4, 4), 8u);
  EXPECT_EQ(CopyTime(4, 4, 4), 12u);
  EXPECT_EQ(CopyTime(5, 4, 4), 12u);
  EXPECT_EQ(CopyTime(6, 4, 4), 16u);
}

TEST(PrintPagesTest, DifferentCopiers) {
  EXPECT_EQ(CopyTime(3, 2, 3), 5u);
  EXPECT_EQ(CopyTime(4, 2, 3), 6u);
  EXPECT_EQ(CopyTime(5, 2, 3), 8u);
  EXPECT_EQ(CopyTime(3, 3, 5), 8u);
  EXPECT_EQ(CopyTime(7, 4, 6), 20u);
  EXPECT_EQ(CopyTime(9, 2, 7), 16u);
  EXPECT_EQ(CopyTime(20, 3, 8), 45u);
}

TEST(PrintPagesTest, ManyCopies) {
  EXPECT_EQ(CopyTime(10, 1, 2), 7u);
  EXPECT_EQ(CopyTime(10, 3, 5), 21u);
  EXPECT_EQ(CopyTime(100, 1, 100), 100u);
}

TEST(PrintPagesTest, OneCopyIsNotSlowerThanBothWorkingAlone) {
  for (size_t n : {size_t{1}, size_t{2}, size_t{7}, size_t{50}}) {
    const size_t result = CopyTime(n, 3, 5);
    EXPECT_GE(result, 3u) << "n = " << n;
    EXPECT_LE(result, n * 5u) << "n = " << n;
  }
}

TEST(FeedAnimalsTest, EmptyAnimals) {
  EXPECT_EQ(Feed({}, {1, 2, 3}), 0u);
}

TEST(FeedAnimalsTest, EmptyFood) {
  EXPECT_EQ(Feed({1, 2, 3}, {}), 0u);
}

TEST(FeedAnimalsTest, BothEmpty) {
  EXPECT_EQ(Feed({}, {}), 0u);
}

TEST(FeedAnimalsTest, ExampleFromStatement) {
  EXPECT_EQ(Feed({8, 1, 4, 2, 3}, {7, 1, 9, 3, 8}), 5u);
}

TEST(FeedAnimalsTest, NoPortionIsBigEnough) {
  EXPECT_EQ(Feed({5, 6}, {1, 2, 4}), 0u);
  EXPECT_EQ(Feed({3}, {2}), 0u);
}

TEST(FeedAnimalsTest, EveryAnimalCanBeFed) {
  EXPECT_EQ(Feed({1, 2, 3}, {1, 2, 3}), 3u);
  EXPECT_EQ(Feed({1, 2, 3}, {10, 10, 10}), 3u);
}

TEST(FeedAnimalsTest, NotEveryAnimalFitsIntoTheFood) {
  EXPECT_EQ(Feed({1, 2, 3}, {1, 1, 1}), 1u);
  EXPECT_EQ(Feed({1, 1, 1}, {1, 2}), 2u);
}

TEST(FeedAnimalsTest, PortionFeedsOnlyOneAnimal) {
  EXPECT_EQ(Feed({1, 1}, {2}), 1u);
  EXPECT_EQ(Feed({2, 2}, {3}), 1u);
  EXPECT_EQ(Feed({1, 1, 1}, {3}), 1u);
}

TEST(FeedAnimalsTest, ExactMatches) {
  EXPECT_EQ(Feed({3, 5}, {3, 5}), 2u);
  EXPECT_EQ(Feed({2, 4, 6}, {2, 4, 6}), 3u);
}

TEST(FeedAnimalsTest, UnusedFoodIsAllowed) {
  EXPECT_EQ(Feed({1, 2}, {100, 100, 100}), 2u);
  EXPECT_EQ(Feed({4, 8}, {3, 4, 8, 20}), 2u);
}

TEST(FeedAnimalsTest, DuplicateRequirementsAndPortions) {
  EXPECT_EQ(Feed({2, 2, 3, 3}, {2, 2, 3, 3}), 4u);
  EXPECT_EQ(Feed({2, 2, 2}, {2, 2}), 2u);
}

TEST(FeedAnimalsTest, OrderOfInputsDoesNotMatter) {
  EXPECT_EQ(Feed({1, 2, 3, 4, 8}, {8, 3, 9, 1, 7}), 5u);
  EXPECT_EQ(Feed({8, 4, 1, 3, 2}, {7, 9, 8, 1, 3}), 5u);
  EXPECT_EQ(Feed({4, 1, 3, 2}, {3, 1, 4, 2}), 4u);
}

TEST(FeedAnimalsTest, BiggerPortionCanFeedSmallerAnimal) {
  EXPECT_EQ(Feed({1, 5}, {4}), 1u);
  EXPECT_EQ(Feed({1, 5}, {5, 1}), 2u);
  EXPECT_EQ(Feed({10}, {100}), 1u);
}

TEST(FeedAnimalsTest, MoreAnimalsThanFood) {
  EXPECT_EQ(Feed({1, 2, 3, 4}, {4, 3}), 2u);
  EXPECT_EQ(Feed({1, 2, 3}, {100}), 1u);
}

TEST(FeedAnimalsTest, LargeValues) {
  EXPECT_EQ(Feed({1'000'000'000, 2'000'000'000},
                 {2'000'000'000, 999'999'999}),
            1u);
  EXPECT_EQ(Feed({1'000'000'000, 2'000'000'000},
                 {2'000'000'000, 1'000'000'000}),
            2u);
}

TEST(ExtraLetterTest, ExtraAtEnd) {
  EXPECT_EQ(algorithms::ExtraLetter("abc", "abcd"), 'd');
}

TEST(ExtraLetterTest, ExtraAtBeginning) {
  EXPECT_EQ(algorithms::ExtraLetter("abc", "zabc"), 'z');
}

TEST(ExtraLetterTest, ExtraInMiddle) {
  EXPECT_EQ(algorithms::ExtraLetter("abc", "abxc"), 'x');
}

TEST(ExtraLetterTest, ExtraRepeatsExistingCharacter) {
  EXPECT_EQ(algorithms::ExtraLetter("abc", "abcc"), 'c');
  EXPECT_EQ(algorithms::ExtraLetter("aab", "aaab"), 'a');
  EXPECT_EQ(algorithms::ExtraLetter("aabb", "aabbb"), 'b');
}

TEST(ExtraLetterTest, SingleCharacterStrings) {
  EXPECT_EQ(algorithms::ExtraLetter("a", "aa"), 'a');
  EXPECT_EQ(algorithms::ExtraLetter("a", "ba"), 'b');
}

TEST(ExtraLetterTest, EmptyFirstString) {
  EXPECT_EQ(algorithms::ExtraLetter("", "x"), 'x');
  EXPECT_EQ(algorithms::ExtraLetter("", "Z"), 'Z');
}

TEST(ExtraLetterTest, PermutationPlusExtra) {
  EXPECT_EQ(algorithms::ExtraLetter("listen", "silentx"), 'x');
}

TEST(ExtraLetterTest, ExtraDigit) {
  EXPECT_EQ(algorithms::ExtraLetter("abc", "1abc"), '1');
}

TEST(ExtraLetterTest, ExtraSpace) {
  EXPECT_EQ(algorithms::ExtraLetter("ab", "a b"), ' ');
}

TEST(ExtraLetterTest, CaseSensitive) {
  EXPECT_EQ(algorithms::ExtraLetter("a", "aA"), 'A');
}

TEST(ExtraLetterTest, EqualStringsReturnNull) {
  EXPECT_EQ(algorithms::ExtraLetter("abc", "abc"), '\0');
  EXPECT_EQ(algorithms::ExtraLetter("", ""), '\0');
}

TEST(ExtraLetterTest, ShorterSecondStringThrows) {
  EXPECT_THROW(algorithms::ExtraLetter("abc", "ab"), std::runtime_error);
  EXPECT_THROW(algorithms::ExtraLetter("a", ""), std::runtime_error);
}

TEST(ExtraLetterTest, LongStrings) {
  std::string a(1000, 'a');
  std::string b = a + 'z';
  EXPECT_EQ(algorithms::ExtraLetter(a, b), 'z');
}

TEST(TwoSumTest, EmptyArray) {
  EXPECT_TRUE(algorithms::TwoSum({}, 10).empty());
}

TEST(TwoSumTest, SingleElement) {
  EXPECT_TRUE(algorithms::TwoSum({5}, 10).empty());
  EXPECT_TRUE(algorithms::TwoSum({5}, 5).empty());
}

TEST(TwoSumTest, UniquePairAtEnd) {
  EXPECT_EQ(SortedTwoSum({1, 2, 3, 4, 5}, 9), std::vector<int>({4, 5}));
}

TEST(TwoSumTest, UniquePairAtStart) {
  EXPECT_EQ(SortedTwoSum({1, 2, 3}, 3), std::vector<int>({1, 2}));
}

TEST(TwoSumTest, UniquePairInMiddle) {
  EXPECT_EQ(SortedTwoSum({1, 2, 3}, 4), std::vector<int>({1, 3}));
}

TEST(TwoSumTest, UniquePairWithDuplicatedValue) {
  EXPECT_EQ(SortedTwoSum({1, 1, 2, 3}, 3), std::vector<int>({1, 2}));
  EXPECT_EQ(SortedTwoSum({1, 1, 2, 3}, 5), std::vector<int>({2, 3}));
}

TEST(TwoSumTest, UsesSameValueTwiceWhenPresentTwice) {
  EXPECT_EQ(SortedTwoSum({2, 2}, 4), std::vector<int>({2, 2}));
  EXPECT_EQ(SortedTwoSum({1, 1, 2}, 2), std::vector<int>({1, 1}));
  EXPECT_EQ(SortedTwoSum({0, 0}, 0), std::vector<int>({0, 0}));
}

TEST(TwoSumTest, DoesNotUseSameElementTwice) {
  EXPECT_TRUE(algorithms::TwoSum({1, 2, 3}, 2).empty());
  EXPECT_TRUE(algorithms::TwoSum({0, 1}, 0).empty());
  EXPECT_TRUE(algorithms::TwoSum({3, 4, 5}, 10).empty());
}

TEST(TwoSumTest, HandlesZero) {
  EXPECT_EQ(SortedTwoSum({0, 5}, 5), std::vector<int>({0, 5}));
}

TEST(TwoSumTest, HandlesNegativeNumbers) {
  EXPECT_EQ(SortedTwoSum({-8, -3, -1, 2}, -4), std::vector<int>({-3, -1}));
}

TEST(TwoSumTest, NoPair) {
  EXPECT_TRUE(algorithms::TwoSum({1, 2, 3, 4, 5}, 100).empty());
  EXPECT_TRUE(algorithms::TwoSum({1, 1, 2}, 6).empty());
}

TEST(TwoSumTest, MultipleValidPairsAreAccepted) {
  ExpectValidTwoSum({1, 2, 3, 4, 5}, 5);
  ExpectValidTwoSum({1, 2, 2, 3}, 4);
  ExpectValidTwoSum({1, 2, 3, 4, 5}, 6);
}

TEST(ShellSortTest, EmptyVector) {
  ExpectShellSorted<int>({});
}

TEST(ShellSortTest, SingleElement) {
  ExpectShellSorted<int>({42});
}

TEST(ShellSortTest, TwoElements) {
  ExpectShellSorted<int>({2, 1});
}

TEST(ShellSortTest, AlreadySorted) {
  ExpectShellSorted<int>({1, 2, 3, 4, 5});
}

TEST(ShellSortTest, ReverseSorted) {
  ExpectShellSorted<int>({5, 4, 3, 2, 1});
}

TEST(ShellSortTest, Duplicates) {
  ExpectShellSorted<int>({3, 1, 3, 2, 1, 3});
}

TEST(ShellSortTest, NegativeNumbers) {
  ExpectShellSorted<int>({0, -5, 3, -1, 2, -5});
}

TEST(ShellSortTest, AllEqual) {
  ExpectShellSorted<int>({7, 7, 7, 7});
}

TEST(ShellSortTest, MixedValues) {
  ExpectShellSorted<int>({9, -2, 0, 4, -7, 3, 1, 8});
}

TEST(ShellSortTest, OddAndEvenSizes) {
  for (size_t n : {size_t{3}, size_t{4}, size_t{7}, size_t{8}, size_t{15},
                   size_t{16}}) {
    std::vector<int> values;
    for (size_t i = 0; i < n; ++i) {
      values.push_back(static_cast<int>((i * 7 + 3) % 11) - 5);
    }
    ExpectShellSorted<int>(values);
  }
}

TEST(ShellSortTest, LargeVector) {
  std::vector<int> values;
  for (int i = 0; i < 1000; ++i) {
    values.push_back((i * 37 + 11) % 101 - 50);
  }
  ExpectShellSorted<int>(values);
}

TEST(ShellSortTest, Strings) {
  ExpectShellSorted<std::string>({"pear", "apple", "fig", "banana"});
}

TEST(ShellSortTest, Doubles) {
  ExpectShellSorted<double>({1.5, -2.0, 0.0, 3.25, -0.5});
}

TEST(GroupAnagramsTest, EmptyInput) {
  ExpectAnagramGroups({}, {});
}

TEST(GroupAnagramsTest, SingleWord) {
  ExpectAnagramGroups({"hello"}, {{"hello"}});
}

TEST(GroupAnagramsTest, ExampleOne) {
  ExpectAnagramGroups({"eat", "tea", "tan", "ate", "nat", "bat"},
                      {{"bat"}, {"nat", "tan"}, {"ate", "eat", "tea"}});
}

TEST(GroupAnagramsTest, ExampleTwo) {
  ExpectAnagramGroups({"won", "now", "aaa", "ooo", "ooo"},
                      {{"aaa"}, {"ooo", "ooo"}, {"won", "now"}});
}

TEST(GroupAnagramsTest, NoAnagrams) {
  ExpectAnagramGroups({"a", "b", "c"}, {{"a"}, {"b"}, {"c"}});
}

TEST(GroupAnagramsTest, AllAnagrams) {
  ExpectAnagramGroups({"eat", "tea", "ate"}, {{"ate", "eat", "tea"}});
}

TEST(GroupAnagramsTest, DuplicateWords) {
  ExpectAnagramGroups({"aaa", "aaa"}, {{"aaa", "aaa"}});
}

TEST(GroupAnagramsTest, DuplicateWordsAmongAnagrams) {
  ExpectAnagramGroups({"eat", "eat", "tea", "ate"},
                      {{"ate", "eat", "eat", "tea"}});
}

TEST(GroupAnagramsTest, RepeatedLetters) {
  ExpectAnagramGroups({"aab", "aba", "baa", "abb", "bab"},
                      {{"aab", "aba", "baa"}, {"abb", "bab"}});
}

TEST(GroupAnagramsTest, DifferentLengths) {
  ExpectAnagramGroups({"a", "aa", "aaa"}, {{"a"}, {"aa"}, {"aaa"}});
}

TEST(GroupAnagramsTest, CaseSensitive) {
  ExpectAnagramGroups({"a", "A", "a"}, {{"a", "a"}, {"A"}});
}

TEST(GroupAnagramsTest, EmptyStrings) {
  ExpectAnagramGroups({"", ""}, {{"", ""}});
  ExpectAnagramGroups({"", "a"}, {{""}, {"a"}});
}

TEST(GroupAnagramsTest, WhitespaceAndPunctuation) {
  ExpectAnagramGroups({"a b", "b a", "ab ", "!a", "a!"},
                      {{"a b", "b a", "ab "}, {"!a", "a!"}});
}

TEST(GroupAnagramsTest, LongAnagrams) {
  ExpectAnagramGroups({"listen", "silent", "enlist", "tinsel", "inlets"},
                      {{"enlist", "inlets", "listen", "silent", "tinsel"}});
}

TEST(GroupAnagramsTest, SeveralGroups) {
  ExpectAnagramGroups({"abc", "bca", "cab", "foo", "ofo", "bar", "rab"},
                      {{"abc", "bca", "cab"}, {"foo", "ofo"}, {"bar", "rab"}});
}
