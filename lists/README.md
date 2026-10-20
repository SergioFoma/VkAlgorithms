# Очереди. Деки и стеки.
# Связные списки. HW2

```
├── list.hpp                  # реализация вспомогательного списка
├── algorithms.cpp            # определение алгоритмов
├── algorithms.hpp            # объявление алгоритмов
├── algorithm_tests.cpp       # тесты алгоритмов
├── benchmarks.cpp            # бенч марки
├── list_tests.cpp            # тесты на список
├── CMakeLists.txt            # сборка проекта
└── README.md
```

## Сборка проекта

для сборки проекта необходимо перейти в папку ```VkAlgorithms/lists```

Команды для сборки и запуска:

```bash
cmake -S . -B Build -DCMAKE_BUILD_TYPE=Release
```

```bash
cmake --build Build --parallel
```

**Запуск тестов списка:**
```bash
Build/list_tests
```

**Запуск тестов алгоритмов:**
```bash
Build/algorithm_tests
```

**Запуск google benchmarks:**
```bash
Build/benchmark_target
```
