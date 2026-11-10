#ifndef CREATE_TREE_HPP_
#define CREATE_TREE_HPP_

#include <array>
#include <vector>

#include "tree.hpp"

namespace tree {

template <typename Type, size_t Sz>
Tree<Type> CreateTree(const std::array<Type, Sz>& arr) {
  
  if (arr.empty()) {
    return {};
  }

  Tree<Type> tree(arr[0]);
  Node<Type>* current_node = tree.GetRoot();

  std::vector<Node<Type>*> nodes = {current_node};
  std::vector<Node<Type>*> childrens;

  size_t ind = 1;
  while (ind < Sz) {
    for (auto* node: nodes) {
      if (ind >= Sz) break;
      auto left = new Node<Type>(arr[ind++]);
      node->left = left;
      childrens.push_back(left);

      if (ind >= Sz) break;
      auto right = new Node<Type>(arr[ind++]);
      node->right = right;
      childrens.push_back(right);
    }

    nodes = std::move(childrens);
  }
  
  return tree;
}

} // namespace tree


#endif
