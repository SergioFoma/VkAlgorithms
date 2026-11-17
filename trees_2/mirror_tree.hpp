#ifndef MIRROR_TREE_HPP_
#define MIRROR_TREE_HPP_

#include "tree.hpp"

namespace algorithm {

template <typename T>
void RecursiveMirror(tree::Node<T>* node) {
  if (!node) {
    return ;
  }

  std::swap(node->left, node->right);

  RecursiveMirror(node->left);
  RecursiveMirror(node->right);
}

template <typename T>
void MirrorTree(tree::Tree<T>& tree) {


  RecursiveMirror(tree.GetRoot());
}

} // namespace algorithm

#endif
