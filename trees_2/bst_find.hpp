#ifndef BST_FIND_HPP_
#define BST_FIND_HPP_

#include <stdexcept>

#include "tree.hpp"

namespace algorithm {

template <typename T>
const tree::Node<T>* RecoursiveFind(const tree::Node<T>* node, size_t& counter, size_t k) {
  
  if (!node) {
    return nullptr;
  }

  auto left_res = RecoursiveFind<T>(node->left, counter, k);
  if (counter == k) {
    return left_res;
  }


  ++counter;
  if (counter == k) {
    return node;
  }

  return RecoursiveFind<T>(node->right, counter, k);
}


template <typename T>
T BSTFindMin(const tree::Tree<T>& bst_tree, size_t k) {

  size_t counter = 0; 
  
  const tree::Node<T>* node = RecoursiveFind(bst_tree.GetRoot(), counter, k);

  if (!node) {
    throw std::runtime_error("BSTFindMin: k is too large!\n");
  }

  return node->val;
}

} // namespace algorithm

#endif
