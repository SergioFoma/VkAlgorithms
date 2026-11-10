# Бинарные деревья. HW4

```
├── CMakeLists.txt                  # сборка проекта
├── create_tree.hpp                 # создание бинарного дерева
├── main.cpp                        # main
├── min_depth.hpp                   # нахождение минимальной глубины
|── find_extremes.hpp               # нахождение произведения максимума и минимума дерева
├── compare_trees.hpp               # сравнение деревьев        
├── symmetrical_check.hpp           # проверка на симметричность
├── tree.hpp                        # объявление дерева
└── unit_tests.cpp                  # unit тесты
```

## Сборка проекта

для сборки проекта необходимо перейти в папку ```VkAlgorithms/trees```

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
