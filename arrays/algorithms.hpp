#ifndef ALGORITHMS_HPP_
#define ALGORITHMS_HPP_

#include <array>
#include <algorithm>
#include <stdexcept>
#include <iostream>

struct TwoSumRes {
  int first = 0;
  int second = 0;
};

template <size_t sz>
TwoSumRes TwoSum(const std::array<int, sz>& numbers, int target) {
  if (sz == 0) {
    return {};
  }

  size_t left = 0, right = sz - 1;
  int current_sum = 0;

  TwoSumRes res = {};
  bool is_find = false;

  while (!is_find && left < right) {
    current_sum = numbers[left] + numbers[right];

    if (current_sum > target) {
      --right;
    } else if (current_sum < target) {
      ++left;
    } else {
      res = {numbers[left], numbers[right]};
      is_find = true;
    }
  }

  return res;
}

template <size_t sz>
void Reverse(std::array<int, sz>& numbers) {

  if (sz <= 1) {
    return ;
  }

  size_t middle = sz / 2;

  for (size_t ind = 0; ind < middle; ++ind) {
    std::swap(numbers[ind], numbers[sz - 1 - ind]);
  }
}


/*!
* counter - how many elements i processed
*/
template <size_t sz>
void RingShift(std::array<int, sz>& numbers, size_t shift) {

  if (sz == 0) {
    return ;
  }

  size_t counter = 0, ind = 0;
  int saved_elem = numbers[ind];
  int current_elem = saved_elem;
  while (counter < sz) {
    ind = (ind + shift) % sz;
    saved_elem = numbers[ind];
    numbers[ind] = current_elem;
    current_elem = saved_elem;
    ++counter;
  }
}

template <size_t sz_1, size_t sz_2>
std::array<int, sz_1 + sz_2> MergeArrays(std::array<int, sz_1>& arr_1,
                 std::array<int, sz_2>& arr_2) {

  const size_t kTotalSize = sz_1 + sz_2;
  std::array<int, kTotalSize> merge_arr;

  size_t first_ptr = 0, second_ptr = 0, ind = 0;
  int smaller_el = 0;

  while (first_ptr < sz_1 && second_ptr < sz_2) {
    if (arr_1[first_ptr] > arr_2[second_ptr]) {
      smaller_el = arr_2[second_ptr];
      ++second_ptr;
    } else {
      smaller_el = arr_1[first_ptr];
      ++first_ptr;
    }

    merge_arr[ind] = smaller_el;
    ++ind;
  }

  while (first_ptr < sz_1) {
    merge_arr[ind++] = arr_1[first_ptr++];
  }
  while (second_ptr < sz_2) {
    merge_arr[ind++] = arr_2[second_ptr++];
  }

  return merge_arr;
}

template <size_t sz>
int FindZero(const std::array<int, sz>& arr) {
  int return_val = (arr[0] == 0 ? 0 : -1);

  for (size_t ind = 0; ind < sz && arr[ind] != 0; ++ind) {
    return_val = static_cast<int>(ind);
  }

  return return_val;
}

template <size_t sz_1, size_t sz_2>
void CheapMergeArrays(std::array<int, sz_1>& arr_1,
                      const std::array<int, sz_2>& arr_2) {

  if (sz_1 < sz_2 || (sz_1 == sz_2 && sz_1 == 0)) {
    throw std::runtime_error("CheapMergeArrays:"
                             "Not enough space for merge arrays!"
    );
  }

  int zero_ind = FindZero(arr_1);
  if (zero_ind== -1) {
    throw std::runtime_error("CheapMergeArrays: Zero element wasn't found!");
  }

  size_t ptr_1 = static_cast<size_t>(zero_ind);
  std::cout << "ptr_1 = " << ptr_1 << '\n';
  size_t ptr_2 = sz_2 - 1;
  size_t ptr = sz_1 - 1;

  while (ptr_1 > 0 && ptr_2 > 0) {
    if (arr_1[ptr_1] > arr_2[ptr_2]) {
      arr_1[ptr] = arr_1[ptr_1];
      --ptr_1;
    } else {
      arr_1[ptr] = arr_2[ptr_2];
      --ptr_2;
    }
    --ptr;
  }

  while (ptr_1 >= 0) {
    arr_1[ptr] = arr_1[ptr_1];
    if (ptr_1 == 0) {
      break;
    }
    --ptr;
    --ptr_1;
  }
  while (ptr_2 >= 0) {
    arr_1[ptr] = arr_2[ptr_2];
    if (ptr_2 == 0) {
      break;
    }
    --ptr;
    --ptr_2;
  }
}


#endif
