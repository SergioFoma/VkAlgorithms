#include <array>
#include <iostream>

#include "create_tree.hpp"
#include "min_depth.hpp"

int main() {

  std::array<int, 3> tree_arr = {1, 2, 3};

  tree::Tree<int> tree = tree::CreateTree(tree_arr);

  size_t min_depth = tree::FindMinDepth(tree);

  std::cout << "min_depth = " << min_depth << '\n';

  return 0;
}
