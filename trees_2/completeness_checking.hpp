#ifndef COMPLETENESS_CHECKING_HPP_
#define COMPLETENESS_CHECKING_HPP_

#include <vector>

#include "tree.hpp"

namespace algorithm {

template <typename T>
void PushChildren(std::vector<const tree::Node<T>*>& current_floor, 
                  std::vector<const tree::Node<T>*>& children,
                  size_t& not_null_children, bool& have_null_node) {

  for (const auto* node: current_floor) {
    if (node) {
      children.push_back(node->left);
      children.push_back(node->right);
      not_null_children += (node->left  ? 1 : 0);
      not_null_children += (node->right ? 1 : 0);
    } else {
      children.push_back(nullptr);
      children.push_back(nullptr);
      have_null_node = true;
    }
  }
}


template <typename T>
bool CheckLastFloor(const std::vector<const tree::Node<T>*>& current_floor) {

  size_t floor_sz = current_floor.size();
  for (size_t ind = 1; ind < floor_sz; ++ind) {
    if (!current_floor[ind - 1] && current_floor[ind]) {
      return false;
    }
  }
  return true;
}

template <typename T>
bool IsCompleteness(const tree::Tree<T>& tree) {
  const auto* root = tree.GetRoot();
  if (!root) {
    return true;
  }

  std::vector<const tree::Node<T>*> current_floor = {root};  // nodes, that have common floor
  bool last_floor = false;

  while (!last_floor) {
    std::vector<const tree::Node<T>*> children;
    size_t not_null_children = 0;
    bool have_null_node = false;

    PushChildren(current_floor, children, not_null_children, have_null_node);

    if (not_null_children == 0) {
      last_floor = true;
    } else if (have_null_node) {
      return false;
    }

    if (not_null_children > 0) {
      current_floor = std::move(children);
    }
  }
  
  return CheckLastFloor(current_floor);
}

} // namespace algorithm

#endif
