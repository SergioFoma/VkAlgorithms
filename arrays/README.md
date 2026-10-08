# Фундаментальные задача по массивам. HW1

```
├── CMakeLists.txt      # сборка проекта
├── README.md           # описание решения
├── algorithms.hpp      # хедер с алгоритмами
├── benchmark.cpp       # замер, насколько моя реализация быстрее предложенной в презентации
├── main.cpp            # первичные небольшие тесты
└── tests.cpp           # google tests

```

## Сборка проекта

для сборки проекта необходимо перейти в папку ```VkAlgorithms/arrays```

Команды для сборки и запуска:

```bash
cmake -S . -B Build -DCMAKE_BUILD_TYPE=Release
```

```bash
cmake --build Build --parallel
```

**Запуск main.cpp:**
```bash
Build/algorithms
```

**Запуск google тестов:**
```bash
Build/algorithm_tests
```

**Запуск google benchmark:**
```bash
Build/benchmark_target
```
