# TimeEcho Snake 🐍🕒

Классическая змейка с механикой «временных эхо» — призрачные копии прошлых позиций постепенно заполняют поле.

**Играть:** [xver1xrd.github.io/snake](https://xver1xrd.github.io/snake/)

## Геймплей

- Собирайте еду, избегайте стен, хвоста и эхо-препятствий
- **🟢 Обычная еда** — +10 очков
- **🔵 Кристалл времени** — замедляет время на 5 сек
- **🟣 Призрачная сфера** — позволяет проходить сквозь эхо 5 сек
- Скорость постепенно растёт, эхо накапливаются

## Управление

| Клавиши | Действие |
|---------|----------|
| WASD / Стрелки | Движение |
| ESC | Меню / Пауза |
| Space / Enter | Выбрать / Рестарт |

## Сборка

### Нативная (Linux)

```bash
cmake -S . -B build
cmake --build build
./build/TimeEchoSnake
```

### WebAssembly

```bash
source emsdk/emsdk_env.sh

# Сборка raylib под WASM (однократно)
cmake -S /tmp/raylib-wasm -B /tmp/raylib-wasm-build \
  -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake \
  -DPLATFORM=Web
cmake --build /tmp/raylib-wasm-build
cmake --install /tmp/raylib-wasm-build --prefix /tmp/raylib-wasm-install

# Сборка игры
em++ -std=c++17 src/*.cpp /tmp/raylib-wasm-install/lib/libraylib.a \
  -I/tmp/raylib-wasm-install/include -I src \
  -s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=64MB \
  --shell-file web/shell.html -o build-web/index.html -O3
```

## Технологии

- C++17, raylib 5.0, CMake
- Emscripten (WebAssembly)
- GitHub Pages

## Структура

```
src/
├── main.cpp      — точка входа
├── Game.cpp/h    — логика игры, меню, эхо, частицы
├── Snake.cpp/h   — змейка
web/shell.html    — HTML-шаблон для WASM
```
