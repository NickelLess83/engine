# Engine

A 2D game engine written in C++20, built on SDL2 and OpenGL.

## Features

- **ECS** — entity/component/system architecture with sparse-set storage
- **Renderer** — sprite batching, debug draw, camera with zoom and pan
- **Audio** — spatial sound, music playback, crossfade, priority-based channel mixing
- **Collision** — AABB and circle detection with physics resolution
- **Input** — action-mapped keyboard input
- **Scenes** — scene stack with managed transitions and asset lifetimes

## Dependencies

| Library | Purpose |
|---|---|
| SDL2 | Window, input, audio backend |
| SDL2_mixer | Audio mixing |
| OpenGL / GLAD | Rendering |
| GLM | Math |
| stb_image | Texture loading |

GLAD and stb_image are vendored under `third_party/`. The rest must be installed on the host.

## Building

```bash
# Ubuntu / Debian
sudo apt-get install cmake libsdl2-dev libsdl2-mixer-dev libglm-dev libgl-dev pkg-config

# Arch
sudo pacman -S cmake sdl2 sdl2_mixer glm

cmake -B build
cmake --build build --parallel
./build/game/game
```

## Code quality

```bash
./check.sh          # clang-format + clang-tidy
./check.sh --fix    # auto-format, then tidy
```

CI runs the same checks on every pull request and blocks merges to `main` until they pass.

## Project structure

```
src/
  audio/        AudioSystem, Music, Sound
  collision/    CollisionSystem
  core/         Engine (main loop)
  ecs/          Registry, Entity, View, SparseSet, Components
  input/        InputManager
  renderer/     Camera, SpriteBatch, DebugDraw, ShaderProgram, Texture, Window
  scene/        SceneManager, Scene
game/
  src/          Sample game (GameScene)
assets/
  shaders/      GLSL shaders
third_party/
  glad/         OpenGL loader
  stb/          stb_image
```
