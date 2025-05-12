# PostBoy

## Сборка и запуск
Сборка:
```bash
cmake --build build --config Release --target postboy
```
Запуск:
```bash
build/bin/postboy
```

## Code style

Общий code style описан в файлах [.clang-format](.clang-format), [.clang-tidy](.clang-tidy).

В [scripts](scripts) есть [clang_format.sh](scripts/clang_format.sh), который применяет clang-format для всех файлов.
