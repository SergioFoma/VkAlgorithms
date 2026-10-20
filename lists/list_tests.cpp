#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "list.hpp"

using list::List;
using list::Node;

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

// Walks the chain directly, without consulting Empty()/size.
// Useful for lists that were just move-constructed/assigned.
template <typename T>
std::vector<T> CollectFrom(const List<T>& lst) {
  std::vector<T> result;
  const Node<T>* node = &lst.Front();
  while (node != nullptr) {
    result.push_back(node->val);
    node = node->next;
  }
  return result;
}

// Counts live instances to verify that ~List/~Node actually free nodes.
struct Tracked {
  static inline int alive = 0;

  Tracked() { ++alive; }
  Tracked(const Tracked&) { ++alive; }
  Tracked& operator=(const Tracked&) = default;
  ~Tracked() { --alive; }
};

}  // namespace

// Copy is still forbidden, move is now allowed.
static_assert(!std::is_copy_constructible_v<List<int>>);
static_assert(!std::is_copy_assignable_v<List<int>>);
static_assert(std::is_move_constructible_v<List<int>>);
static_assert(std::is_move_assignable_v<List<int>>);

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

TEST(ListTest, InsertDoesNotTakeOwnershipOfForeignNode) {
  // The node passed as position is never owned by the list,
  // so destroying the list must not touch it.
  auto owned = std::make_unique<Node<int>>(7);
  {
    List<int> lst;
    lst.PushBack(1);
    lst.Insert(&lst.Front(), 2);
  }
  EXPECT_EQ(owned->val, 7);
  EXPECT_EQ(owned->next, nullptr);
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
  // first was detached from the chain, so ~List will not free it.
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

// ============================================================================
// Destructor: ~List must release the whole chain (Node's dtor recurses).
// ASan additionally catches any double-free or use-after-free here.
// ============================================================================

TEST(ListDestructorTest, DestructorFreesEntireChain) {
  const int base = Tracked::alive;
  {
    List<Tracked> lst;
    lst.PushBack(Tracked{});
    lst.PushBack(Tracked{});
    lst.PushBack(Tracked{});
    EXPECT_EQ(Tracked::alive, base + 4);  // 1 sentinel + 3 nodes
  }
  EXPECT_EQ(Tracked::alive, base);
}

TEST(ListDestructorTest, DestructorFreesValueConstructedNode) {
  const int base = Tracked::alive;
  {
    List<Tracked> lst(Tracked{});
    EXPECT_EQ(Tracked::alive, base + 2);  // 1 sentinel + 1 node
  }
  EXPECT_EQ(Tracked::alive, base);
}

TEST(ListDestructorTest, DestructorOfEmptyListIsHarmless) {
  const int base = Tracked::alive;
  {
    List<Tracked> lst;
    EXPECT_EQ(Tracked::alive, base + 1);  // sentinel only
  }
  EXPECT_EQ(Tracked::alive, base);
}

TEST(ListDestructorTest, DeleteElemNFreesRemovedNode) {
  const int base = Tracked::alive;
  {
    List<Tracked> lst;
    lst.PushBack(Tracked{});
    EXPECT_EQ(Tracked::alive, base + 2);
    lst.DeleteElemN(1);  // drops the only data node
    EXPECT_EQ(Tracked::alive, base + 1);
  }
  EXPECT_EQ(Tracked::alive, base);
}

TEST(ListDestructorTest, WorksAfterInsertAndDeleteMix) {
  // Every node ever created must be freed exactly once by the end of scope.
  const int base = Tracked::alive;
  {
    List<Tracked> lst;
    lst.PushBack(Tracked{});            // A
    lst.Insert(&lst.Front(), Tracked{});  // A -> B
    lst.Insert(nullptr, Tracked{});     // C -> A -> B
    lst.DeleteElemN(1);                 // removes B
    lst.DeleteElemN(2);                 // removes C, A stays owned by lst
  }
  EXPECT_EQ(Tracked::alive, base);
}

// ============================================================================
// Move semantics (now allowed): ownership of the chain must transfer
// exactly once, leaving the moved-from list safe to destroy.
// ============================================================================

TEST(ListMoveTest, MoveConstructorTransfersChain) {
  List<int> src;
  src.PushBack(1);
  src.PushBack(2);
  src.PushBack(3);

  List<int> dst(std::move(src));

  EXPECT_EQ(CollectFrom(dst), std::vector<int>({1, 2, 3}));
  // ~src and ~dst both run at scope end: ASan flags a double-free
  // if the move left src pointing at the same nodes.
}

TEST(ListMoveTest, MoveConstructorLeavesSourceSafeToDestroy) {
  const int base = Tracked::alive;
  {
    List<Tracked> src;
    src.PushBack(Tracked{});
    src.PushBack(Tracked{});

    List<Tracked> dst(std::move(src));

    EXPECT_EQ(CollectFrom(dst).size(), size_t{2});
  }
  // src destroyed at end of scope must not free dst's nodes a second time,
  // and every node must be freed exactly once overall.
  EXPECT_EQ(Tracked::alive, base);
}

TEST(ListMoveTest, MoveConstructorFromValueInitializedList) {
  List<int> src(42);
  List<int> dst(std::move(src));
  EXPECT_EQ(CollectFrom(dst), std::vector<int>({42}));
}

TEST(ListMoveTest, MoveAssignmentReplacesContent) {
  List<int> dst;
  dst.PushBack(10);
  dst.PushBack(20);

  List<int> src;
  src.PushBack(1);
  src.PushBack(2);

  dst = std::move(src);

  EXPECT_EQ(CollectFrom(dst), std::vector<int>({1, 2}));
}

TEST(ListMoveTest, MoveAssignmentDestroysReplacedNodes) {
  const int base = Tracked::alive;
  {
    List<Tracked> dst;
    dst.PushBack(Tracked{});
    EXPECT_EQ(Tracked::alive, base + 2);  // sentinel + 1 node

    List<Tracked> src;
    src.PushBack(Tracked{});
    src.PushBack(Tracked{});
    EXPECT_EQ(Tracked::alive, base + 5);  // + src sentinel + 2 nodes

    dst = std::move(src);

    // Old dst content is freed during assignment, src's nodes are not copied.
    EXPECT_EQ(Tracked::alive, base + 4);
    EXPECT_EQ(CollectFrom(dst).size(), size_t{2});
  }
  EXPECT_EQ(Tracked::alive, base);
}

TEST(ListMoveTest, MoveAssignmentToEmptyList) {
  List<int> dst;
  List<int> src;
  src.PushBack(7);

  dst = std::move(src);

  EXPECT_EQ(CollectFrom(dst), std::vector<int>({7}));
}

TEST(ListMoveTest, MovedIntoListStillSupportsOperations) {
  List<int> src;
  src.PushBack(2);
  List<int> dst(std::move(src));

  dst.PushBack(3);                 // chain: 2 -> 3
  dst.Insert(&dst.Front(), 1);     // Insert goes after pos: 2 -> 1 -> 3
  Node<int>* next = dst.Erase(2);  // drop the front node
  ASSERT_NE(next, nullptr);
  EXPECT_EQ(next->val, 1);

  // sz_ is not transferred by the move, so avoid Empty()/ToVector() checks
  // and walk the chain directly.
  EXPECT_EQ(CollectFrom(dst), std::vector<int>({1, 3}));
}
