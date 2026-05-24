# TimeEcho Snake

Змейка с эхо-препятствиями. Собирайте еду, уклоняйтесь от своих прошлых копий.

**[Играть](https://xver1xrd.github.io/snake/)**

## Управление

WASD или стрелки — движение
ESC — меню
Enter / Space — выбрать / рестарт

## Сборка

```bash
# Нативная
cmake -S . -B build && cmake --build build
./build/TimeEchoSnake

# Web (нужен emsdk)
source emsdk/emsdk_env.sh
em++ -std=c++17 src/*.cpp /tmp/raylib-wasm-install/lib/libraylib.a \
  -I/tmp/raylib-wasm-install/include -I src \
  -s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=64MB \
  --shell-file web/shell.html -o build-web/index.html -O3
```

## Зависимости

- raylib 5.0 (ставится автоматически через FetchContent)
- Emscripten (для веб-сборки)
