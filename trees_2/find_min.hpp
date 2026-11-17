#ifndef FIND_MIN_HPP_
#define FIND_MIN_HPP_

#include <vector>
#include <stdexcept>
#include <utility>

#include "heap.hpp"

namespace algorithm {

template <typename T>
T HeapFindMin(const std::vector<T>& nums, size_t k) {
  
  if (k > nums.size()) {
    throw std::runtime_error("FindMin:: k is too large!\n");
  }

  auto heap = heap::Heap<T, std::less<T>>::BuildHeap(nums);
  
  size_t counter = 1;
  while (!heap.Empty()) {
    T el = heap.Pop();

    if (counter == k) {
      return el;
    }

    ++counter;
  }

  std::unreachable();
}

} // namespace algorithm

#endif
