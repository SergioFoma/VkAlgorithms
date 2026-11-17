#ifndef MERGE_ARRAYS_HPP_
#define MERGE_ARRAYS_HPP_

#include <algorithm>
#include <filesystem>
#include <limits>

#include "heap.hpp"

namespace {

template <typename T>
struct MergeNode {
  T val;
  size_t arr_ind = 0;
  size_t ind = 0;

  bool operator()(const MergeNode<T>& node) {
    return val < node.val;
  }
};

template <typename T>
struct CustomerCompare {
  bool operator()(const MergeNode<T>& less, const MergeNode<T>& greater) {
    return less.val < greater.val;
  }
};

} // namespace

namespace algorithm {

template <typename T>
std::vector<T> MergeArrays(const std::vector<std::vector<T>>& arrays) {
  
  size_t arrays_sz = arrays.size();
  heap::Heap<MergeNode<T>, CustomerCompare<T>> heap;

  for (size_t arr_ind = 0; arr_ind < arrays_sz; ++arr_ind) {
    const auto& arr = arrays[arr_ind];
    if (!arr.empty()) {
      heap.Insert(MergeNode{arr[0], arr_ind, 0}); 
    }
  }
  
  std::vector<T> merge_arr;
  while (!heap.Empty()) {
    MergeNode node = heap.Pop();
    merge_arr.push_back(node.val);
    
    size_t arr_sz = arrays[node.arr_ind].size();
    if (arr_sz > 0 && node.ind < arr_sz - 1) {
      const auto& arr = arrays[node.arr_ind];
      heap.Insert(MergeNode{arr[node.ind + 1], node.arr_ind, node.ind + 1});
    }
  }

  return merge_arr;
}
} // namespace algorithm

#endif
