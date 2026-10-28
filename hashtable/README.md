# Сортировки. Хеш - таблицы. HW3

```
├── algorithm_tests.cpp   # unit тесты алгоритмов
├── algorithms.cpp        # реализация алгоритмов
├── algorithms.hpp        # объявления и шаблонные функции
├── CMakeLists.txt        # сборка проекта
├── main.cpp              # main
└── README.md

```

## Сборка проекта

для сборки проекта необходимо перейти в папку ```VkAlgorithms/hashtable```

Команды для сборки и запуска:

```bash
cmake -S . -B Build
```

```bash
cmake --build Build
```

**Запуск main.cpp:**
```bash
Build/target
```

**Запуск google тестов:**
```bash
Build/algorithm_tests
```
