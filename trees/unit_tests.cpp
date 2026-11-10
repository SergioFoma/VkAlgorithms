#include <array>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "compare_trees.hpp"
#include "create_tree.hpp"
#include "find_extremes.hpp"
#include "min_depth.hpp"
#include "symmetrical_check.hpp"
#include "tree.hpp"

namespace {

// Вспомогательная функция: обход дерева в порядке уровней (level-order)
// с сохранением структуры. Пропущенные дети хранятся как std::nullopt,
// лишние trailing-нули в конце срезаются.
template <typename T>
std::vector<std::optional<T>> LevelOrder(const tree::Node<T>* root) {
  std::vector<std::optional<T>> out;
  if (root == nullptr) {
    return out;
  }

  std::vector<const tree::Node<T>*> cur = {root};
  std::vector<const tree::Node<T>*> next;

  while (!cur.empty()) {
    next.clear();
    for (const auto* node : cur) {
      if (node != nullptr) {
        out.push_back(node->val);
        next.push_back(node->left);
        next.push_back(node->right);
      } else {
        out.push_back(std::nullopt);
      }
    }

    bool has_real_nodes = false;
    for (const auto* node : next) {
      if (node != nullptr) {
        has_real_nodes = true;
        break;
      }
    }
    if (!has_real_nodes) {
      break;  // дальнейшие уровни состоят только из nullptr
    }
    cur = std::move(next);
  }

  // срезаем хвостовые nullptr
  while (!out.empty() && !out.back().has_value()) {
    out.pop_back();
  }
  return out;
}

// Создание "голого" дерева из массива (все элементы считаются существующими
// узлами, level-order раскладка) — для тестов структуры
template <size_t Sz>
tree::Tree<int> MakeIntTree(const std::array<int, Sz>& arr) {
  return tree::CreateTree<int, Sz>(arr);
}

tree::Node<int>* MakeNode(int val) {
  return new tree::Node<int>(val);
}

}  // namespace

// =========================================================================
// 1. Создание дерева (Tree / Node / базовая структура)
// =========================================================================

TEST(TreeCreation, DefaultTreeHasNoRoot) {
  tree::Tree<int> t;
  EXPECT_EQ(t.GetRoot(), nullptr);
}

TEST(TreeCreation, SingleValueTreeHasRootWithThatValue) {
  tree::Tree<int> t(42);
  ASSERT_NE(t.GetRoot(), nullptr);
  EXPECT_EQ(t.GetRoot()->val, 42);
  EXPECT_EQ(t.GetRoot()->left, nullptr);
  EXPECT_EQ(t.GetRoot()->right, nullptr);
}

TEST(TreeCreation, ManualTreeStructure) {
  //        1
  //       / \
  //      2   3
  //     /
  //    4
  tree::Tree<int> t(1);
  tree::Node<int>* root = t.GetRoot();
  root->left = MakeNode(2);
  root->right = MakeNode(3);
  root->left->left = MakeNode(4);

  EXPECT_EQ(LevelOrder(root),
            (std::vector<std::optional<int>>{1, 2, 3, 4}));
  EXPECT_EQ(root->left->left->val, 4);
  EXPECT_EQ(root->left->right, nullptr);
  EXPECT_EQ(root->right->left, nullptr);
  EXPECT_EQ(root->right->right, nullptr);
}

TEST(TreeCreation, RootValueCanBeChanged) {
  tree::Tree<std::string> t("hello");
  t.GetRoot()->val = "world";
  EXPECT_EQ(t.GetRoot()->val, "world");
}

TEST(TreeCreation, MoveConstructorTransfersRoot) {
  tree::Tree<int> a(7);
  tree::Node<int>* root_ptr = a.GetRoot();

  tree::Tree<int> b(std::move(a));

  EXPECT_EQ(b.GetRoot(), root_ptr);
  EXPECT_EQ(a.GetRoot(), nullptr);
  EXPECT_EQ(b.GetRoot()->val, 7);
}

TEST(TreeCreation, MoveAssignmentTransfersRoot) {
  tree::Tree<int> a(7);
  tree::Tree<int> b(100);
  tree::Node<int>* root_ptr = a.GetRoot();

  b = std::move(a);

  EXPECT_EQ(b.GetRoot(), root_ptr);
  EXPECT_EQ(b.GetRoot()->val, 7);
  EXPECT_EQ(a.GetRoot(), nullptr);
}

TEST(TreeCreation, MoveAssignmentFromEmptyTree) {
  tree::Tree<int> a;
  tree::Tree<int> b(5);

  b = std::move(a);

  EXPECT_EQ(b.GetRoot(), nullptr);
}

TEST(TreeCreation, SelfMoveAssignmentKeepsRoot) {
  tree::Tree<int> a(3);
  tree::Node<int>* root_ptr = a.GetRoot();

  a = std::move(a);

  EXPECT_EQ(a.GetRoot(), root_ptr);
  EXPECT_EQ(a.GetRoot()->val, 3);
}

TEST(NodeCreation, NodeMoveConstructorTransfersChildren) {
  auto* src = MakeNode(1);
  src->left = MakeNode(2);
  src->right = MakeNode(3);

  tree::Node<int> moved(std::move(*src));

  EXPECT_EQ(moved.val, 1);
  ASSERT_NE(moved.left, nullptr);
  ASSERT_NE(moved.right, nullptr);
  EXPECT_EQ(moved.left->val, 2);
  EXPECT_EQ(moved.right->val, 3);
  EXPECT_EQ(src->left, nullptr);
  EXPECT_EQ(src->right, nullptr);

  delete src;  // владение передано moved-узлу
}

TEST(NodeCreation, NodeMoveAssignmentTransfersChildren) {
  auto* src = MakeNode(1);
  src->left = MakeNode(2);

  tree::Node<int> dst(99);
  dst = std::move(*src);

  EXPECT_EQ(dst.val, 1);
  ASSERT_NE(dst.left, nullptr);
  EXPECT_EQ(dst.left->val, 2);
  EXPECT_EQ(src->left, nullptr);

  delete src;
}

TEST(NodeCreation, DeepTreeDestructionDoesNotCrash) {
  // Глубокая цепочка: проверка, что деструктор рекурсивно освобождает ветку
  tree::Tree<int> t(0);
  tree::Node<int>* cur = t.GetRoot();
  for (int i = 1; i < 10000; ++i) {
    cur->left = MakeNode(i);
    cur = cur->left;
  }
  EXPECT_EQ(cur->val, 9999);
}

// =========================================================================
// 2. CreateTree: восстановление дерева из level-order массива
// =========================================================================

TEST(CreateTree, EmptyArrayGivesEmptyTree) {
  std::array<int, 0> empty{};
  tree::Tree<int> t = tree::CreateTree(empty);
  EXPECT_EQ(t.GetRoot(), nullptr);
}

TEST(CreateTree, SingleElement) {
  tree::Tree<int> t = tree::CreateTree(std::array{5});
  ASSERT_NE(t.GetRoot(), nullptr);
  EXPECT_EQ(t.GetRoot()->val, 5);
  EXPECT_EQ(t.GetRoot()->left, nullptr);
  EXPECT_EQ(t.GetRoot()->right, nullptr);
}

TEST(CreateTree, TwoElementsGoesToFirstChild) {
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2});
  auto* root = t.GetRoot();
  ASSERT_NE(root, nullptr);
  ASSERT_NE(root->left, nullptr);
  EXPECT_EQ(root->left->val, 2);
  EXPECT_EQ(root->right, nullptr);
}

TEST(CreateTree, ThreeElementsFullSecondLevel) {
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2, 3});
  auto* root = t.GetRoot();
  EXPECT_EQ(root->val, 1);
  EXPECT_EQ(root->left->val, 2);
  EXPECT_EQ(root->right->val, 3);
  EXPECT_EQ(root->left->left, nullptr);
  EXPECT_EQ(root->left->right, nullptr);
  EXPECT_EQ(root->right->left, nullptr);
  EXPECT_EQ(root->right->right, nullptr);
}

TEST(CreateTree, FourElementsLastNodeIsLeftChild) {
  //        1
  //       / \
  //      2   3
  //     /
  //    4
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2, 3, 4});
  auto* root = t.GetRoot();
  EXPECT_EQ(root->left->left->val, 4);
  EXPECT_EQ(root->left->right, nullptr);
  EXPECT_EQ(root->right->left, nullptr);
  EXPECT_EQ(root->right->right, nullptr);
}

TEST(CreateTree, FiveElementsFillsLeftThenRight) {
  //        1
  //       / \
  //      2   3
  //     / \
  //    4   5
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2, 3, 4, 5});
  auto* n2 = t.GetRoot()->left;
  EXPECT_EQ(n2->left->val, 4);
  EXPECT_EQ(n2->right->val, 5);
  EXPECT_EQ(t.GetRoot()->right->left, nullptr);
  EXPECT_EQ(t.GetRoot()->right->right, nullptr);
}

TEST(CreateTree, SixElementsOnlyRightMostNodeChildless) {
  //        1
  //       / \
  //      2   3
  //     / \ /
  //    4  5 6
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2, 3, 4, 5, 6});
  auto* n3 = t.GetRoot()->right;
  EXPECT_EQ(n3->left->val, 6);
  EXPECT_EQ(n3->right, nullptr);
}

TEST(CreateTree, SevenElementsPerfectBinaryTree) {
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2, 3, 4, 5, 6, 7});
  auto* root = t.GetRoot();
  EXPECT_EQ(root->val, 1);
  EXPECT_EQ(root->left->val, 2);
  EXPECT_EQ(root->right->val, 3);
  EXPECT_EQ(root->left->left->val, 4);
  EXPECT_EQ(root->left->right->val, 5);
  EXPECT_EQ(root->right->left->val, 6);
  EXPECT_EQ(root->right->right->val, 7);
  EXPECT_EQ(LevelOrder(root).size(), 7u);
}

TEST(CreateTree, FifteenElementsPerfectTreeDepthFour) {
  std::array<int, 15> arr = {1,  2,  3,  4,  5,  6,  7,  8,
                             9,  10, 11, 12, 13, 14, 15};
  tree::Tree<int> t = MakeIntTree(arr);

  std::vector<std::optional<int>> expected(arr.begin(), arr.end());
  EXPECT_EQ(LevelOrder(t.GetRoot()), expected);
}

TEST(CreateTree, TenElementsNonPerfectShape) {
  //        1
  //       / \
  //      2   3
  //     / \ / \
  //    4  5 6  7
  //   / \ /
  //  8  9 10
  std::array<int, 10> arr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  tree::Tree<int> t = MakeIntTree(arr);

  std::vector<std::optional<int>> expected(arr.begin(), arr.end());
  EXPECT_EQ(LevelOrder(t.GetRoot()), expected);

  auto* root = t.GetRoot();
  EXPECT_EQ(root->left->left->val, 4);
  EXPECT_EQ(root->left->left->left->val, 8);
  EXPECT_EQ(root->left->left->right->val, 9);
  EXPECT_EQ(root->left->right->left->val, 10);
  EXPECT_EQ(root->left->right->right, nullptr);
  EXPECT_EQ(root->right->left->left, nullptr);
  EXPECT_EQ(root->right->right->left, nullptr);
}

TEST(CreateTree, ThirtyOneElementsDepthFive) {
  std::array<int, 31> arr{};
  for (size_t i = 0; i < arr.size(); ++i) {
    arr[i] = static_cast<int>(i) + 1;
  }
  tree::Tree<int> t = MakeIntTree(arr);

  std::vector<std::optional<int>> expected(arr.begin(), arr.end());
  EXPECT_EQ(LevelOrder(t.GetRoot()), expected);
}

TEST(CreateTree, NegativeAndZeroValues) {
  std::array<int, 7> arr = {0, -1, -2, -3, -4, -5, -6};
  tree::Tree<int> t = MakeIntTree(arr);

  std::vector<std::optional<int>> expected(arr.begin(), arr.end());
  EXPECT_EQ(LevelOrder(t.GetRoot()), expected);
}

TEST(CreateTree, StringValues) {
  std::array<std::string, 7> arr = {"a", "b", "c", "d", "e", "f", "g"};
  tree::Tree<std::string> t = tree::CreateTree(arr);

  std::vector<std::optional<std::string>> expected(arr.begin(), arr.end());
  EXPECT_EQ(LevelOrder(t.GetRoot()), expected);
}

TEST(CreateTree, DoubleValues) {
  std::array<double, 3> arr = {1.5, -2.25, 3.0};
  tree::Tree<double> t = tree::CreateTree(arr);

  ASSERT_NE(t.GetRoot(), nullptr);
  EXPECT_DOUBLE_EQ(t.GetRoot()->val, 1.5);
  EXPECT_DOUBLE_EQ(t.GetRoot()->left->val, -2.25);
  EXPECT_DOUBLE_EQ(t.GetRoot()->right->val, 3.0);
}

TEST(CreateTree, RoundTripLevelOrderPreservesArray) {
  // Для полного массива без пропусков LevelOrder должен точно воспроизводить массив
  std::array<int, 20> arr{};
  for (size_t i = 0; i < arr.size(); ++i) {
    arr[i] = static_cast<int>(i * i);
  }
  tree::Tree<int> t = MakeIntTree(arr);

  std::vector<std::optional<int>> expected(arr.begin(), arr.end());
  EXPECT_EQ(LevelOrder(t.GetRoot()), expected);
}

// =========================================================================
// 3. IsSymmetrical: проверка симметричности дерева
// =========================================================================

TEST(Symmetrical, EmptyTreeIsSymmetrical) {
  tree::Tree<int> t;
  EXPECT_TRUE(IsSymmetrical(t));
}

TEST(Symmetrical, SingleNodeIsSymmetrical) {
  tree::Tree<int> t = tree::CreateTree(std::array{1});
  EXPECT_TRUE(IsSymmetrical(t));
}

TEST(Symmetrical, RootWithTwoEqualChildrenIsSymmetrical) {
  //      1
  //     / \
  //    2   2
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2, 2});
  EXPECT_TRUE(IsSymmetrical(t));
}

TEST(Symmetrical, RootWithTwoDifferentChildrenIsNotSymmetrical) {
  //      1
  //     / \
  //    2   3
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2, 3});
  EXPECT_FALSE(IsSymmetrical(t));
}

TEST(Symmetrical, OnlyLeftChildIsNotSymmetrical) {
  //      1
  //     /
  //    2
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2});
  EXPECT_FALSE(IsSymmetrical(t));
}

TEST(Symmetrical, PerfectSymmetricalTreeDepthThree) {
  //        1
  //       / \
  //      2   2
  //     /\   /\
  //    3  4 4  3
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2, 2, 3, 4, 4, 3});
  EXPECT_TRUE(IsSymmetrical(t));
}

TEST(Symmetrical, MirrorValuesOnThirdLevel) {
  //        1
  //       / \
  //      2   2
  //     /\   /\
  //    4  5 5  4
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2, 2, 4, 5, 5, 4});
  EXPECT_TRUE(IsSymmetrical(t));
}

TEST(Symmetrical, SingleMismatchOnThirdLevel) {
  //        1
  //       / \
  //      2   2
  //     /\   /\
  //    3  4 4  5   <- правый крайний 5 вместо 3
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2, 2, 3, 4, 4, 5});
  EXPECT_FALSE(IsSymmetrical(t));
}

TEST(Symmetrical, CompletelyAsymmetricValues) {
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2, 2, 4, 5, 6, 7});
  EXPECT_FALSE(IsSymmetrical(t));
}

TEST(Symmetrical, DeepSymmetricalTreeDepthFour) {
  //              1
  //            /   \
  //           2     2
  //          /\     /\
  //         3  4   4  3
  //        /\ /\   /\ /\
  //       5 6 7 8 8 7 6 5
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2, 2, 3, 4, 4, 3,
                                   5, 6, 7, 8, 8, 7, 6, 5});
  EXPECT_TRUE(IsSymmetrical(t));
}

TEST(Symmetrical, DeepTreeWithOneSwappedLeaf) {
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2, 2, 3, 4, 4, 3,
                                   5, 6, 7, 8, 8, 7, 6, 9});  // 9 != 5
  EXPECT_FALSE(IsSymmetrical(t));
}

TEST(Symmetrical, MirrorShapeSubtreesAreSymmetrical) {
  //        1
  //       / \
  //      2   2
  //       \   \
  //        3   3
  // Зеркальная форма: правый потомок слева и правый потомок справа?
  // На самом деле зеркало: у левого 2 правый сын, у правого 2 левый сын
  tree::Tree<int> t(1);
  auto* root = t.GetRoot();
  root->left = MakeNode(2);
  root->right = MakeNode(2);
  root->left->right = MakeNode(3);
  root->right->left = MakeNode(3);
  EXPECT_TRUE(IsSymmetrical(t));
}

TEST(Symmetrical, DifferentDepthBranchesNotSymmetrical) {
  //          1
  //         / \
  //        2   2
  //       /
  //      3          <- левая ветка глубже правой
  tree::Tree<int> t(1);
  auto* root = t.GetRoot();
  root->left = MakeNode(2);
  root->right = MakeNode(2);
  root->left->left = MakeNode(3);
  EXPECT_FALSE(IsSymmetrical(t));
}

TEST(Symmetrical, SameValuesButNotMirrorShapeIsNotSymmetrical) {
  //          1
  //         / \
  //        2   2
  //       /     /
  //      3     3
  // Значения по уровням симметричны, но форма НЕ зеркальна:
  // у левого 2 левый сын, значит у правого 2 должен быть правый сын.
  tree::Tree<int> t(1);
  auto* root = t.GetRoot();
  root->left = MakeNode(2);
  root->right = MakeNode(2);
  root->left->left = MakeNode(3);
  root->right->left = MakeNode(3);
  EXPECT_FALSE(IsSymmetrical(t));
}

TEST(Symmetrical, StringTreeSymmetrical) {
  std::array<std::string, 7> arr = {"m", "a", "a", "b", "c", "c", "b"};
  tree::Tree<std::string> t = tree::CreateTree(arr);
  EXPECT_TRUE(IsSymmetrical(t));
}

TEST(Symmetrical, StringTreeNotSymmetrical) {
  std::array<std::string, 7> arr = {"m", "a", "a", "b", "c", "c", "x"};
  tree::Tree<std::string> t = tree::CreateTree(arr);
  EXPECT_FALSE(IsSymmetrical(t));
}

// =========================================================================
// 4. FindMinDepth: минимальная глубина дерева
// =========================================================================

TEST(MinDepth, EmptyTreeDepthIsZero) {
  tree::Tree<int> t;
  EXPECT_EQ(FindMinDepth(t), 0u);
}

TEST(MinDepth, SingleNodeDepthIsOne) {
  tree::Tree<int> t = tree::CreateTree(std::array{1});
  EXPECT_EQ(FindMinDepth(t), 1u);
}

TEST(MinDepth, RootWithOnlyLeftChildDepthIsTwo) {
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2});
  EXPECT_EQ(FindMinDepth(t), 2u);
}

TEST(MinDepth, FullSecondLevelDepthIsTwo) {
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2, 3});
  EXPECT_EQ(FindMinDepth(t), 2u);
}

TEST(MinDepth, FourElementsRightBranchIsShortest) {
  //        1
  //       / \
  //      2   3   <- 3 лист на глубине 2
  //     /
  //    4
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2, 3, 4});
  EXPECT_EQ(FindMinDepth(t), 2u);
}

TEST(MinDepth, PerfectTreeOfSevenDepthIsThree) {
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2, 3, 4, 5, 6, 7});
  EXPECT_EQ(FindMinDepth(t), 3u);
}

TEST(MinDepth, PerfectTreeOfFifteenDepthIsFour) {
  std::array<int, 15> arr{};
  for (size_t i = 0; i < arr.size(); ++i) {
    arr[i] = static_cast<int>(i) + 1;
  }
  tree::Tree<int> t = MakeIntTree(arr);
  EXPECT_EQ(FindMinDepth(t), 4u);
}

TEST(MinDepth, PerfectTreeOfThirtyOneDepthIsFive) {
  std::array<int, 31> arr{};
  for (size_t i = 0; i < arr.size(); ++i) {
    arr[i] = static_cast<int>(i) + 1;
  }
  tree::Tree<int> t = MakeIntTree(arr);
  EXPECT_EQ(FindMinDepth(t), 5u);
}

TEST(MinDepth, SkewedLeftChainDepthIsHeight) {
  //    1
  //   /
  //  2
  // /
  // 3
  //  \
  //   4
  tree::Tree<int> t(1);
  auto* root = t.GetRoot();
  root->left = MakeNode(2);
  root->left->left = MakeNode(3);
  root->left->left->right = MakeNode(4);
  EXPECT_EQ(FindMinDepth(t), 4u);
}

TEST(MinDepth, SkewedRightChainDepthIsHeight) {
  tree::Tree<int> t(1);
  auto* root = t.GetRoot();
  root->right = MakeNode(2);
  root->right->right = MakeNode(3);
  root->right->right->right = MakeNode(4);
  root->right->right->right->right = MakeNode(5);
  EXPECT_EQ(FindMinDepth(t), 5u);
}

TEST(MinDepth, ShallowLeafOnRightDeepLeftSubtree) {
  //          1
  //         / \
  //        2   3      <- лист на глубине 2
  //       / \
  //      4   5
  //     / \
  //    6   7
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2, 3, 4, 5, 6, 7});
  // дерево полное, но переделаем: у 3 нет детей только если элементов <7
  // здесь 3 имеет детей 6,7 => минимальная глубина 3
  EXPECT_EQ(FindMinDepth(t), 3u);
}

TEST(MinDepth, DeepLeftShortRightManualTree) {
  //          1
  //         / \
  //        2   3   <- лист
  //       / \
  //      4   5
  tree::Tree<int> t(1);
  auto* root = t.GetRoot();
  root->left = MakeNode(2);
  root->right = MakeNode(3);
  root->left->left = MakeNode(4);
  root->left->right = MakeNode(5);
  EXPECT_EQ(FindMinDepth(t), 2u);
}

TEST(MinDepth, LeafIsOnlyAtBottomOfBalancedTree) {
  //          1
  //         / \
  //        2   3
  //       /        <- глубина до листа 4, но лист 3 на глубине 2
  //      4
  //       \
  //        5
  tree::Tree<int> t(1);
  auto* root = t.GetRoot();
  root->left = MakeNode(2);
  root->right = MakeNode(3);
  root->left->left = MakeNode(4);
  root->left->left->right = MakeNode(5);
  EXPECT_EQ(FindMinDepth(t), 2u);
}

TEST(MinDepth, AllLeavesOnSameDeepLevel) {
  //          1
  //         / \
  //        2   3
  //       /     \
  //      4       5
  //     /         \
  //    6           7
  tree::Tree<int> t(1);
  auto* root = t.GetRoot();
  root->left = MakeNode(2);
  root->right = MakeNode(3);
  root->left->left = MakeNode(4);
  root->right->right = MakeNode(5);
  root->left->left->left = MakeNode(6);
  root->right->right->right = MakeNode(7);
  EXPECT_EQ(FindMinDepth(t), 4u);
}

TEST(MinDepth, StringTreeMinDepth) {
  std::array<std::string, 7> arr = {"a", "b", "c", "d", "e", "f", "g"};
  tree::Tree<std::string> t = tree::CreateTree(arr);
  EXPECT_EQ(FindMinDepth(t), 3u);
}

TEST(MinDepth, SingleChainOfTenNodes) {
  tree::Tree<int> t(0);
  auto* cur = t.GetRoot();
  for (int i = 1; i <= 10; ++i) {
    cur->right = MakeNode(i);
    cur = cur->right;
  }
  EXPECT_EQ(FindMinDepth(t), 11u);
}

TEST(MinDepth, SixElementsShortestLeafAtDepthThree) {
  //        1
  //       / \
  //      2   3
  //     / \ /
  //    4  5 6
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2, 3, 4, 5, 6});
  EXPECT_EQ(FindMinDepth(t), 3u);
}

// =========================================================================
// 5. DoMull: произведение min и max в BST, заданной level-order массивом
//
// Массив — complete-раскладка BST: дети узла i в позициях 2i+1 (левый)
// и 2i+2 (правый). Минимум — самый левый узел (спуск 2i+1), максимум —
// самый правый узел (спуск 2i+2). Спуск по одной ветке — O(h), h — высота.
// =========================================================================

namespace {

// Истинные min/max для complete-BST массива (эталон для сравнения):
// спуск влево: 0 -> 1 -> 3 -> 7..., спуск вправо: 0 -> 2 -> 6 -> 14...
std::pair<int, int> TrueMinMax(const std::vector<int>& arr) {
  int mn = arr[0];
  size_t i = 1;
  while (i < arr.size()) {
    mn = arr[i];
    i = 2 * i + 1;
  }

  int mx = arr[0];
  size_t j = 2;
  while (j < arr.size()) {
    mx = arr[j];
    j = 2 * j + 2;
  }
  return {mn, mx};
}

}  // namespace

TEST(FindExtremes, SingleElementProductIsSquare) {
  std::array<int, 1> arr = {42};
  EXPECT_EQ(tree::DoMull(arr), 42 * 42);
}

TEST(FindExtremes, AllSameValuesProductIsSquare) {
  std::array<int, 7> arr = {7, 7, 7, 7, 7, 7, 7};
  EXPECT_EQ(tree::DoMull(arr), 49);
}

TEST(FindExtremes, TwoElementsMinIsLeftChild) {
  //      10
  //     /
  //    3
  std::array<int, 2> arr = {10, 3};
  EXPECT_EQ(tree::DoMull(arr), 30);
}

TEST(FindExtremes, BasicBstDepthTwo) {
  //      7
  //     / \
  //    4   9
  std::array<int, 3> arr = {7, 4, 9};
  EXPECT_EQ(tree::DoMull(arr), 36);
}

TEST(FindExtremes, ClassicBstDepthThree) {
  //          10
  //        /    \
  //       5     15
  //      / \    / \
  //     3   7  12  20
  std::array<int, 7> arr = {10, 5, 15, 3, 7, 12, 20};
  EXPECT_EQ(tree::DoMull(arr), 3 * 20);
}

TEST(FindExtremes, IncompleteLastLevelLeftDeep) {
  //          10
  //        /    \
  //       5     15
  //      /
  //     3
  std::array<int, 4> arr = {10, 5, 15, 3};
  EXPECT_EQ(tree::DoMull(arr), 3 * 15);
}

TEST(FindExtremes, IncompleteLastLevelRightOnly) {
  //          10
  //        /    \
  //       5     15
  //        \
  //         7
  std::array<int, 5> arr = {10, 5, 15, 3, 7};
  EXPECT_EQ(tree::DoMull(arr), 3 * 15);
}

TEST(FindExtremes, FullBstDepthFour) {
  //              8
  //           /     \
  //          4      12
  //         / \    /  \
  //        2   6  10  14
  //       /\  /\  /\  /\
  //      1  3 5 7 9 11 13 15
  std::array<int, 15> arr = {8, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 15};
  EXPECT_EQ(tree::DoMull(arr), 15);  // 1 * 15
}

TEST(FindExtremes, NegativeValues) {
  //          -1
  //        /    \
  //      -5      2
  //      / \    / \
  //    -8  -3  1   3
  std::array<int, 7> arr = {-1, -5, 2, -8, -3, 1, 3};
  EXPECT_EQ(tree::DoMull(arr), -24);  // -8 * 3
}

TEST(FindExtremes, ZeroRoot) {
  //       0
  //      / \
  //    -1   1
  std::array<int, 3> arr = {0, -1, 1};
  EXPECT_EQ(tree::DoMull(arr), -1);
}

TEST(FindExtremes, MaxEqualsRootWhenNoRightChild) {
  //      15
  //     /
  //    7
  // Единственный способ для complete-раскладки не иметь правого ребёнка
  // корня при двух элементах: максимум тогда в корне.
  std::array<int, 2> arr = {15, 7};
  EXPECT_EQ(tree::DoMull(arr), 105);  // 7 * 15
}

TEST(FindExtremes, Int64AvoidsOverflow) {
  //        100000
  //        /     \
  //    50000    200000
  std::array<int64_t, 3> arr = {100000, 50000, 200000};
  EXPECT_EQ(tree::DoMull(arr), int64_t{50000} * 200000);
}

TEST(FindExtremes, DoubleValues) {
  std::array<double, 3> arr = {2.5, 1.5, 3.5};
  EXPECT_EQ(tree::DoMull(arr), 1.5 * 3.5);
}

TEST(FindExtremes, AgreesWithTrueMinMaxReference) {
  // сверка DoMull с эталонным поиском min/max по level-order раскладке
  std::array<int, 7> arr = {10, 5, 15, 3, 7, 12, 20};
  std::vector<int> ref(arr.begin(), arr.end());

  auto [mn, mx] = TrueMinMax(ref);
  EXPECT_EQ(mn, 3);
  EXPECT_EQ(mx, 20);
  EXPECT_EQ(tree::DoMull(arr), mn * mx);
}

TEST(FindExtremes, ConsistentWithCreatedTree) {
  // DoMull по массиву == min*max, если искать крайние спуском по дереву
  std::array<int, 7> arr = {10, 5, 15, 3, 7, 12, 20};
  tree::Tree<int> t = tree::CreateTree(arr);

  // спуск влево от корня ( BST-минимум)
  auto* node = t.GetRoot();
  while (node->left) {
    node = node->left;
  }
  const int min_val = node->val;

  // спуск вправо (BST-максимум)
  node = t.GetRoot();
  while (node->right) {
    node = node->right;
  }
  const int max_val = node->val;

  EXPECT_EQ(tree::DoMull(arr), min_val * max_val);
}

// =========================================================================
// 6. IsIdentical: идентичны ли два дерева (структура + значения), рекурсия
// =========================================================================

TEST(CompareTrees, TwoEmptyTreesAreIdentical) {
  tree::Tree<int> t1;
  tree::Tree<int> t2;
  EXPECT_TRUE(IsIdentical(t1, t2));
}

TEST(CompareTrees, EmptyAndSingleAreNotIdentical) {
  tree::Tree<int> empty;
  tree::Tree<int> single(1);
  EXPECT_FALSE(IsIdentical(empty, single));
  EXPECT_FALSE(IsIdentical(single, empty));  // симметричность проверки
}

TEST(CompareTrees, TwoSinglesSameValue) {
  tree::Tree<int> t1(42);
  tree::Tree<int> t2(42);
  EXPECT_TRUE(IsIdentical(t1, t2));
}

TEST(CompareTrees, TwoSinglesDifferentValue) {
  tree::Tree<int> t1(42);
  tree::Tree<int> t2(43);
  EXPECT_FALSE(IsIdentical(t1, t2));
}

TEST(CompareTrees, SameTreeWithItself) {
  tree::Tree<int> t = tree::CreateTree(std::array{1, 2, 3, 4, 5});
  EXPECT_TRUE(IsIdentical(t, t));
}

TEST(CompareTrees, IdenticalFullTrees) {
  tree::Tree<int> t1 = tree::CreateTree(std::array{1, 2, 3, 4, 5, 6, 7});
  tree::Tree<int> t2 = tree::CreateTree(std::array{1, 2, 3, 4, 5, 6, 7});
  EXPECT_TRUE(IsIdentical(t1, t2));
}

TEST(CompareTrees, DifferentValueInOneNode) {
  tree::Tree<int> t1 = tree::CreateTree(std::array{1, 2, 3});
  tree::Tree<int> t2 = tree::CreateTree(std::array{1, 2, 4});  // right 3->4
  EXPECT_FALSE(IsIdentical(t1, t2));
}

TEST(CompareTrees, SameShapeDifferentValuesDeep) {
  tree::Tree<int> t1 = tree::CreateTree(std::array{1, 2, 3, 4, 5, 6, 7});
  tree::Tree<int> t2 = tree::CreateTree(std::array{1, 2, 3, 4, 5, 6, 8});
  EXPECT_FALSE(IsIdentical(t1, t2));
}

TEST(CompareTrees, SameValuesDifferentShape) {
  // тот же набор значений, но 2 и 3 поменяны местами — структура разная
  tree::Tree<int> t1 = tree::CreateTree(std::array{1, 2, 3});
  tree::Tree<int> t2 = tree::CreateTree(std::array{1, 3, 2});
  EXPECT_FALSE(IsIdentical(t1, t2));
}

TEST(CompareTrees, DifferentSizesOneExtraLeaf) {
  tree::Tree<int> t1 = tree::CreateTree(std::array{1, 2, 3});
  tree::Tree<int> t2 = tree::CreateTree(std::array{1, 2, 3, 4});
  EXPECT_FALSE(IsIdentical(t1, t2));
}

TEST(CompareTrees, LeftChildVersusRightChildStructure) {
  //        1                       1
  //       /                         \
  //      2                           2
  // значение то же, но ребёнок с другой стороны
  tree::Tree<int> t1(1);
  t1.GetRoot()->left = new tree::Node<int>(2);
  tree::Tree<int> t2(1);
  t2.GetRoot()->right = new tree::Node<int>(2);
  EXPECT_FALSE(IsIdentical(t1, t2));
}

TEST(CompareTrees, SameSetDifferentDepthChain) {
  //        1                       1
  //       / \                       \
  //      2   3                       2
  //                                  \
  //                                   3
  tree::Tree<int> t1 = tree::CreateTree(std::array{1, 2, 3});
  tree::Tree<int> t2(1);
  t2.GetRoot()->right = new tree::Node<int>(2);
  t2.GetRoot()->right->right = new tree::Node<int>(3);
  EXPECT_FALSE(IsIdentical(t1, t2));
}

TEST(CompareTrees, DeepIdenticalTrees) {
  std::array<int, 15> arr = {8, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 15};
  tree::Tree<int> t1 = tree::CreateTree(arr);
  tree::Tree<int> t2 = tree::CreateTree(arr);
  EXPECT_TRUE(IsIdentical(t1, t2));
}

TEST(CompareTrees, DeepDifferenceAtLastLevel) {
  std::array<int, 15> a1 = {8, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 15};
  std::array<int, 15> a2 = {8, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 16};
  tree::Tree<int> t1 = tree::CreateTree(a1);
  tree::Tree<int> t2 = tree::CreateTree(a2);
  EXPECT_FALSE(IsIdentical(t1, t2));
}

TEST(CompareTrees, ManualIdenticalTrees) {
  //          1                  1
  //         / \                / \
  //        2   3             2   3
  //       /                  /
  //      4                  4
  tree::Tree<int> t1(1);
  t1.GetRoot()->left = new tree::Node<int>(2);
  t1.GetRoot()->right = new tree::Node<int>(3);
  t1.GetRoot()->left->left = new tree::Node<int>(4);

  tree::Tree<int> t2(1);
  t2.GetRoot()->left = new tree::Node<int>(2);
  t2.GetRoot()->right = new tree::Node<int>(3);
  t2.GetRoot()->left->left = new tree::Node<int>(4);

  EXPECT_TRUE(IsIdentical(t1, t2));
}

TEST(CompareTrees, ManualTreesDifferInMiddleOfChain) {
  //        1              1
  //       /              /
  //      2             99   <- значение не совпадает уже на втором узле
  //       \              \
  //        3              3
  tree::Tree<int> t1(1);
  t1.GetRoot()->left = new tree::Node<int>(2);
  t1.GetRoot()->left->right = new tree::Node<int>(3);

  tree::Tree<int> t2(1);
  t2.GetRoot()->left = new tree::Node<int>(99);
  t2.GetRoot()->left->right = new tree::Node<int>(3);

  EXPECT_FALSE(IsIdentical(t1, t2));
}

TEST(CompareTrees, StringTreesIdentical) {
  std::array<std::string, 7> arr = {"m", "a", "b", "c", "d", "e", "f"};
  tree::Tree<std::string> t1 = tree::CreateTree(arr);
  tree::Tree<std::string> t2 = tree::CreateTree(arr);
  EXPECT_TRUE(IsIdentical(t1, t2));
}

TEST(CompareTrees, StringTreesNotIdentical) {
  tree::Tree<std::string> t1 = tree::CreateTree(
      std::array<std::string, 3>{"m", "a", "b"});
  tree::Tree<std::string> t2 = tree::CreateTree(
      std::array<std::string, 3>{"m", "a", "x"});
  EXPECT_FALSE(IsIdentical(t1, t2));
}

TEST(CompareTrees, DoubleTreesIdenticalExact) {
  tree::Tree<double> t1 = tree::CreateTree(std::array{1.5, 0.25, 2.75});
  tree::Tree<double> t2 = tree::CreateTree(std::array{1.5, 0.25, 2.75});
  EXPECT_TRUE(IsIdentical(t1, t2));
}

TEST(CompareTrees, ConsistentWithSymmetryOnMirroredTrees) {
  // зеркальные деревья НЕ идентичны (но симметричны) — разные понятия
  tree::Tree<int> t1 = tree::CreateTree(std::array{1, 2, 2, 3, 4, 4, 3});
  tree::Tree<int> t2 = tree::CreateTree(std::array{1, 2, 2, 4, 3, 3, 4});
  EXPECT_FALSE(IsIdentical(t1, t2));
  EXPECT_TRUE(IsSymmetrical(t1));
  EXPECT_TRUE(IsSymmetrical(t2));
}

