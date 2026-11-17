#include <algorithm>
#include <cstdint>
#include <optional>
#include <queue>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "completeness_checking.hpp"
#include "bst_find.hpp"
#include "find_min.hpp"
#include "heap.hpp"
#include "merge_arrays.hpp"
#include "mirror_tree.hpp"
#include "set_balance_factor.hpp"
#include "tree.hpp"

namespace {

using algorithm::IsCompleteness;

// Builds a tree from a level-order array (heap-style):
// node i is a child of node (i-1) / 2, left children have odd indices.
// std::nullopt means the node is absent at that position.
template <typename T>
tree::Tree<T> BuildTree(const std::vector<std::optional<T>>& nodes) {
  if (nodes.empty() || !nodes[0].has_value()) {
    return tree::Tree<T>{};
  }

  tree::Tree<T> result{nodes[0].value()};

  std::vector<tree::Node<T>*> ptrs(nodes.size(), nullptr);
  ptrs[0] = result.GetRoot();

  for (size_t i = 1; i < nodes.size(); ++i) {
    if (!nodes[i].has_value()) {
      continue;
    }
    tree::Node<T>* parent = ptrs[(i - 1) / 2];
    EXPECT_NE(parent, nullptr);  // tests never attach children to a missing node
    if (parent == nullptr) {
      continue;
    }
    auto* child = new tree::Node<T>{nodes[i].value()};
    ptrs[i] = child;
    if (i % 2 == 1) {
      parent->left = child;
    } else {
      parent->right = child;
    }
  }

  return result;
}

// ===== Complete trees: the last level is filled from left to right =====

TEST(IsCompleteness, EmptyTree) {
  // By convention, an empty tree is considered complete
  auto tree = BuildTree<int>({});
  EXPECT_TRUE(IsCompleteness(tree));
}

TEST(IsCompleteness, SingleNode) {
  auto tree = BuildTree<int>({1});
  EXPECT_TRUE(IsCompleteness(tree));
}

TEST(IsCompleteness, RootWithLeftChildOnly) {
  auto tree = BuildTree<int>({1, 2});
  EXPECT_TRUE(IsCompleteness(tree));
}

TEST(IsCompleteness, RootWithBothChildren) {
  auto tree = BuildTree<int>({1, 2, 3});
  EXPECT_TRUE(IsCompleteness(tree));
}

TEST(IsCompleteness, FourNodesLastLevelLeftAligned) {
  auto tree = BuildTree<int>({1, 2, 3, 4});
  EXPECT_TRUE(IsCompleteness(tree));
}

TEST(IsCompleteness, FullTreeOfHeightThree) {
  auto tree = BuildTree<int>({1, 2, 3, 4, 5, 6, 7});
  EXPECT_TRUE(IsCompleteness(tree));
}

TEST(IsCompleteness, SixNodesLastLevelMissingRightmost) {
  auto tree = BuildTree<int>({1, 2, 3, 4, 5, 6});
  EXPECT_TRUE(IsCompleteness(tree));
}

TEST(IsCompleteness, TenNodesPartiallyFilledFourthLevel) {
  // Levels 1-3 are full, level 4 holds 7, 8, 9 aligned to the left
  auto tree = BuildTree<int>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
  EXPECT_TRUE(IsCompleteness(tree));
}

TEST(IsCompleteness, FourteenOfFifteenNodes) {
  std::vector<std::optional<int>> nodes;
  for (int i = 1; i <= 14; ++i) {
    nodes.push_back(i);
  }
  auto tree = BuildTree(nodes);
  EXPECT_TRUE(IsCompleteness(tree));
}

// ===== Incomplete trees =====

TEST(IsCompleteness, RootWithRightChildOnly) {
  auto tree = BuildTree<int>({1, std::nullopt, 2});
  EXPECT_FALSE(IsCompleteness(tree));
}

TEST(IsCompleteness, RightGrandchildWithoutLeftSibling) {
  // Index 4 = right child of the left son, index 3 is empty
  auto tree = BuildTree<int>({1, 2, 3, std::nullopt, 4});
  EXPECT_FALSE(IsCompleteness(tree));
}

TEST(IsCompleteness, LeftChildOfRootHasOnlyRightChild) {
  // Index 3 is skipped, index 4 is present
  auto tree = BuildTree<int>({1, 2, 3, std::nullopt, 5});
  EXPECT_FALSE(IsCompleteness(tree));
}

TEST(IsCompleteness, HoleInLastLevel) {
  // Last level: 4, 5, ... 7 — a hole at position 6 (index 5)
  auto tree = BuildTree<int>({1, 2, 3, 4, 5, std::nullopt, 7});
  EXPECT_FALSE(IsCompleteness(tree));
}

TEST(IsCompleteness, LeftOnlyParentFollowedByParentWithChildren) {
  // Node 2 has only a left child (index 4, LR, is empty),
  // node 3 has both children — the last level has a hole at LR
  auto tree = BuildTree<int>({1, 2, 3, 4, std::nullopt, 5, 6});
  EXPECT_FALSE(IsCompleteness(tree));
}

TEST(IsCompleteness, LastLevelStartsNotFromLeftmost) {
  // The first three levels are full, level 4 has a single node under RR
  // (index 14), positions 7..13 are empty
  auto tree = BuildTree<int>({1, 2, 3, 4, 5, 6, 7,
                         std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                         std::nullopt, std::nullopt, std::nullopt, 8});
  EXPECT_FALSE(IsCompleteness(tree));
}

TEST(IsCompleteness, DeepLeftChildWithMissingSiblings) {
  // The only child of the root (the left one) has both children
  // while the right son of the root is missing
  auto tree = BuildTree<int>({1, 2, std::nullopt, 4, 5});
  EXPECT_FALSE(IsCompleteness(tree));
}

// ===== Another value type =====

TEST(IsCompleteness, WorksForStrings) {
  auto tree = BuildTree<std::string>({"a", "b", "c", "d"});
  EXPECT_TRUE(IsCompleteness(tree));

  auto broken = BuildTree<std::string>({"a", "b", "c", std::nullopt, "e"});
  EXPECT_FALSE(IsCompleteness(broken));
}

// ============================================================================
// Task 2: MergeArrays — merge K sorted arrays into one sorted array
// ============================================================================

namespace {

using algorithm::MergeArrays;

// Helper: the result must be sorted ascending and contain exactly
// the same elements as the concatenation of the inputs (multiset equality).
template <typename T>
void CheckMerge(const std::vector<std::vector<T>>& arrays) {
  std::vector<T> expected;
  for (const auto& a : arrays) {
    expected.insert(expected.end(), a.begin(), a.end());
  }
  std::sort(expected.begin(), expected.end());

  const auto merged = MergeArrays(arrays);

  EXPECT_EQ(merged.size(), expected.size());
  EXPECT_TRUE(std::is_sorted(merged.begin(), merged.end()));
  EXPECT_EQ(merged, expected);
}

TEST(MergeArrays, TwoBasicSortedArrays) {
  auto merged = MergeArrays<int>({{1, 4, 7}, {2, 5, 8}});
  EXPECT_EQ(merged, (std::vector<int>{1, 2, 4, 5, 7, 8}));
}

TEST(MergeArrays, ThreeArraysOfDifferentLengths) {
  auto merged = MergeArrays<int>({{1, 10, 20}, {2, 3}, {4, 5, 6, 100}});
  EXPECT_EQ(merged, (std::vector<int>{1, 2, 3, 4, 5, 6, 10, 20, 100}));
}

TEST(MergeArrays, NoArrays) {
  auto merged = MergeArrays<int>({});
  EXPECT_TRUE(merged.empty());
}

TEST(MergeArrays, OnlyEmptyArrays) {
  auto merged = MergeArrays<int>({{}, {}, {}});
  EXPECT_TRUE(merged.empty());
}

TEST(MergeArrays, EmptyArraysMixedWithRealOnes) {
  auto merged = MergeArrays<int>({{}, {5}, {}, {1, 3}});
  EXPECT_EQ(merged, (std::vector<int>{1, 3, 5}));
}

TEST(MergeArrays, SingleArray) {
  auto merged = MergeArrays<int>({{1, 2, 3}});
  EXPECT_EQ(merged, (std::vector<int>{1, 2, 3}));
}

TEST(MergeArrays, Duplicates) {
  auto merged = MergeArrays<int>({{1, 1, 5}, {1, 2, 5}, {5}});
  EXPECT_EQ(merged, (std::vector<int>{1, 1, 1, 2, 5, 5, 5}));
}

TEST(MergeArrays, NegativeAndEqualBorders) {
  auto merged = MergeArrays<int>({{-5, -1, 0}, {-3, 0, 2}, {0}});
  EXPECT_EQ(merged, (std::vector<int>{-5, -3, -1, 0, 0, 0, 2}));
}

TEST(MergeArrays, SingleElementArrays) {
  auto merged = MergeArrays<int>({{5}, {1}, {3}});
  EXPECT_EQ(merged, (std::vector<int>{1, 3, 5}));
}

TEST(MergeArrays, Strings) {
  auto merged = MergeArrays<std::string>(
      {{"apple", "pear"}, {"banana"}, {"cherry", "fig", "melon"}});
  EXPECT_EQ(merged,
            (std::vector<std::string>{"apple", "banana", "cherry", "fig",
                                      "melon", "pear"}));
}

TEST(MergeArrays, ManyArraysCheckProperty) {
  // 50 arrays, each sorted, values overlapping heavily
  std::vector<std::vector<int>> arrays;
  for (int k = 0; k < 50; ++k) {
    std::vector<int> a;
    for (int v = k; v < 1000; v += 50) {
      a.push_back(v);
    }
    arrays.push_back(std::move(a));
  }
  CheckMerge(arrays);
}

TEST(MergeArrays, LongArraysCheckProperty) {
  // Two long arrays merged alternately: O(N log N) output must be sorted
  std::vector<int> evens;
  std::vector<int> odds;
  for (int i = 0; i < 10000; ++i) {
    evens.push_back(2 * i);
    odds.push_back(2 * i + 1);
  }
  std::vector<std::vector<int>> input{evens, odds};
  CheckMerge(input);
}

}  // namespace

// ============================================================================
// Task 3: HeapFindMin — k-th smallest element via a heap (k is 1-based)
// ============================================================================

namespace {

using algorithm::HeapFindMin;

TEST(HeapFindMin, KthSmallestBasic) {
  std::vector<int> nums{7, 3, 9, 1, 5};
  EXPECT_EQ(HeapFindMin(nums, 1), 1);
  EXPECT_EQ(HeapFindMin(nums, 2), 3);
  EXPECT_EQ(HeapFindMin(nums, 3), 5);
  EXPECT_EQ(HeapFindMin(nums, 4), 7);
  EXPECT_EQ(HeapFindMin(nums, 5), 9);
}

TEST(HeapFindMin, UnsortedWithNegatives) {
  std::vector<int> nums{4, -2, 10, 0, -7, 3};
  EXPECT_EQ(HeapFindMin(nums, 1), -7);
  EXPECT_EQ(HeapFindMin(nums, 4), 3);
  EXPECT_EQ(HeapFindMin(nums, 6), 10);
}

TEST(HeapFindMin, SingleElement) {
  std::vector<int> nums{42};
  EXPECT_EQ(HeapFindMin(nums, 1), 42);
}

TEST(HeapFindMin, Duplicates) {
  std::vector<int> nums{5, 1, 5, 2, 1};
  EXPECT_EQ(HeapFindMin(nums, 2), 1);
  EXPECT_EQ(HeapFindMin(nums, 3), 2);
  EXPECT_EQ(HeapFindMin(nums, 4), 5);
  EXPECT_EQ(HeapFindMin(nums, 5), 5);
}

TEST(HeapFindMin, AlreadySorted) {
  std::vector<int> nums{1, 2, 3, 4, 5};
  EXPECT_EQ(HeapFindMin(nums, 3), 3);
}

TEST(HeapFindMin, ReverseSorted) {
  std::vector<int> nums{5, 4, 3, 2, 1};
  EXPECT_EQ(HeapFindMin(nums, 2), 2);
  EXPECT_EQ(HeapFindMin(nums, 5), 5);
}

TEST(HeapFindMin, AllEqual) {
  std::vector<int> nums{3, 3, 3, 3};
  EXPECT_EQ(HeapFindMin(nums, 1), 3);
  EXPECT_EQ(HeapFindMin(nums, 4), 3);
}

TEST(HeapFindMin, Strings) {
  std::vector<std::string> words{"pear", "apple", "fig", "cherry"};
  EXPECT_EQ(HeapFindMin(words, 2), "cherry");
  EXPECT_EQ(HeapFindMin(words, 4), "pear");
}

TEST(HeapFindMin, ThrowsWhenKTooLarge) {
  std::vector<int> nums{1, 2, 3};
  EXPECT_THROW(HeapFindMin(nums, 4), std::runtime_error);
}

TEST(HeapFindMin, ThrowsOnEmptyArray) {
  std::vector<int> nums;
  EXPECT_THROW(HeapFindMin(nums, 1), std::runtime_error);
}

TEST(HeapFindMin, LargeArrayKthValue) {
  // nums[i] = (3*i + 7) % 101 over 5000 elements; compare with the sorted copy
  std::vector<int> nums;
  for (int i = 0; i < 5000; ++i) {
    nums.push_back((3 * i + 7) % 101);
  }
  std::vector<int> sorted_nums = nums;
  std::sort(sorted_nums.begin(), sorted_nums.end());

  EXPECT_EQ(HeapFindMin(nums, 1), sorted_nums[0]);
  EXPECT_EQ(HeapFindMin(nums, 1000), sorted_nums[999]);
  EXPECT_EQ(HeapFindMin(nums, 5000), sorted_nums[4999]);
}

// ============================================================================
// Task 4: heap::Heap — direct tests of the heap itself
// ============================================================================

namespace {

// Pops everything until the heap is empty.
template <typename T, typename C>
std::vector<T> Drain(heap::Heap<T, C>& h) {
  std::vector<T> out;
  while (!h.Empty()) {
    out.push_back(h.Pop());
  }
  return out;
}

TEST(Heap, EmptyOnCreation) {
  heap::Heap<int> h;
  EXPECT_TRUE(h.Empty());
  EXPECT_THROW(h.Pop(), std::runtime_error);
}

TEST(Heap, InsertAndTopValue) {
  heap::Heap<int> h;
  h.Insert(5);
  h.Insert(2);
  h.Insert(8);
  // GetTop() returns the root INDEX; the value lives in the array
  EXPECT_EQ(h.GetHeap()[h.GetTop()], 2);
}

TEST(Heap, PopOrderIsAscendingForLess) {
  heap::Heap<int> h;
  for (int v : {5, 1, 3, 1, 9}) {
    h.Insert(v);
  }
  EXPECT_EQ(Drain(h), (std::vector<int>{1, 1, 3, 5, 9}));
  EXPECT_TRUE(h.Empty());
}

TEST(Heap, PopThrowsWhenExhausted) {
  heap::Heap<int> h;
  h.Insert(7);
  EXPECT_EQ(h.Pop(), 7);
  EXPECT_THROW(h.Pop(), std::runtime_error);
}

TEST(Heap, BuildHeapProducesSortedPops) {
  auto h = heap::Heap<int, std::less<int>>::BuildHeap({9, 3, 7, 1, 20, 3});
  EXPECT_EQ(Drain(h), (std::vector<int>{1, 3, 3, 7, 9, 20}));
}

TEST(Heap, BuildHeapRespectsHeapProperty) {
  auto h = heap::Heap<int, std::less<int>>::BuildHeap({42, 7, 19, 0, 33, 8});
  const auto arr = h.GetHeap();
  ASSERT_EQ(arr.size(), 6u);
  // every parent must be <= its children
  for (size_t i = 1; i < arr.size(); ++i) {
    EXPECT_LE(arr[(i - 1) / 2], arr[i]) << "at index " << i;
  }
}

TEST(Heap, BuildHeapFromEmptyVector) {
  auto h = heap::Heap<int, std::less<int>>::BuildHeap({});
  EXPECT_TRUE(h.Empty());
  EXPECT_THROW(h.Pop(), std::runtime_error);
}

TEST(Heap, MaxHeapWithGreater) {
  auto h = heap::Heap<int, std::greater<int>>::BuildHeap({4, 1, 8, 4});
  EXPECT_EQ(Drain(h), (std::vector<int>{8, 4, 4, 1}));
}

TEST(Heap, StringsOrdering) {
  auto h = heap::Heap<std::string, std::less<std::string>>::BuildHeap(
      {"pear", "apple", "fig"});
  EXPECT_EQ(Drain(h), (std::vector<std::string>{"apple", "fig", "pear"}));
}

TEST(Heap, IndexHelpers) {
  auto h = heap::Heap<int, std::less<int>>::BuildHeap({1, 2, 3, 4, 5, 6, 7});
  const size_t sz = h.GetHeap().size();
  EXPECT_EQ(h.GetTop(), 0u);
  EXPECT_EQ(h.GetLeft(0), 1u);
  EXPECT_EQ(h.GetRight(0), 2u);
  EXPECT_EQ(h.GetLeft(3), sz);   // node 3 is a leaf: no left child
  EXPECT_EQ(h.GetRight(3), sz);
  EXPECT_EQ(h.GetParentInd(0), 0u);  // root has no parent
  EXPECT_EQ(h.GetParentInd(5), 2u);
}

TEST(Heap, InterleavedInsertAndPopAgainstPriorityQueue) {
  // Differential test against std::priority_queue (min-heap oracle)
  std::mt19937 rng(123);
  heap::Heap<int, std::less<int>> h;
  std::priority_queue<int, std::vector<int>, std::greater<int>> oracle;

  for (int step = 0; step < 2000; ++step) {
    const bool do_insert = oracle.empty() || (rng() % 100 < 55);
    if (do_insert) {
      const int v = static_cast<int>(rng() % 1000);
      h.Insert(v);
      oracle.push(v);
    } else {
      EXPECT_EQ(h.Pop(), oracle.top());
      oracle.pop();
    }

    if (!oracle.empty()) {
      EXPECT_EQ(h.GetHeap()[h.GetTop()], oracle.top());
    }
  }

  while (!oracle.empty()) {
    EXPECT_EQ(h.Pop(), oracle.top());
    oracle.pop();
  }
  EXPECT_TRUE(h.Empty());
}

// ============================================================================
// Task 5: BSTFindMin — k-th smallest element in a BST via in-order traversal
// (k is 1-based)
// ============================================================================

namespace {

using algorithm::BSTFindMin;

// Inserts a value into the tree rooted at node following BST rules
// (strictly less goes left, otherwise right). The tree owns the node.
template <typename T>
void InsertBstNode(tree::Node<T>* root, const T& val) {
  tree::Node<T>* cur = root;
  while (true) {
    if (val < cur->val) {
      if (!cur->left) {
        cur->left = new tree::Node<T>{val};
        return;
      }
      cur = cur->left;
    } else {
      if (!cur->right) {
        cur->right = new tree::Node<T>{val};
        return;
      }
      cur = cur->right;
    }
  }
}

// Builds a BST by inserting values in the given order.
// Precondition: values is non-empty.
template <typename T>
tree::Tree<T> BuildBst(const std::vector<T>& values) {
  tree::Tree<T> result{values[0]};
  for (size_t i = 1; i < values.size(); ++i) {
    InsertBstNode(result.GetRoot(), values[i]);
  }
  return result;
}

TEST(BSTFindMin, BalancedTreeAllPositions) {
  // BST root 4, levels {2,6}, {1,3,5,7} — in-order is 1..7
  auto bst = BuildBst<int>({4, 2, 6, 1, 3, 5, 7});
  EXPECT_EQ(BSTFindMin(bst, 1), 1);
  EXPECT_EQ(BSTFindMin(bst, 2), 2);
  EXPECT_EQ(BSTFindMin(bst, 3), 3);
  EXPECT_EQ(BSTFindMin(bst, 4), 4);
  EXPECT_EQ(BSTFindMin(bst, 5), 5);
  EXPECT_EQ(BSTFindMin(bst, 6), 6);
  EXPECT_EQ(BSTFindMin(bst, 7), 7);
}

TEST(BSTFindMin, SingleNode) {
  auto bst = BuildBst<int>({42});
  EXPECT_EQ(BSTFindMin(bst, 1), 42);
}

TEST(BSTFindMin, LeftSkewedChain) {
  // 5 -> 4 -> 3 -> 2 -> 1 (everything goes left)
  auto bst = BuildBst<int>({5, 4, 3, 2, 1});
  EXPECT_EQ(BSTFindMin(bst, 1), 1);
  EXPECT_EQ(BSTFindMin(bst, 5), 5);
  EXPECT_EQ(BSTFindMin(bst, 3), 3);
}

TEST(BSTFindMin, RightSkewedChain) {
  auto bst = BuildBst<int>({1, 2, 3, 4, 5});
  EXPECT_EQ(BSTFindMin(bst, 2), 2);
  EXPECT_EQ(BSTFindMin(bst, 5), 5);
}

TEST(BSTFindMin, MinAndMaxExtremes) {
  auto bst = BuildBst<int>({50, 25, 75, 10, 30, 60, 90, 5});
  EXPECT_EQ(BSTFindMin(bst, 1), 5);          // minimum
  EXPECT_EQ(BSTFindMin(bst, 8), 90);         // maximum (8 nodes)
  EXPECT_EQ(BSTFindMin(bst, 4), 30);
}

TEST(BSTFindMin, NegativeValues) {
  auto bst = BuildBst<int>({0, -10, 10, -5, -15});
  EXPECT_EQ(BSTFindMin(bst, 1), -15);
  EXPECT_EQ(BSTFindMin(bst, 2), -10);
  EXPECT_EQ(BSTFindMin(bst, 3), -5);
  EXPECT_EQ(BSTFindMin(bst, 5), 10);
}

TEST(BSTFindMin, Strings) {
  auto bst = BuildBst<std::string>({"m", "d", "t", "a", "g", "p"});
  // in-order: a d g m p t
  EXPECT_EQ(BSTFindMin(bst, 1), "a");
  EXPECT_EQ(BSTFindMin(bst, 3), "g");
  EXPECT_EQ(BSTFindMin(bst, 6), "t");
}

TEST(BSTFindMin, LargerTreeAgainstSortedCopy) {
  std::vector<int> values;
  for (int i = 0; i < 200; ++i) {
    values.push_back((i * 37) % 400);  // pseudo-random insertion order
  }
  auto bst = BuildBst(values);

  std::vector<int> sorted_values = values;
  std::sort(sorted_values.begin(), sorted_values.end());
  const size_t n = sorted_values.size();

  EXPECT_EQ(BSTFindMin(bst, 1), sorted_values[0]);
  EXPECT_EQ(BSTFindMin(bst, n / 2), sorted_values[n / 2 - 1]);
  EXPECT_EQ(BSTFindMin(bst, n), sorted_values[n - 1]);
}

// ============================================================================
// Task 6: SetBalanceFactor — compute height and fill balance_factor
// (left_height - right_height) for every node; returns the tree height.
// ============================================================================

namespace {

using algorithm::SetBalanceFactor;

// Independent reference implementations for checking
template <typename T>
size_t RefHeight(const tree::Node<T>* node) {
  if (!node) {
    return 0;
  }
  return 1 + std::max(RefHeight(node->left), RefHeight(node->right));
}

template <typename T>
int RefBalance(const tree::Node<T>* node) {
  if (!node) {
    return 0;
  }
  return static_cast<int>(RefHeight(node->left)) -
         static_cast<int>(RefHeight(node->right));
}

// Walks the whole tree and checks every stored balance_factor
template <typename T>
void CheckAllBalanceFactors(const tree::Node<T>* node) {
  if (!node) {
    return;
  }
  EXPECT_EQ(node->balance_factor, RefBalance(node));
  CheckAllBalanceFactors(node->left);
  CheckAllBalanceFactors(node->right);
}

TEST(SetBalanceFactor, NullptrHasZeroHeight) {
  EXPECT_EQ(SetBalanceFactor<int>(nullptr), 0u);
}

TEST(SetBalanceFactor, SingleNode) {
  auto tree = BuildBst<int>({5});
  EXPECT_EQ(SetBalanceFactor(tree.GetRoot()), 1u);
  EXPECT_EQ(tree.GetRoot()->balance_factor, 0);
}

TEST(SetBalanceFactor, LeftOnlyChild) {
  auto tree = BuildBst<int>({5, 3});
  EXPECT_EQ(SetBalanceFactor(tree.GetRoot()), 2u);
  EXPECT_EQ(tree.GetRoot()->balance_factor, 1);
  EXPECT_EQ(tree.GetRoot()->left->balance_factor, 0);
}

TEST(SetBalanceFactor, RightOnlyChild) {
  auto tree = BuildBst<int>({5, 8});
  EXPECT_EQ(SetBalanceFactor(tree.GetRoot()), 2u);
  EXPECT_EQ(tree.GetRoot()->balance_factor, -1);
  EXPECT_EQ(tree.GetRoot()->right->balance_factor, 0);
}

TEST(SetBalanceFactor, PerfectTreeAllZeros) {
  auto tree = BuildBst<int>({4, 2, 6, 1, 3, 5, 7});
  auto* root = tree.GetRoot();
  EXPECT_EQ(SetBalanceFactor(root), 3u);
  CheckAllBalanceFactors(root);  // every node must be 0 via the reference
  EXPECT_EQ(root->balance_factor, 0);
}

TEST(SetBalanceFactor, LeftSkewedChain) {
  // 5 -> 4 -> 3 -> 2 -> 1, every node leans fully left
  auto tree = BuildBst<int>({5, 4, 3, 2, 1});
  auto* root = tree.GetRoot();
  EXPECT_EQ(SetBalanceFactor(root), 5u);

  const tree::Node<int>* node = root;
  for (int expected_bf = 4; expected_bf >= 0; --expected_bf) {
    EXPECT_EQ(node->balance_factor, expected_bf);
    node = node->left;
  }
}

TEST(SetBalanceFactor, RightSkewedChain) {
  auto tree = BuildBst<int>({1, 2, 3});
  auto* root = tree.GetRoot();
  EXPECT_EQ(SetBalanceFactor(root), 3u);
  EXPECT_EQ(root->balance_factor, -2);
  EXPECT_EQ(root->right->balance_factor, -1);
  EXPECT_EQ(root->right->right->balance_factor, 0);
}

TEST(SetBalanceFactor, MixedTreeEveryNodeChecked) {
  auto tree = BuildBst<int>({10, 5, 15, 1, 20, 3, 17});
  auto* root = tree.GetRoot();
  EXPECT_EQ(SetBalanceFactor(root), 4u);
  EXPECT_EQ(root->balance_factor, 0);          // h(5)=3, h(15)=3
  EXPECT_EQ(root->left->balance_factor, 2);    // h(1)=2, h(empty)=0
  EXPECT_EQ(root->left->left->balance_factor, -1);  // node 1: only right child 3
  EXPECT_EQ(root->right->balance_factor, -2);  // node 15: only right child 20
  CheckAllBalanceFactors(root);                // full check against the reference
}

TEST(SetBalanceFactor, RepeatedCallIsIdempotent) {
  auto tree = BuildBst<int>({8, 3, 12, 1, 6, 2});
  auto* root = tree.GetRoot();
  EXPECT_EQ(SetBalanceFactor(root), RefHeight(root));
  const int root_bf = root->balance_factor;
  EXPECT_EQ(SetBalanceFactor(root), RefHeight(root));
  EXPECT_EQ(root->balance_factor, root_bf);
  CheckAllBalanceFactors(root);
}

TEST(SetBalanceFactor, Strings) {
  auto tree = BuildBst<std::string>({"m", "d", "t", "a"});
  auto* root = tree.GetRoot();
  EXPECT_EQ(SetBalanceFactor(root), 3u);
  EXPECT_EQ(root->balance_factor, 1);   // left h=2, right h=1
  EXPECT_EQ(root->left->balance_factor, 1);
  EXPECT_EQ(root->right->balance_factor, 0);
}

TEST(SetBalanceFactor, TreeWithoutRoot) {
  tree::Tree<int> empty_tree;  // default-constructed: GetRoot() == nullptr
  EXPECT_EQ(SetBalanceFactor(empty_tree.GetRoot()), 0u);
}

TEST(SetBalanceFactor, RightChainOfFour) {
  // 1 -> 2 -> 3 -> 4 to the right: |balance_factor| grows to 3
  auto tree = BuildBst<int>({1, 2, 3, 4});
  auto* root = tree.GetRoot();
  EXPECT_EQ(SetBalanceFactor(root), 4u);

  const tree::Node<int>* node = root;
  for (int expected_bf = -3; expected_bf <= 0; ++expected_bf) {
    EXPECT_EQ(node->balance_factor, expected_bf);
    node = node->right;
  }
}

TEST(SetBalanceFactor, ZigzagNodeBalancedButNotFull) {
  // 10: left 5 (leaf), right 15 with a left child 12
  // bf(12)=0, bf(15)=1, bf(5)=0, bf(10)=1-2=-1 while the tree "looks" balanced
  auto tree = BuildBst<int>({10, 5, 15, 12});
  auto* root = tree.GetRoot();
  EXPECT_EQ(SetBalanceFactor(root), 3u);
  EXPECT_EQ(root->balance_factor, -1);
  EXPECT_EQ(root->left->balance_factor, 0);
  EXPECT_EQ(root->right->balance_factor, 1);
  EXPECT_EQ(root->right->left->balance_factor, 0);
}

TEST(SetBalanceFactor, MixedSignsInOneTree) {
  // some internal nodes lean left, others right
  auto tree = BuildBst<int>({50, 30, 70, 20, 40, 80, 10, 75});
  auto* root = tree.GetRoot();
  EXPECT_EQ(SetBalanceFactor(root), 4u);
  EXPECT_EQ(root->balance_factor, 0);
  EXPECT_EQ(root->left->balance_factor, 1);   // 30: left h=2, right h=1
  EXPECT_EQ(root->right->balance_factor, -2); // 70: left empty, right h=2
  CheckAllBalanceFactors(root);
}

TEST(SetBalanceFactor, WorksOnNonBstShapes) {
  // The algorithm uses only the structure, not BST ordering:
  // root 1, left 2 (with children 4 and 5), right 3 (leaf) — level-order build
  auto tree = BuildTree<int>({1, 2, 3, 4, 5});
  auto* root = tree.GetRoot();
  EXPECT_EQ(SetBalanceFactor(root), 3u);      // root->2->4 chain
  EXPECT_EQ(root->balance_factor, 1);         // left h=2, right h=1
  EXPECT_EQ(root->left->balance_factor, 0);   // two leaf children
  EXPECT_EQ(root->right->balance_factor, 0);
}

TEST(SetBalanceFactor, LargeRandomBstMatchesReference) {
  std::mt19937 rng(2026);
  std::vector<int> values;
  for (int i = 0; i < 500; ++i) {
    values.push_back(static_cast<int>(rng()));
  }
  auto tree = BuildBst(values);
  auto* root = tree.GetRoot();

  const size_t height = SetBalanceFactor(root);
  EXPECT_EQ(height, RefHeight(root));
  CheckAllBalanceFactors(root);
}

TEST(SetBalanceFactor, ReturnedHeightEqualsMaxPathLength) {
  // height from the root must be 1 + the longest chain of children
  auto tree = BuildBst<int>({4, 2, 6, 1, 3, 5, 7, 0});
  auto* root = tree.GetRoot();
  EXPECT_EQ(SetBalanceFactor(root), 4u);  // 4-2-1-0 chain
  EXPECT_EQ(root->left->left->balance_factor, 1);  // node 1: only left 0
}

// ============================================================================
// Task 7: MirrorTree — swap left/right subtrees of every node (in-place)
// ============================================================================

namespace {

using algorithm::MirrorTree;

// Collects the tree into a level-order array (nullopt for absent nodes)
// for comparison. Trailing nullopts are trimmed.
template <typename T>
std::vector<std::optional<T>> ToLevelOrder(const tree::Tree<T>& t) {
  std::vector<std::optional<T>> out;
  if (t.GetRoot() == nullptr) {
    return out;
  }

  std::vector<const tree::Node<T>*> floor{t.GetRoot()};
  while (!floor.empty()) {
    std::vector<const tree::Node<T>*> next;
    bool any_real = false;
    for (const auto* node : floor) {
      if (node) {
        out.push_back(node->val);
        next.push_back(node->left);
        next.push_back(node->right);
        any_real = true;
      } else {
        out.push_back(std::nullopt);
      }
    }
    if (!any_real) {
      break;
    }
    floor = std::move(next);
  }

  while (!out.empty() && !out.back().has_value()) {
    out.pop_back();
  }
  return out;
}

TEST(MirrorTree, NullptrRootStaysEmpty) {
  tree::Tree<int> empty_tree;
  MirrorTree(empty_tree);
  EXPECT_EQ(empty_tree.GetRoot(), nullptr);
}

TEST(MirrorTree, SingleNodeUnchanged) {
  auto tree = BuildTree<int>({5});
  MirrorTree(tree);
  EXPECT_EQ(ToLevelOrder(tree), (std::vector<std::optional<int>>{5}));
}

TEST(MirrorTree, FullTreeOfSevenReversed) {
  auto tree = BuildTree<int>({1, 2, 3, 4, 5, 6, 7});
  MirrorTree(tree);
  EXPECT_EQ(ToLevelOrder(tree),
            (std::vector<std::optional<int>>{1, 3, 2, 7, 6, 5, 4}));
}

TEST(MirrorTree, LeftOnlyBecomesRightOnly) {
  auto tree = BuildTree<int>({1, 2});
  MirrorTree(tree);
  EXPECT_EQ(ToLevelOrder(tree),
            (std::vector<std::optional<int>>{1, std::nullopt, 2}));
}

TEST(MirrorTree, DeepChainFlipped) {
  // chain 1 -> 2 -> 3 down the left side goes down the right side after mirror
  auto tree = BuildTree<int>({1, 2, std::nullopt, 3});
  MirrorTree(tree);
  const auto* root = tree.GetRoot();
  ASSERT_NE(root->right, nullptr);
  EXPECT_EQ(root->val, 1);
  EXPECT_EQ(root->right->val, 2);
  ASSERT_NE(root->right->right, nullptr);
  EXPECT_EQ(root->right->right->val, 3);
  EXPECT_EQ(root->left, nullptr);
}

TEST(MirrorTree, AsymmetricShapeExactPositions) {
  // {1; 2,3; 4 under 2; 6 under 3}
  auto tree = BuildTree<int>({1, 2, 3, 4, std::nullopt, 6});
  MirrorTree(tree);
  EXPECT_EQ(ToLevelOrder(tree),
            (std::vector<std::optional<int>>{1, 3, 2, std::nullopt, 6,
                                             std::nullopt, 4}));
}

TEST(MirrorTree, MirrorTwiceIsIdentity) {
  auto tree = BuildTree<std::string>({"a", "b", "c", std::nullopt, "d"});
  const auto before = ToLevelOrder(tree);
  MirrorTree(tree);
  MirrorTree(tree);
  EXPECT_EQ(ToLevelOrder(tree), before);
}

TEST(MirrorTree, MirrorOfBstIsNotBstAnymore) {
  // structural check: swapping children of a BST breaks the order property
  auto tree = BuildBst<int>({4, 2, 6, 1, 3});
  MirrorTree(tree);
  const auto* root = tree.GetRoot();
  ASSERT_NE(root->left, nullptr);
  EXPECT_GT(root->left->val, root->val);  // ex-right subtree now on the left
}

}  // namespace

}  // namespace

}  // namespace

}  // namespace

}  // namespace

}  // namespace

