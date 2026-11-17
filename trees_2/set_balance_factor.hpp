#ifndef SET_BALANCE_FACTOR_HPP_
#define SET_BALANCE_FACTOR_HPP_

#include <algorithm>

#include "tree.hpp"

namespace algorithm {

template <typename T>
size_t SetBalanceFactor(tree::Node<T>* root) {
   
  if (!root) {
    return 0;         // zero height
  }

  size_t left_height = SetBalanceFactor(root->left);
  size_t right_height = SetBalanceFactor(root->right);

  root->balance_factor = left_height - right_height;
  
  return std::max(left_height, right_height) + 1;
}

}

#endif
