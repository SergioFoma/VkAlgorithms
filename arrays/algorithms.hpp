#ifndef ALGORITHMS_HPP_
#define ALGORITHMS_HPP_

#include <array>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <stdio.h>

struct TwoSumRes {
  int first = 0;
  int second = 0;
};

template <size_t sz>
TwoSumRes TwoSum(const std::array<int, sz>& nums, int target) {
  if (sz == 0) {
    return {};
  }

  size_t left = 0, right = sz - 1;
  int current_sum = 0;

  TwoSumRes res = {};
  bool is_find = false;

  while (!is_find && left < right) {
    current_sum = nums[left] + nums[right];

    if (current_sum > target) {
      --right;
    } else if (current_sum < target) {
      ++left;
    } else {
      res = {nums[left], nums[right]};
      is_find = true;
    }
  }

  return res;
}

template <size_t sz>
void Reverse(std::array<int, sz>& nums) {

  if (sz <= 1) {
    return ;
  }

  size_t middle = sz / 2;

  for (size_t ind = 0; ind < middle; ++ind) {
    std::swap(nums[ind], nums[sz - 1 - ind]);
  }
}

/*!
* counter - how many elements i processed
*/
template <size_t sz>
void RingShift(std::array<int, sz>& nums, size_t shift) {

  if (sz == 0) {
    return ;
  }

  size_t counter = 0, ind = 0;
  int saved_elem = nums[ind];
  int current_elem = saved_elem;
  while (counter < sz) {
    ind = (ind + shift) % sz;
    saved_elem = nums[ind];
    nums[ind] = current_elem;
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

template <size_t sz_1>
int IsValid(const std::array<int, sz_1>& arr_1, size_t sz_2) {

  if (sz_1 < sz_2 || (sz_1 == sz_2 && sz_1 == 0)) {
    throw std::runtime_error("CheapMergeArrays:"
                             "Not enough space for merge arrays!"
    );
  }

  int zero_ind = FindZero(arr_1);
  if (zero_ind== -1) {
    throw std::runtime_error("CheapMergeArrays: Zero element wasn't found!");
  }

  return zero_ind;
}

template <size_t sz_1, size_t sz_2>
void CheapMergeArrays(std::array<int, sz_1>& arr_1,
                      const std::array<int, sz_2>& arr_2) {

  int ptr_1 = IsValid(arr_1, sz_2);
  int ptr_2 = static_cast<int>(sz_2) - 1;
  int ptr = static_cast<int>(sz_1) - 1;

  while (ptr_1 > -1 && ptr_2 > -1) {
    if (arr_1[ptr_1] > arr_2[ptr_2]) {
      arr_1[ptr] = arr_1[ptr_1];
      --ptr_1;
    } else {
      arr_1[ptr] = arr_2[ptr_2];
      --ptr_2;
    }
    --ptr;
  }

  while (ptr_1 > -1) {
    arr_1[ptr--] = arr_1[ptr_1--];
  }
  while (ptr_2 > -1) {
    arr_1[ptr--] = arr_2[ptr_2--];
  }
}

template <size_t sz>
std::vector<int> MinSubarray(const std::array<int, sz>& nums, int target) {

  size_t left_ptr = 0;
  size_t prev_left = 0, prev_right = sz;
  int current_sum = 0;
  bool is_find = false;

  for (size_t right_ptr = 0; right_ptr < sz; ++right_ptr){
    current_sum += nums[right_ptr];

    while (current_sum >= target) {
      if (right_ptr - left_ptr < prev_right - prev_left) {
        prev_right = right_ptr;
        prev_left = left_ptr;
      }
      current_sum -= nums[left_ptr];
      ++left_ptr;
      is_find = true;
    }
  }

  std::vector<int> subarray;
  for (size_t ind = prev_left; is_find && ind <= prev_right; ++ind) {
    subarray.push_back(nums[ind]);
  }

  return subarray;
}

template<size_t sz>
void Sort(std::array<int, sz>& nums) {

  int zeros_counter = 0;
  for (int el: nums) {
    if (el == 0) ++zeros_counter;
  }

  for (int counter = 0; counter < zeros_counter; ++counter) {
    nums[counter] = 0;
  }
  for (int counter = zeros_counter; counter < sz; ++counter) {
    nums[counter] = 1;
  }
}

template <size_t sz>
void NetherlandsFlag(std::array<int ,sz>& nums) {

  int unit_counter = 0;
  int zeros_counter = 0;

  for (int el: nums) {
    if (el == 0) ++zeros_counter;
    if (el == 1) ++unit_counter;
  }
  int upper_bound = zeros_counter + unit_counter;

  for (int zer_ind = 0; zer_ind < zeros_counter; ++zer_ind) {
    nums[zer_ind] = 0;
  }
  for (int unit_ind = zeros_counter; unit_ind < upper_bound; ++unit_ind) {
    nums[unit_ind] = 1;
  }
  for (int two_ind = upper_bound; two_ind < sz; ++two_ind) {
    nums[two_ind] = 2;
  }
}

template <size_t sz>
void SortEven(std::array<int, sz>& nums) {

  size_t first_even_ind = 0, first_uneven_ind = 0;
  bool is_find_even = false, is_find_uneven = false;

  for (size_t ind = 0; ind < sz && !is_find_even && !is_find_uneven; ++ind) {
    if (!is_find_even && nums[ind] % 2 == 0) {
      first_even_ind = ind;
      is_find_even = true;
    } else if (!is_find_uneven && nums[ind] % 2 == 0) {
      first_uneven_ind = ind;
      is_find_uneven = true;
    }
  }

  size_t even_ptr = first_even_ind, uneven_ptr = first_uneven_ind;
  while (even_ptr < sz && uneven_ptr < sz) {

    if (uneven_ptr < even_ptr &&
        nums[even_ptr] % 2 == 0 &&
        nums[uneven_ptr] % 2 != 0 ) {
      std::swap(nums[even_ptr], nums[uneven_ptr]);
    }

    if (nums[even_ptr] % 2 != 0)   ++even_ptr;
    if (nums[uneven_ptr] % 2 == 0) ++uneven_ptr;
  }
}

template <size_t sz>
void MoveZeros(std::array<int ,sz>& nums) {

  size_t notzero_ind = 0, zero_ind = 0;
  bool is_find_notzero = false, is_find_zero = false;

  for (size_t ind = 0; ind < sz && !is_find_notzero && !is_find_zero; ++ind) {
    if (!is_find_notzero && nums[ind] != 0) {
      notzero_ind = ind;
      is_find_notzero = true;
    } else if (!is_find_zero && nums[ind] == 0) {
      zero_ind = ind;
      is_find_zero = true;
    }
  }

  size_t zero_ptr = zero_ind, notzero_ptr = notzero_ind;
  while (zero_ptr < sz && notzero_ptr < sz) {

    if (zero_ptr < notzero_ptr &&
        nums[zero_ptr] == 0 &&
        nums[notzero_ptr] != 0 ) {
      std::swap(nums[zero_ptr], nums[notzero_ptr]);
    }

    if (nums[zero_ptr] != 0)    ++zero_ptr;
    if (nums[notzero_ptr] == 0) ++notzero_ptr;
  }
}

#endif
