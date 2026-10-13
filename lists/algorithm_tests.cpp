#include <stack>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "algorithms.hpp"

using list::List;
using list::Node;

namespace {

template <typename T>
std::vector<T> PopAll(std::stack<T> stack) {
  std::vector<T> result;
  while (!stack.empty()) {
    result.push_back(std::move(stack.top()));
    stack.pop();
  }
  return result;
}

}  // namespace

namespace {

template <typename T>
void ReleaseOwnership(List<T>& lst) {
  lst.SetNewHead(new Node<T>{});
}

template <typename T>
Node<T>* Tail(List<T>& lst) {
  Node<T>* node = &lst.Front();
  while (node->next != nullptr) {
    node = node->next;
  }
  return node;
}

template <typename T>
std::vector<T> Chain(const Node<T>* head, size_t limit = 16) {
  std::vector<T> result;
  while (head != nullptr && result.size() < limit) {
    result.push_back(head->val);
    head = head->next;
  }
  return result;
}

}  // namespace

TEST(IsRingTest, EmptyIsNotRing) {
  List<int> lst;
  EXPECT_FALSE(algorithm::IsRing(lst));
}

TEST(IsRingTest, SingleElementIsNotRing) {
  List<int> lst(1);
  EXPECT_FALSE(algorithm::IsRing(lst));
}

TEST(IsRingTest, PlainListIsNotRing) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  lst.PushBack(3);
  lst.PushBack(4);
  EXPECT_FALSE(algorithm::IsRing(lst));
}

TEST(IsRingTest, SelfLoopIsRing) {
  List<int> lst(1);
  Node<int>* node = &lst.Front();
  node->next = node;
  EXPECT_TRUE(algorithm::IsRing(lst));
  node->next = nullptr;
}

TEST(IsRingTest, ClosedTailIsRing) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  lst.PushBack(3);
  Node<int>* tail = Tail(lst);
  tail->next = &lst.Front();
  EXPECT_TRUE(algorithm::IsRing(lst));
  tail->next = nullptr;
  EXPECT_FALSE(algorithm::IsRing(lst));
}

TEST(IsRingTest, RhoShapedRingIsRing) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  lst.PushBack(3);
  lst.PushBack(4);
  Node<int>* tail = Tail(lst);
  tail->next = lst.Front().next;
  EXPECT_TRUE(algorithm::IsRing(lst));
  tail->next = nullptr;
}

TEST(ReverseTest, EmptyStaysEmpty) {
  List<int> lst;
  algorithm::Reverse(lst);
  EXPECT_TRUE(lst.Empty());
}

TEST(ReverseTest, SingleElementUnchanged) {
  List<int> lst(5);
  Node<int>* only = &lst.Front();
  algorithm::Reverse(lst);
  EXPECT_EQ(&lst.Front(), only);
  EXPECT_EQ(lst.Front().val, 5);
  EXPECT_EQ(lst.Front().next, nullptr);
}

TEST(ReverseTest, ReversesTwoElements) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  algorithm::Reverse(lst);
  EXPECT_EQ(Chain(&lst.Front()), std::vector<int>({2, 1}));
  EXPECT_EQ(Tail(lst)->next, nullptr);
}

TEST(ReverseTest, ReversesThreeElements) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  lst.PushBack(3);
  Node<int>* old_head = &lst.Front();
  Node<int>* old_tail = Tail(lst);
  algorithm::Reverse(lst);
  EXPECT_EQ(&lst.Front(), old_tail);
  EXPECT_EQ(Chain(&lst.Front()), std::vector<int>({3, 2, 1}));
  EXPECT_EQ(old_head->next, nullptr);
  EXPECT_EQ(Tail(lst)->next, nullptr);
}

TEST(ReverseTest, ReverseTwiceRestoresOrder) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  lst.PushBack(3);
  lst.PushBack(4);
  Node<int>* old_head = &lst.Front();
  algorithm::Reverse(lst);
  algorithm::Reverse(lst);
  EXPECT_EQ(&lst.Front(), old_head);
  EXPECT_EQ(Chain(&lst.Front()), std::vector<int>({1, 2, 3, 4}));
}

TEST(FindMiddleTest, EmptyReturnsNullptr) {
  List<int> lst;
  EXPECT_EQ(algorithm::FindMiddle(lst), nullptr);
}

TEST(FindMiddleTest, SingleElement) {
  List<int> lst(1);
  EXPECT_EQ(algorithm::FindMiddle(lst), &lst.Front());
}

TEST(FindMiddleTest, TwoElementsReturnsSecond) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  Node<int>* middle = algorithm::FindMiddle(lst);
  ASSERT_NE(middle, nullptr);
  EXPECT_EQ(middle, lst.Front().next);
  EXPECT_EQ(middle->val, 2);
}

TEST(FindMiddleTest, ThreeElementsReturnsSecond) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  lst.PushBack(3);
  Node<int>* middle = algorithm::FindMiddle(lst);
  ASSERT_NE(middle, nullptr);
  EXPECT_EQ(middle->val, 2);
}

TEST(FindMiddleTest, FourElementsReturnsThird) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  lst.PushBack(3);
  lst.PushBack(4);
  Node<int>* middle = algorithm::FindMiddle(lst);
  ASSERT_NE(middle, nullptr);
  EXPECT_EQ(middle->val, 3);
}

TEST(DeleteElemTest, DeletesMiddleAndReturnsNext) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  lst.PushBack(3);
  Node<int>* next = algorithm::DeleteElem(lst, 2);
  ASSERT_NE(next, nullptr);
  EXPECT_EQ(next->val, 3);
  EXPECT_EQ(Chain(&lst.Front()), std::vector<int>({1, 3}));
}

TEST(DeleteElemTest, DeletesFrontAndReturnsNext) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  lst.PushBack(3);
  Node<int>* next = algorithm::DeleteElem(lst, 1);
  ASSERT_NE(next, nullptr);
  EXPECT_EQ(next, &lst.Front());
  EXPECT_EQ(Chain(&lst.Front()), std::vector<int>({2, 3}));
}

TEST(DeleteElemTest, DeletesLastReturnsNullptr) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  EXPECT_EQ(algorithm::DeleteElem(lst, 2), nullptr);
  EXPECT_EQ(Chain(&lst.Front()), std::vector<int>({1}));
}

TEST(DeleteElemTest, EmptyReturnsNullptr) {
  List<int> lst;
  EXPECT_EQ(algorithm::DeleteElem(lst, 1), nullptr);
}

TEST(DeleteElemTest, MissingValueThrowsAndKeepsList) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  EXPECT_THROW(algorithm::DeleteElem(lst, 99), std::runtime_error);
  EXPECT_EQ(Chain(&lst.Front()), std::vector<int>({1, 2}));
}

TEST(DeleteElemTest, DuplicatesDeleteFirstOnly) {
  List<int> lst;
  lst.PushBack(7);
  lst.PushBack(7);
  lst.PushBack(8);
  Node<int>* next = algorithm::DeleteElem(lst, 7);
  ASSERT_NE(next, nullptr);
  EXPECT_EQ(next->val, 7);
  EXPECT_EQ(Chain(&lst.Front()), std::vector<int>({7, 8}));
}

TEST(FindSubstrTest, EmptySourceIsAlwaysFound) {
  EXPECT_TRUE(algorithm::FindSubstr("anything", ""));
  EXPECT_TRUE(algorithm::FindSubstr("", ""));
}

TEST(FindSubstrTest, SourceLongerThanDestNotFound) {
  EXPECT_FALSE(algorithm::FindSubstr("ab", "abc"));
  EXPECT_FALSE(algorithm::FindSubstr("", "a"));
}

TEST(FindSubstrTest, ContiguousMatchFound) {
  EXPECT_TRUE(algorithm::FindSubstr("hello world", "world"));
  EXPECT_TRUE(algorithm::FindSubstr("hello world", "hello"));
  EXPECT_TRUE(algorithm::FindSubstr("hello world", "lo wo"));
  EXPECT_TRUE(algorithm::FindSubstr("abc", "abc"));
}

TEST(FindSubstrTest, NonContiguousSubsequenceIsFound) {
  EXPECT_TRUE(algorithm::FindSubstr("abcdef", "adf"));
  EXPECT_TRUE(algorithm::FindSubstr("abcdef", "abdef"));
}

TEST(FindSubstrTest, WrongOrderNotFound) {
  EXPECT_FALSE(algorithm::FindSubstr("abcdef", "fad"));
  EXPECT_FALSE(algorithm::FindSubstr("abc", "ca"));
}

TEST(FindSubstrTest, AbsentCharsNotFound) {
  EXPECT_FALSE(algorithm::FindSubstr("abcdef", "xyz"));
  EXPECT_FALSE(algorithm::FindSubstr("abcdef", "axe"));
  EXPECT_FALSE(algorithm::FindSubstr("aa", "aaa"));
}

TEST(FindSubstrTest, IsCaseSensitive) {
  EXPECT_FALSE(algorithm::FindSubstr("ABC", "abc"));
}

TEST(IsPalindromeTest, EmptyStringIsNotPalindrome) {
  EXPECT_FALSE(algorithm::IsPalindrome(""));
}

TEST(IsPalindromeTest, SingleCharIsPalindrome) {
  EXPECT_TRUE(algorithm::IsPalindrome("a"));
  EXPECT_TRUE(algorithm::IsPalindrome("x"));
}

TEST(IsPalindromeTest, TwoChars) {
  EXPECT_TRUE(algorithm::IsPalindrome("aa"));
  EXPECT_FALSE(algorithm::IsPalindrome("ab"));
}

TEST(IsPalindromeTest, EvenLengthPalindrome) {
  EXPECT_TRUE(algorithm::IsPalindrome("abba"));
  EXPECT_TRUE(algorithm::IsPalindrome("toottoot"));
}

TEST(IsPalindromeTest, OddLengthPalindrome) {
  EXPECT_TRUE(algorithm::IsPalindrome("aba"));
  EXPECT_TRUE(algorithm::IsPalindrome("abcba"));
  EXPECT_TRUE(algorithm::IsPalindrome("racecar"));
}

TEST(IsPalindromeTest, AllPairsMustMatchNotJustFirst) {
  EXPECT_FALSE(algorithm::IsPalindrome("abca"));
  EXPECT_FALSE(algorithm::IsPalindrome("racecars"));
  EXPECT_FALSE(algorithm::IsPalindrome("abxa"));
}

TEST(IsPalindromeTest, NotPalindrome) {
  EXPECT_FALSE(algorithm::IsPalindrome("abcd"));
  EXPECT_FALSE(algorithm::IsPalindrome("ab"));
  EXPECT_FALSE(algorithm::IsPalindrome("hello"));
}

TEST(IsPalindromeTest, CaseSensitive) {
  EXPECT_FALSE(algorithm::IsPalindrome("Aa"));
}

TEST(IsPalindromeTest, RepeatedCharIsPalindrome) {
  EXPECT_TRUE(algorithm::IsPalindrome("aaaa"));
}

TEST(SortByStackTest, EmptyVectorGivesEmptyStack) {
  std::vector<int> vec;
  std::stack<int> stack = algorithm::SortByStack(vec);
  EXPECT_TRUE(stack.empty());
  EXPECT_TRUE(vec.empty());
}

TEST(SortByStackTest, SingleElement) {
  std::vector<int> vec{7};
  std::stack<int> stack = algorithm::SortByStack(vec);
  EXPECT_EQ(PopAll(stack), std::vector<int>({7}));
}

TEST(SortByStackTest, PopsInDescendingOrder) {
  std::vector<int> vec{3, 1, 5, 2, 4};
  std::stack<int> stack = algorithm::SortByStack(vec);
  EXPECT_EQ(PopAll(stack), std::vector<int>({5, 4, 3, 2, 1}));
}

TEST(SortByStackTest, AlreadySortedAscending) {
  std::vector<int> vec{1, 2, 3, 4};
  std::stack<int> stack = algorithm::SortByStack(vec);
  EXPECT_EQ(PopAll(stack), std::vector<int>({4, 3, 2, 1}));
}

TEST(SortByStackTest, ReverseSortedInput) {
  std::vector<int> vec{5, 4, 3, 2, 1};
  std::stack<int> stack = algorithm::SortByStack(vec);
  EXPECT_EQ(PopAll(stack), std::vector<int>({5, 4, 3, 2, 1}));
}

TEST(SortByStackTest, DuplicatesAreKept) {
  std::vector<int> vec{3, 1, 3, 2, 1};
  std::stack<int> stack = algorithm::SortByStack(vec);
  EXPECT_EQ(PopAll(stack), std::vector<int>({3, 3, 2, 1, 1}));
}

TEST(SortByStackTest, InputVectorIsConsumed) {
  std::vector<int> vec{4, 2, 7, 1};
  algorithm::SortByStack(vec);
  EXPECT_TRUE(vec.empty());
}

TEST(SortByStackTest, WorksWithStrings) {
  std::vector<std::string> vec{"pear", "apple", "fig"};
  std::stack<std::string> stack = algorithm::SortByStack(vec);
  EXPECT_EQ(PopAll(stack), std::vector<std::string>({"pear", "fig", "apple"}));
}

TEST(ConnectingListsTest, KeepsSecondWhenFirstEmpty) {
  List<int> first;
  List<int> second;
  second.PushBack(1);
  second.PushBack(2);
  List<int>& merged = algorithm::ConnectingLists(first, second);
  EXPECT_EQ(&merged, &second);
  EXPECT_EQ(Chain(&merged.Front()), std::vector<int>({1, 2}));
}

TEST(ConnectingListsTest, KeepsFirstWhenSecondEmpty) {
  List<int> first;
  first.PushBack(1);
  first.PushBack(2);
  List<int> second;
  List<int>& merged = algorithm::ConnectingLists(first, second);
  EXPECT_EQ(&merged, &first);
  EXPECT_EQ(Chain(&merged.Front()), std::vector<int>({1, 2}));
}

TEST(ConnectingListsTest, BothEmpty) {
  List<int> first;
  List<int> second;
  List<int>& merged = algorithm::ConnectingLists(first, second);
  EXPECT_TRUE(merged.Empty());
}

TEST(ConnectingListsTest, MergesInterleavedSorted) {
  List<int> first;
  first.PushBack(1);
  first.PushBack(3);
  first.PushBack(5);
  List<int> second;
  second.PushBack(2);
  second.PushBack(4);
  List<int>& merged = algorithm::ConnectingLists(first, second);
  EXPECT_EQ(&merged, &first);
  EXPECT_EQ(Chain(&merged.Front()), std::vector<int>({1, 2, 3, 4, 5}));
  ReleaseOwnership(second);
}

TEST(ConnectingListsTest, MergesNonOverlappingRanges) {
  List<int> first;
  first.PushBack(1);
  first.PushBack(2);
  List<int> second;
  second.PushBack(3);
  second.PushBack(4);
  algorithm::ConnectingLists(first, second);
  EXPECT_EQ(Chain(&first.Front()), std::vector<int>({1, 2, 3, 4}));
  ReleaseOwnership(second);
}

TEST(ConnectingListsTest, SecondRangeSmallerMovesHead) {
  List<int> first;
  first.PushBack(3);
  first.PushBack(5);
  List<int> second;
  second.PushBack(1);
  second.PushBack(2);
  List<int>& merged = algorithm::ConnectingLists(first, second);
  EXPECT_EQ(&merged, &first);
  EXPECT_EQ(Chain(&merged.Front()), std::vector<int>({1, 2, 3, 5}));
  ReleaseOwnership(second);
}

TEST(ConnectingListsTest, DuplicateValuesAcrossLists) {
  List<int> first;
  first.PushBack(1);
  first.PushBack(3);
  List<int> second;
  second.PushBack(1);
  second.PushBack(2);
  algorithm::ConnectingLists(first, second);
  EXPECT_EQ(Chain(&first.Front()), std::vector<int>({1, 1, 2, 3}));
  ReleaseOwnership(second);
}

TEST(ConnectingListsTest, SingleElementEach) {
  List<int> first(2);
  List<int> second(1);
  algorithm::ConnectingLists(first, second);
  EXPECT_EQ(Chain(&first.Front()), std::vector<int>({1, 2}));
  ReleaseOwnership(second);
}

TEST(ConnectingListsTest, MergesLargerLists) {
  List<int> first;
  for (int v : {1, 4, 7, 10}) {
    first.PushBack(v);
  }
  List<int> second;
  for (int v : {2, 3, 5, 8, 9, 11}) {
    second.PushBack(v);
  }
  algorithm::ConnectingLists(first, second);
  EXPECT_EQ(Chain(&first.Front(), 64),
            std::vector<int>({1, 2, 3, 4, 5, 7, 8, 9, 10, 11}));
  ReleaseOwnership(second);
}
