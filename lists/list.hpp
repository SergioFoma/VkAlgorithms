#ifndef LIST_HPP_
#define LIST_HPP_

#include <cstddef>
#include <cassert>
#include <stdexcept>

namespace list {

template <typename T>
struct Node {
  T val;
  Node* next = nullptr;

  Node() = default;
  Node(const T& val): val(val) {}

  ~Node() {
    delete next;
  }
};

template <typename T>
class List {
 public:
  List() = default;
  List(const T& val) {
    head_.next = new Node{val};
    ++sz_;
  }

  List(const List& other) = delete;
  List(List&& other) = delete;
  List& operator=(const List& other) = delete;
  List& operator=(List&& other) = delete;

  Node<T>& Front() const {
    return *(head_.next);
  }

  bool Empty() const {
    return sz_ == 0;
  }

  void PushBack(const T& val) {
    Node<T>* current = &head_;

    while (current->next != nullptr) {
      current = current->next;
    }

    current->next = new Node{val};
    ++sz_;
  }

  Node<T>* Insert(const Node<T>* pos, const T& val) {
    // insertion takes place after pos
    // if pos is nullptr, then insert at the beginning
    if (pos == nullptr) {
      Node<T>* head_next = head_.next;
      head_.next = new Node{val};
      head_.next->next = head_next;
      ++sz_;
      return head_.next;
    }

    Node<T>* current = head_.next;
    while (current != nullptr && current != pos) {
      current = current->next;
    }

    if (current == nullptr) {
      throw std::runtime_error("Invalid insertion position!");
    }

    Node<T>* pos_next = current->next;
    current->next = new Node{val};
    current->next->next = pos_next;
    ++sz_;
    return current->next;
  }

  Node<T>* Erase(const T& val) {
    // delete elem by val
    // returns the pointer to the next element after the deleted one
    if (Empty()) return nullptr;

    Node<T>* current = &head_;
    while (current->next != nullptr && current->next->val != val) {
      current = current->next;
    }

    if (current->next == nullptr) {
      throw std::runtime_error("Invalid insertion position");
    }

    Node<T>* pos_next_next = nullptr;
    if (current->next->next != nullptr) {
      pos_next_next = current->next->next;
    }
    current->next->next = nullptr;          // unbin tail
    delete current->next;
    current->next = pos_next_next;
    --sz_;
    return pos_next_next;
  }

  list::List<T>& DeleteElemN(size_t n) {

    list::List<T>& list = *this;
    if (list.Empty() || n == 0) {
      return list;
    }

    auto current = head_.next;
    auto faraway = current;
    list::Node<T>* prev = &(head_);

    size_t count = 0;
    while (count < n && faraway != nullptr) {
      faraway = faraway->next;
      ++count;
    }

    if (count != n) {
      return list;
    }

    while (faraway != nullptr) {
      prev = current;
      current = current->next;
      faraway = faraway->next;
    }

    auto next = current->next;
    current->next = nullptr;
    delete current;
    prev->next = next;
    --sz_;

    return list;
  }

  void SetNewHead(Node<T>* node) {
    assert(node);
    head_.next = node;
  }

 private:
  Node<T> head_;
  size_t sz_ = 0;
};
} // namespace list

#endif
