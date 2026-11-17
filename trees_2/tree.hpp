#ifndef TREE_HPP_
#define TREE_HPP_

#include <utility>

namespace tree {

template <typename T>
struct Node {
  Node<T>* left = nullptr;
  Node<T>* right = nullptr;
  T val;
  int balance_factor = 0;       // for task 5

  explicit Node(T val): val(std::move(val)) {}

  Node(const Node<T>& other) = delete;
  Node(Node<T>&& other)
    : left(std::exchange(other.left, nullptr)),
      right(std::exchange(other.right, nullptr)),
      val(std::move(other.val)) {}

  Node<T>& operator=(const Node<T>& other) = delete;
  Node<T>& operator=(Node<T>&& other) {
    if (this == &other) {
      return *this;
    }

    left = std::exchange(other.left, nullptr);
    right = std::exchange(other.right, nullptr);
    val = std::move(other.val);

    return *this;
  }

  ~Node() {
    delete left;
    delete right;
  }
};

template <typename T>
class Tree {
 public:
  Tree() = default;

  explicit Tree(T val): root_(new Node<T>{std::move(val)}) {}

  Tree(Tree<T>&& other): root_(std::exchange(other.root_, nullptr)) {}

  Tree(const Tree<T>& other) = delete;

  Tree& operator=(Tree<T>&& other) {
    if (this == &other) {
      return *this;
    }

    delete root_;

    root_ = std::exchange(other.root_, nullptr);

    return *this;
  }

  Tree& operator=(const Tree<T>& other) = delete;

  const Node<T>* GetRoot() const {
    return root_;
  }

  Node<T>* GetRoot() {
    return root_;
  }

  ~Tree() {
    delete root_;
  }

 private:
  Node<T>* root_ = nullptr;

};

} // namespace tree





#endif
