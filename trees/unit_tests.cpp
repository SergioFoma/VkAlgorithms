#include <array>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "create_tree.hpp"
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
