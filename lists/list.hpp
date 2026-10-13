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

  ~Node() {
    delete next;
  }
};

template <typename T>
class List {
 public:
  List() = default;
  List(const T& val): head_{val} {}

  List(const List& other) = delete;
  List(List&& other) = delete;
  List& operator=(const List& other) = delete;
  List& operator=(List&& other) = delete;

  const Node<T>& Front() const {
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
      throw std::runtime_error("Invalid insertion position");
    }

    Node<T>* pos_next = pos->next;
    pos->next = new Node{val};
    pos->next->next = pos_next;
    ++sz_;
    return pos->next;
  }

  Node<T>* Erase(const Node<T>* pos) {
    // delete elem by point
    // returns the pointer to the next element after the deleted one
    if (Empty()) return nullptr;

    if (pos == nullptr) {
      Node<T>* head_next_next = head_.next->next;
      head_.next = head_next_next;v                                              v                                                                                                                                                                                                                                                                                                            c
    }
  }

 private:
  Node<T> head_;
  size_t sz_ = 0;
};
} // namespace list

#endif
