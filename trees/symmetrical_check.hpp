#ifndef SYMMETRICAL_CHECK_HPP_
#define SYMMETRICAL_CHECK_HPP_

#include <vector>

#include "tree.hpp"

namespace tree {

template <typename T>
bool IsSymmetrical(const tree::Tree<T>& tree) {
  
  std::vector<Node<T>*> nodes;
  std::vector<Node<T>*> childrens;
  Node<T>* root = nullptr;
  if ((root = tree.GetRoot()) != nullptr) {
    nodes.push_back(root);
  }
  
  while (!nodes.empty()) { 
    for (auto* node: nodes) {
      if (!node) {
        continue;
      }

      childrens.push_back(node->left);
      childrens.push_back(node->right);
    }
  
    size_t childs_sz = childrens.size();
    for (size_t ind = 0; ind < childs_sz / 2; ++ind) {
      bool equal = (!childrens[ind] && !childrens[childs_sz - ind - 1] ) ||
      (childrens[ind] && childrens[childs_sz - ind - 1] &&
      (childrens[ind]->val == childrens[childs_sz - ind - 1]->val));
      
      if (!equal) {
        return false;
      }
    }

    nodes = std::move(childrens);
  }

  return true;
}
} // namespae tree

#endif
