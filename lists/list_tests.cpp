#include <string>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include "list.hpp"

using list::List;
using list::Node;

static_assert(!std::is_copy_constructible_v<List<int>>);
static_assert(!std::is_move_constructible_v<List<int>>);
static_assert(!std::is_copy_assignable_v<List<int>>);
static_assert(!std::is_move_assignable_v<List<int>>);

namespace {

template <typename T>
std::vector<T> ToVector(const List<T>& lst) {
  std::vector<T> result;
  if (lst.Empty()) {
    return result;
  }
  const Node<T>* node = &lst.Front();
  while (node != nullptr) {
    result.push_back(node->val);
    node = node->next;
  }
  return result;
}

}  // namespace

TEST(ListTest, DefaultConstructedIsEmpty) {
  List<int> lst;
  EXPECT_TRUE(lst.Empty());
}

TEST(ListTest, ValueConstructorCreatesSingleElement) {
  List<int> lst(42);
  EXPECT_FALSE(lst.Empty());
  EXPECT_EQ(lst.Front().val, 42);
  EXPECT_EQ(ToVector(lst), std::vector<int>({42}));
}

TEST(ListTest, PushBackAppendsElementsInOrder) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  lst.PushBack(3);
  EXPECT_FALSE(lst.Empty());
  EXPECT_EQ(lst.Front().val, 1);
  EXPECT_EQ(ToVector(lst), std::vector<int>({1, 2, 3}));
}

TEST(ListTest, PushBackAfterValueConstructor) {
  List<int> lst(1);
  lst.PushBack(2);
  lst.PushBack(3);
  EXPECT_EQ(ToVector(lst), std::vector<int>({1, 2, 3}));
}

TEST(ListTest, InsertWithNullptrAddsToFront) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  Node<int>* inserted = lst.Insert(nullptr, 0);
  ASSERT_NE(inserted, nullptr);
  EXPECT_EQ(inserted, &lst.Front());
  EXPECT_EQ(inserted->val, 0);
  EXPECT_EQ(ToVector(lst), std::vector<int>({0, 1, 2}));
}

TEST(ListTest, InsertIntoEmptyListWithNullptr) {
  List<int> lst;
  ASSERT_TRUE(lst.Empty());
  Node<int>* inserted = lst.Insert(nullptr, 1);
  ASSERT_NE(inserted, nullptr);
  EXPECT_FALSE(lst.Empty());
  EXPECT_EQ(inserted, &lst.Front());
  EXPECT_EQ(ToVector(lst), std::vector<int>({1}));
}

TEST(ListTest, InsertAddsAfterGivenNode) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(3);
  Node<int>* inserted = lst.Insert(&lst.Front(), 2);
  ASSERT_NE(inserted, nullptr);
  EXPECT_EQ(inserted->val, 2);
  EXPECT_EQ(ToVector(lst), std::vector<int>({1, 2, 3}));
}

TEST(ListTest, InsertAfterLastNodeAppendsToEnd) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  Node<int>* inserted = lst.Insert(lst.Front().next, 3);
  ASSERT_NE(inserted, nullptr);
  EXPECT_EQ(inserted->val, 3);
  EXPECT_EQ(inserted->next, nullptr);
  EXPECT_EQ(ToVector(lst), std::vector<int>({1, 2, 3}));
}

TEST(ListTest, InsertThrowsOnForeignNode) {
  List<int> lst;
  lst.PushBack(1);
  Node<int> foreign{99};
  EXPECT_THROW(lst.Insert(&foreign, 2), std::runtime_error);
  EXPECT_EQ(ToVector(lst), std::vector<int>({1}));
}

TEST(ListTest, EraseByValueRemovesFront) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  lst.PushBack(3);
  Node<int>* next = lst.Erase(1);
  ASSERT_NE(next, nullptr);
  EXPECT_EQ(next, &lst.Front());
  EXPECT_EQ(next->val, 2);
  EXPECT_EQ(ToVector(lst), std::vector<int>({2, 3}));
}

TEST(ListTest, EraseByValueRemovesMiddle) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  lst.PushBack(3);
  Node<int>* next = lst.Erase(2);
  ASSERT_NE(next, nullptr);
  EXPECT_EQ(next->val, 3);
  EXPECT_EQ(next, lst.Front().next);
  EXPECT_EQ(ToVector(lst), std::vector<int>({1, 3}));
}

TEST(ListTest, EraseLastValueReturnsNullptr) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  Node<int>* next = lst.Erase(2);
  EXPECT_EQ(next, nullptr);
  EXPECT_EQ(ToVector(lst), std::vector<int>({1}));
}

TEST(ListTest, EraseRemovesFirstMatchOnly) {
  List<int> lst;
  lst.PushBack(7);
  lst.PushBack(7);
  lst.PushBack(8);
  Node<int>* next = lst.Erase(7);
  ASSERT_NE(next, nullptr);
  EXPECT_EQ(next->val, 7);
  EXPECT_EQ(ToVector(lst), std::vector<int>({7, 8}));
}

TEST(ListTest, EraseAllValuesEmptiesList) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  lst.PushBack(3);
  while (!lst.Empty()) {
    lst.Erase(lst.Front().val);
  }
  EXPECT_TRUE(lst.Empty());
}

TEST(ListTest, EraseOnEmptyListReturnsNullptr) {
  List<int> lst;
  EXPECT_EQ(lst.Erase(1), nullptr);
}

TEST(ListTest, EraseThrowsOnMissingValue) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  EXPECT_THROW(lst.Erase(99), std::runtime_error);
  EXPECT_EQ(ToVector(lst), std::vector<int>({1, 2}));
}

TEST(ListTest, ReturnedPointerIsUsable) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  lst.PushBack(3);
  Node<int>* after_erased = lst.Erase(1);
  ASSERT_NE(after_erased, nullptr);
  Node<int>* inserted = lst.Insert(after_erased, 5);
  ASSERT_NE(inserted, nullptr);
  EXPECT_EQ(ToVector(lst), std::vector<int>({2, 5, 3}));
  Node<int>* next = lst.Erase(5);
  EXPECT_EQ(next, after_erased->next);
  EXPECT_EQ(ToVector(lst), std::vector<int>({2, 3}));
}

TEST(ListTest, SetNewHeadReplacesFront) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  Node<int>* first = &lst.Front();
  Node<int>* second = first->next;
  first->next = nullptr;
  lst.SetNewHead(second);
  EXPECT_EQ(&lst.Front(), second);
  EXPECT_EQ(ToVector(lst), std::vector<int>({2}));
  delete first;
}

TEST(ListTest, NonTrivialValueTypeWorks) {
  List<std::string> lst("alpha");
  lst.PushBack("beta");
  lst.PushBack("gamma");
  ASSERT_FALSE(lst.Empty());
  EXPECT_EQ(lst.Front().val, "alpha");
  EXPECT_EQ(ToVector(lst), std::vector<std::string>({"alpha", "beta", "gamma"}));

  Node<std::string>* inserted = lst.Insert(&lst.Front(), "delta");
  ASSERT_NE(inserted, nullptr);
  EXPECT_EQ(ToVector(lst),
            std::vector<std::string>({"alpha", "delta", "beta", "gamma"}));

  Node<std::string>* next = lst.Erase("delta");
  ASSERT_NE(next, nullptr);
  EXPECT_EQ(next->val, "beta");
  EXPECT_EQ(ToVector(lst), std::vector<std::string>({"alpha", "beta", "gamma"}));

  next = lst.Erase("gamma");
  EXPECT_EQ(next, nullptr);
  EXPECT_EQ(ToVector(lst), std::vector<std::string>({"alpha", "beta"}));
}

TEST(DeleteElemNTest, RemovesSecondFromEnd) {
  List<int> lst;
  for (int i = 1; i <= 5; ++i) {
    lst.PushBack(i);
  }
  List<int>& result = lst.DeleteElemN(2);
  EXPECT_EQ(&result, &lst);
  EXPECT_EQ(ToVector(result), std::vector<int>({1, 2, 3, 5}));
}

TEST(DeleteElemNTest, NEqualsOneRemovesTail) {
  List<int> lst;
  for (int i = 1; i <= 5; ++i) {
    lst.PushBack(i);
  }
  Node<int>* old_head = &lst.Front();
  lst.DeleteElemN(1);
  EXPECT_EQ(ToVector(lst), std::vector<int>({1, 2, 3, 4}));
  EXPECT_EQ(&lst.Front(), old_head);
  Node<int>* tail = &lst.Front();
  while (tail->next != nullptr) {
    tail = tail->next;
  }
  EXPECT_EQ(tail->next, nullptr);
}

TEST(DeleteElemNTest, NEqualsLengthRemovesHead) {
  List<int> lst;
  for (int i = 1; i <= 5; ++i) {
    lst.PushBack(i);
  }
  Node<int>* old_second = lst.Front().next;
  lst.DeleteElemN(5);
  EXPECT_EQ(ToVector(lst), std::vector<int>({2, 3, 4, 5}));
  EXPECT_EQ(&lst.Front(), old_second);
}

TEST(DeleteElemNTest, SingleElementBecomesEmpty) {
  List<int> lst(42);
  List<int>& result = lst.DeleteElemN(1);
  EXPECT_EQ(&result, &lst);
  EXPECT_TRUE(result.Empty());
}

TEST(DeleteElemNTest, EmptyListStaysEmpty) {
  List<int> lst;
  List<int>& result = lst.DeleteElemN(3);
  EXPECT_EQ(&result, &lst);
  EXPECT_TRUE(result.Empty());
}

TEST(DeleteElemNTest, NGreaterThanLengthKeepsList) {
  List<int> lst;
  lst.PushBack(1);
  lst.PushBack(2);
  lst.PushBack(3);
  lst.DeleteElemN(5);
  EXPECT_EQ(ToVector(lst), std::vector<int>({1, 2, 3}));
}

TEST(DeleteElemNTest, RemovesByPositionNotFirstMatchByValue) {
  List<int> lst;
  lst.PushBack(5);
  lst.PushBack(7);
  lst.PushBack(3);
  lst.PushBack(7);
  lst.PushBack(8);
  lst.DeleteElemN(2);
  EXPECT_EQ(ToVector(lst), std::vector<int>({5, 7, 3, 8}));
}
