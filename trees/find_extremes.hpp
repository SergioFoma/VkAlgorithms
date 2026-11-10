#ifndef FIND_EXTREMES_HPP_
#define FIND_EXTREMES_HPP_

#include <array>

namespace tree {

template <typename Type, size_t Sz>
Type DoMull(std::array<Type, Sz>& tree_arr) {

  size_t min_ind = 0;
  size_t max_ind = 0;

  Type min_el;
  Type max_el; 

  while (min_ind < Sz) {
    min_el = tree_arr[min_ind];
    min_ind = 2 * min_ind + 1;
  }
  while (max_ind < Sz) {
    max_el = tree_arr[max_ind];
    max_ind = 2 * max_ind + 2;
  }

  return max_el * min_el;
}

} // namespace tree

#endif
