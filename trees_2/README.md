# Кучи и бинарные деревья. HW5

```
├── CMakeLists.txt                  # build project
├── main.cpp                        # main
├── heap.hpp                        # heap implementation
├── tree.hpp                        # tree implementation
├── completeness_checking.hpp       # checking for a complete tree
├── merge_arrays.hpp                # merge k sorted arrays
├── find_min.hpp                    # find kth minimum (use min heap)
├── bst_find.hpp                    # find kth minimum (use tree)
├── set_balance_factor.hpp          # set balance factor in binary tree
├── mirror_tree.hpp                 # transformed into a mirror tree
├── README.md
└── unit_tests.cpp                  # unit tests
```

## Сборка проекта

для сборки проекта необходимо перейти в папку ```VkAlgorithms/trees_2```

Команды для сборки и запуска:

```bash
cmake -S . -B Build
```

```bash
cmake --build Build
```

**Запуск google тестов:**
```bash
Build/unit_tests
```
