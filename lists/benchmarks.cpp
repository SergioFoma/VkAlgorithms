#include <benchmark/benchmark.h>
#include <algorithm>
#include <stack>
#include <string>

#include "algorithms.hpp"

namespace {

bool ClassworkIsPalindrome(const std::string& str) {
  std::stack<char> str_stack;

  for (char symbol: str) {
    str_stack.push(symbol);
  }

  for (char symbol: str) {
    if (symbol != str_stack.top()) {
      return false;
    }
    str_stack.pop();
  }

  return true;
}

} // namespace

void BmClasswork1(benchmark::State& state) {
  for (auto st: state) {
    std::string first_case = "racecar";
    ClassworkIsPalindrome(first_case);
  }
}

void BmMySolution1(benchmark::State& state) {
  for (auto st: state) {
    std::string first_case = "racecar";
    algorithm::IsPalindrome(first_case);
  }
}

void BmClasswork2(benchmark::State& state) {
  for (auto st: state) {
    std::string first_case = "hello";
    ClassworkIsPalindrome(first_case);
  }
}

void BmMySolution2(benchmark::State& state) {
  for (auto st: state) {
    std::string first_case = "hello";
    algorithm::IsPalindrome(first_case);
  }
}

void BmClasswork3(benchmark::State& state) {
  for (auto st: state) {
    std::string first_case = "tattarrattattattarrattattattarrattattattarrattattattarrattat";
    ClassworkIsPalindrome(first_case);
  }
}

void BmMySolution3(benchmark::State& state) {
  for (auto st: state) {
    std::string first_case = "tattarrattattattarrattattattarrattattattarrattattattarrattat";
    algorithm::IsPalindrome(first_case);
  }
}

BENCHMARK(BmClasswork1);
BENCHMARK(BmMySolution1);

BENCHMARK(BmClasswork2);
BENCHMARK(BmMySolution2);

BENCHMARK(BmClasswork3);
BENCHMARK(BmMySolution3);
