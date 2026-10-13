#ifndef ALGORITHMS_HPP_
#define ALGORITHMS_HPP_

#include <string>
#include <stack>
#include <vector>
#include <utility>
#include <algorithm>

#include "list.hpp"

namespace algorithm {

template <typename T>
bool IsRing(const list::List<T>& list) {

  if (list.Empty()) {
    return false;
  }

  auto slow = &(list.Front());
  auto fast = list.Front().next;

  while (fast != nullptr && slow != fast) {
    slow = slow->next;
    if (fast->next != nullptr) {
      fast = fast->next->next;
    } else {
      fast = fast->next;
    }
  }

  if (fast == nullptr) {
    return false;
  }
  return true;
}

template <typename T>
void Reverse(list::List<T>& list) {

  if (list.Empty()) {
    return ;
  }

  auto current = &(list.Front());
  list::Node<T>* prev = nullptr;
  list::Node<T>* next = nullptr;

  while (current->next != nullptr) {
    next = current->next;
    current->next = prev;
    prev = current;
    current = next;
  }

  current->next = prev;
  list.SetNewHead(current);
}

template <typename T>
list::Node<T>* FindMiddle(const list::List<T>& list) {

  if (list.Empty()) {
    return nullptr;
  }

  auto slow = &(list.Front());
  auto fast = slow;

  while (fast != nullptr && fast->next != nullptr) {
    slow = slow->next;
    fast = fast->next->next;
  }

  return slow;
}

template <typename T>
list::Node<T>* DeleteElem(list::List<T>& list, const T& val) {
  return list.Erase(val);
}

bool FindSubstr(const std::string& dest, const std::string& src);

bool IsPalindrome(const std::string& str);

template <typename T>
std::stack<T> SortByStack(std::vector<T>& vec) {
  std::stack<T> stack;

  while (!vec.empty()) {
    T back = std::move(vec.back());
    vec.pop_back();

    while (!stack.empty() && back < stack.top()) {
      vec.push_back(std::move(stack.top()));
      stack.pop();
    }

    stack.push(std::move(back));
  }

  return stack;
}

template <typename T>
list::List<T>& ConnectingLists(list::List<T>& first, list::List<T>& second) {

  if (first.Empty()) {
    return second;
  } else if (second.Empty()) {
    return first;
  }

  auto& first_head = first.Front();
  auto& second_head = second.Front();

  list::Node<T>* next = &second_head;
  if (second_head.val < first_head.val) {
    first.SetNewHead(&second_head);
    next = &first_head;
  }

  list::Node<T>* current = &(first.Front());

  while (current->next != nullptr && next != nullptr) {
    if (current->next->val <= next->val) {
      current = current->next;
    } else {
      std::swap(current->next, next);
    }
  }

  while (current->next == nullptr) {
    current->next = next;
  }

  return first;
}


} // namespace algorithm

#endif
