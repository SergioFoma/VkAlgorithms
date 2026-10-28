#include <algorithm>
#include <unordered_map>

#include "algorithms.hpp"

static constexpr size_t kZero = 0.0;

size_t algorithms::PrintPages(size_t n, size_t x, size_t y)  {
  if (n == 0) {
    return kZero;
  } else if (x == kZero) {
    return n * y;
  } else if (y == kZero) {
    return n * x;
  }

  size_t first_copy = std::min(x, y);

  if (n == 1) {
    return first_copy;
  }

  size_t n_left = n - 1;
  size_t left = 0, right = n_left * std::max(x, y);

  while (left + 1 < right) {
    size_t middle = (left + right) / 2;
    if (middle / x + middle / y < n_left) {
      left = middle;
    } else {
      right = middle;
    }
  }

  return right + first_copy;
}

size_t algorithms::FeedAnimals(std::vector<size_t>& animals,
                               std::vector<size_t>& food) {

  size_t animals_sz = animals.size();
  size_t food_sz = food.size();

  if (animals_sz == kZero || food_sz == kZero) {
    return kZero;
  }

  std::sort(animals.begin(), animals.end());
  std::sort(food.begin(), food.end());

  int animals_ind = animals_sz - 1;
  int food_ind = food_sz - 1;
  size_t counter = 0;

  while (animals_ind > - 1 && food_ind > -1) {
    if (food[food_ind] >= animals[animals_ind]) {
      --animals_ind;
      --food_ind;
      ++counter;
    } else {
      --animals_ind;
    }
  }

  return counter;
}

char algorithms::ExtraLetter(const std::string& a,
                             const std::string& b) {
  if (b.size() < a.size()) {
    throw std::runtime_error("String b is smaller than string a!");
  }

  char empty_symbol = '\0';
  if (b.empty() || a.size() == b.size()) {
    return empty_symbol;
  }

  std::unordered_map<char, int> map_b;

  for (const auto& symbol: b) {
    ++map_b[symbol];
  }
  for (const auto& symbol: a) {
    if (map_b.find(symbol) != map_b.end()) {
      --map_b[symbol];
    }
  }
  for (const auto& pair: map_b) {
    if (pair.second > 0) {
      return pair.first;
    }
  }

  return empty_symbol;
}

std::vector<int> algorithms::TwoSum(const std::vector<int>& nums, int target) {
  std::vector<int> result{};

  if (nums.empty()) {
    return result;
  }

  // first - value, second - counter;
  std::unordered_map<int, int> map;

  for (const auto& num: nums) {
    ++map[num];
  }

  int init_counter = 1;
  for (const auto& pair: map) {
    if ((target != pair.first * 2 && map.find(target - pair.first) != map.end()) ||
        (target == pair.first * 2 && map[pair.first] > init_counter)) {
          result.push_back(pair.first);
          result.push_back(target -  pair.first);
          break;
    }
  }

  return result;
}

std::vector<std::vector<std::string>> algorithms::GroupAnagrams(
  const std::vector<std::string>& words
) {

  std::vector<std::vector<std::string>> groups{};

  std::unordered_multimap<std::string, std::string> map;

  for (const auto& str: words) {
    std::string sorted_str = str;
    std::sort(sorted_str.begin(), sorted_str.end());
    map.insert({sorted_str, str});
  };

  for (auto it = map.begin(); it != map.end();) {
    const auto& range = map.equal_range(it->first);

    std::vector<std::string> word_list{};
    for (auto list_it = range.first; list_it != range.second; ++list_it) {
      word_list.push_back(list_it->second);
    }
    groups.push_back(word_list);

    it = range.second;
  }

  return groups;
}
