#include <cstddef>

#include "algorithms.hpp"


bool algorithm::FindSubstr(const std::string& dest, const std::string& src) {

  size_t dest_len = dest.size(), src_len = src.size();
  if (src_len > dest_len) {
    return false;
  }

  size_t dest_ptr = 0, src_ptr = 0;

  while (dest_ptr < dest_len && src_ptr < src_len) {
    if (src[src_ptr] == dest[dest_ptr]) {
      ++src_ptr;
    }
    ++dest_ptr;
  }

  return src_ptr == src_len;
}

bool algorithm::IsPalindrome(const std::string& str) {

  if (str.empty()) {
    return false;
  }

  size_t sz = str.size();
  size_t middle = sz / 2;

  bool is_palindrome = true;
  for (size_t ind = 0; ind < middle && is_palindrome; ++ind) {
    is_palindrome = (str[ind] == str[sz - 1 - ind]);
  }

  return is_palindrome;
}
