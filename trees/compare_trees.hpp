#ifndef COMPARE_TREES_HPP_
#define COMPARE_TREES_HPP_

#include "tree.hpp"

namespace tree {

template <typename T>
bool IsSameNodes(const Node<T>* node1, const Node<T>* node2) {
  if (node1 == nullptr || node2 == nullptr) {
    return node1 == node2;
  }

  if (node1->val != node2->val) {
    return false;
  }

  return IsSameNodes(node1->left, node2->left) &&
         IsSameNodes(node1->right, node2->right);
}

template <typename T>
bool IsIdentical(const tree::Tree<T>& tree1, const tree::Tree<T>& tree2) {
  return IsSameNodes(tree1.GetRoot(), tree2.GetRoot());
}

} // namespace tree

#endif
