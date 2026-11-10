#ifndef MIN_DEPTH_HPP_
#define MIN_DEPTH_HPP_

#include <vector>
#include <iostream>

#include "tree.hpp"

namespace tree {

template <typename T>
size_t FindMinDepth(const tree::Tree<T>& tree) {
  size_t depth = 0;

  std::vector<Node<T>*> nodes;
  std::vector<Node<T>*> childs;
  Node<T>* root = nullptr;
  if ((root = tree.GetRoot()) != nullptr) {
    nodes.push_back(root);
  }

  while (!nodes.empty()) {
    ++depth;

    for (auto* node: nodes) {
      bool is_left = node->left;
      bool is_right = node->right;

      if (is_left) {
        childs.push_back(node->left);
      }
      if (is_right) {
        childs.push_back(node->right);
      }
      if (!is_left && !is_right) {
        return depth;
      }
    }
    
    nodes = std::move(childs);

    std::cout << "depth = " << depth << ", size = " << nodes.size() << '\n';
  }

  return depth;
}

} // namespace tree

#endif
