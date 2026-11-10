#ifndef TREE_HPP_
#define TREE_HPP_

#include <utility>

namespace tree {

template <typename T>
struct Node {
  Node<T>* left = nullptr;
  Node<T>* right = nullptr;
  T val;

  explicit Node(T val): val(std::move(val)) {}

  Node(const Node<T>& other) = delete;
  Node(Node<T>&& other)
    : left(other.left), right(other.right), val(std::move(other.val)) {
      other.left = nullptr;
      other.right = nullptr;
    }

  Node<T>& operator=(const Node<T>& other) = delete;
  Node<T>& operator=(Node<T>&& other) {
    if (this == &other) {
      return *this;
    }

    left = other.left;
    right = other.right;
    val = std::move(other.val);

    other.left = nullptr;
    other.right = nullptr;

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

  Tree(Tree<T>&& other): root_(other.root_) {
    other.root_ = nullptr;
  }

  Tree(const Tree<T>& other) = delete;

  Tree& operator=(Tree<T>&& other) {
    if (this == &other) {
      return *this;
    }

    delete root_;

    root_ = other.root_;
    other.root_ = nullptr;

    return *this;
  }

  Tree& operator=(const Tree<T>& other) = delete;

  Node<T>* GetRoot() const {
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
