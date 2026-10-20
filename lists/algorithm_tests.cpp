#include <stack>
#include <string>
#include <utility>
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

// Counts live instances to check that lists free every node exactly once.
struct Tracked {
  static inline int alive = 0;

  Tracked() { ++alive; }
  explicit Tracked(int v) : val(v) { ++alive; }
  Tracked(const Tracked& other) : val(other.val) { ++alive; }
  Tracked& operator=(const Tracked&) = default;
  ~Tracked() { --alive; }

  bool operator==(const Tracked& other) const { return val == other.val; }
  bool operator<(const Tracked& other) const { return val < other.val; }
  bool operator<=(const Tracked& other) const { return val <= other.val; }

  int val = 0;
};

}  // namespace

namespace {

// ConnectingLists splices nodes of both lists into one chain, but leaves
// second.head_ pointing at a node that now belongs to first. With the new
// ~List this means a double delete, so tests must detach `second` from the
// shared chain before it goes out of scope.
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

// RAII: closes a ring for the duration of a test and reopens the chain on
// scope exit, so ~List never recurses through a cycle even if an assertion
// aborts the test body early.
class ScopedRing {
 public:
  ScopedRing(Node<int>* tail, Node<int>* target)
      : tail_(tail), saved_(tail->next) {
    tail_->next = target;
  }
  ~ScopedRing() { tail_->next = saved_; }

  ScopedRing(const ScopedRing&) = delete;
  ScopedRing& operator=(const ScopedRing&) = delete;

  void Break() { tail_->next = saved_; }

 private:
  Node<int>* tail_;
  Node<int>* saved_;
};

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
  ScopedRing closer(node, node);
  EXPECT_TRUE(algorithm::IsRing(lst));
}

TEST(IsRingTest, ClosedTailIsRing) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  lst.PushBack(3);
  Node<int>* tail = Tail(lst);
  ScopedRing closer(tail, &lst.Front());
  EXPECT_TRUE(algorithm::IsRing(lst));
  closer.Break();
  EXPECT_FALSE(algorithm::IsRing(lst));
}

TEST(IsRingTest, RhoShapedRingIsRing) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  lst.PushBack(3);
  lst.PushBack(4);
  Node<int>* tail = Tail(lst);
  ScopedRing closer(tail, lst.Front().next);
  EXPECT_TRUE(algorithm::IsRing(lst));
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

TEST(ReverseTest, ResultIsStillOwnedByTheList) {
  // Reverse only permutes links, so ~List must free every node exactly once
  // (ASan turns a lost or doubled link into a crash here).
  const int base = Tracked::alive;
  {
    List<Tracked> lst(Tracked{0});
    lst.PushBack(Tracked{1});
    lst.PushBack(Tracked{2});
    Node<Tracked>* old_head = &lst.Front();
    algorithm::Reverse(lst);
    ASSERT_EQ(Chain(&lst.Front()),
              std::vector<Tracked>({Tracked{2}, Tracked{1}, Tracked{0}}));
    ASSERT_EQ(Tail(lst), old_head);  // same nodes, permuted links
  }
  EXPECT_EQ(Tracked::alive, base);
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

TEST(DeleteElemTest, DeletedNodeIsFreedExactlyOnce) {
  // ~Node is recursive and Erase unlinks before deleting, so each node must
  // die exactly once; ASan aborts on the second delete otherwise.
  const int base = Tracked::alive;
  {
    List<Tracked> lst(Tracked{1});   // sentinel + node(1)
    lst.PushBack(Tracked{2});        // + node(2)
    EXPECT_EQ(Tracked::alive, base + 3);
    lst.Erase(Tracked{1});           // frees node(1) only
    EXPECT_EQ(Tracked::alive, base + 2);
  }
  EXPECT_EQ(Tracked::alive, base);
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
  // Now both lists own the merged chain, ~List would delete it twice.
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

TEST(ConnectingListsTest, MergedChainFreedExactlyOnce) {
  // 3 + 3 nodes; after the merge they all live in first's chain and must
  // be deleted exactly once (ASan aborts on the second delete otherwise).
  const int base = Tracked::alive;
  {
    List<Tracked> first(Tracked{1});
    first.PushBack(Tracked{3});
    List<Tracked> second(Tracked{2});
    second.PushBack(Tracked{4});
    algorithm::ConnectingLists(first, second);
    ReleaseOwnership(second);
  }
  EXPECT_EQ(Tracked::alive, base);
}

// ============================================================================
// Algorithms must work on moved-into lists (~List now owns the chain).
// Note: the move operations do not transfer sz_, so these tests inspect the
// chain through Front()/links instead of Empty()/ToVector().
// ============================================================================

TEST(MoveTest, ReverseAfterMoveConstructor) {
  List<int> src;
  src.PushBack(1);
  src.PushBack(2);
  src.PushBack(3);

  List<int> dst(std::move(src));
  algorithm::Reverse(dst);

  EXPECT_EQ(Chain(&dst.Front()), std::vector<int>({3, 2, 1}));
}

TEST(MoveTest, IsRingAfterMoveConstructor) {
  List<int> src;
  src.PushBack(1);
  src.PushBack(2);
  List<int> dst(std::move(src));

  Node<int>* tail = Tail(dst);
  ScopedRing closer(tail, &dst.Front());
  EXPECT_TRUE(algorithm::IsRing(dst));
}

TEST(MoveTest, MoveAfterMergeKeepsSingleOwner) {
  const int base = Tracked::alive;
  {
    List<Tracked> first(Tracked{1});
    first.PushBack(Tracked{3});
    List<Tracked> second(Tracked{2});
    second.PushBack(Tracked{4});
    algorithm::ConnectingLists(first, second);
    ReleaseOwnership(second);

    // The whole merged chain lives only in first now; hand it to another
    // list. Exactly one owner must remain, so ~List frees four nodes once.
    List<Tracked> merged(std::move(first));
    EXPECT_EQ(Chain(&merged.Front(), 8).size(), size_t{4});
  }
  EXPECT_EQ(Tracked::alive, base);
}

TEST(MoveTest, MoveAssignmentFreesOldChainBeforeTakingNewOne) {
  List<int> dst;
  dst.PushBack(10);
  dst.PushBack(20);

  List<int> src;
  src.PushBack(1);
  src.PushBack(2);
  src.PushBack(3);

  dst = std::move(src);

  EXPECT_EQ(Chain(&dst.Front()), std::vector<int>({1, 2, 3}));
}

TEST(MoveTest, EraseAfterMoveAssignment) {
  List<int> dst;
  List<int> src;
  src.PushBack(1);
  src.PushBack(2);
  dst = std::move(src);

  Node<int>* next = algorithm::DeleteElem(dst, 1);
  ASSERT_NE(next, nullptr);
  EXPECT_EQ(next->val, 2);
  EXPECT_EQ(Chain(&dst.Front()), std::vector<int>({2}));
}
