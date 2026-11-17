#ifndef HEAP_HPP_
#define HEAP_HPP_

#include <functional>
#include <utility>
#include <vector>

namespace heap {

template <typename T, typename Comparator = std::less<T>>
class Heap {
 public:
  Heap() = default;

  Heap(const std::vector<T> arr)
    : heap_(std::move(arr)) {}

  size_t GetTop() const {
    if (heap_.empty()) {
      return heap_.size();
    }

    return 0;
  }

  size_t GetLeft(size_t ind) const {
    size_t pos = 2 * ind + 1;
    if (pos >= heap_.size()) {
      return heap_.size();
    }

    return pos;
  }

  size_t GetRight(size_t ind) const {
    size_t pos = 2 * ind + 2;
    if (pos >= heap_.size()) {
      return heap_.size();
    }
    return pos;
  }

  size_t GetParentInd(size_t ind) const {
    if (ind == 0) {
      return 0;
    }

    return (ind - 1) / 2;
  }
  
  void SiftUp(size_t current_ind) {
    size_t parent_ind = GetParentInd(current_ind);

    while (current_ind != 0 &&
           comp_(heap_[current_ind], heap_[parent_ind])) {
      std::swap(heap_[current_ind], heap_[parent_ind]);
      current_ind = parent_ind;
      parent_ind = GetParentInd(current_ind);
    }
  }

  void SiftDown(size_t current_ind) {
    size_t el_ind = 0;

    while (current_ind < heap_.size()) {
      size_t left_ind = GetLeft(current_ind);
      size_t right_ind = GetRight(current_ind);

      if (left_ind == heap_.size() && right_ind == heap_.size()) {
        break;
      } else if (left_ind == heap_.size()) {
          el_ind = right_ind;
      } else if (right_ind == heap_.size()){
          el_ind = left_ind;
      } else {
        el_ind = (comp_(heap_[left_ind], heap_[right_ind]) ?
                  left_ind :
                  right_ind);
      }

      if (comp_(heap_[el_ind], heap_[current_ind])) {
        std::swap(heap_[current_ind], heap_[el_ind]);
        current_ind = el_ind;
      } else {
        break;
      }
    }
  }

  void Insert(T val) {
    heap_.push_back(std::move(val));

    SiftUp(heap_.size() - 1);
  }

  T Pop() {

    if (Empty()) {
      throw std::runtime_error("Pop: Heap is empty!\n");
    }

    T top = heap_[GetTop()];
    std::swap(heap_[GetTop()], heap_.back());
    heap_.pop_back();

    if (Empty()) {
      return top;
    }

    SiftDown(GetTop());

    return top;
  }
  
  static Heap<T, Comparator> BuildHeap(const std::vector<T>& arr) {

    Heap<T, Comparator> heap(arr);
    
    int arr_sz = static_cast<int>(arr.size());
    for (int ind = arr_sz / 2; ind >= 0; --ind) {
      heap.SiftDown(ind);
    }

    return heap;
  }

  bool Empty() {
    return heap_.size() == 0;
  }

  std::vector<T> GetHeap() {
    return heap_;
  }

 private:
  std::vector<T> heap_;
  Comparator comp_;
};

} // namespace heap

#endif
