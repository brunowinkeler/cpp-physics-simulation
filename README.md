# cpp-physics-simulation

Progressive physics simulations in C++20 — from projectile launch to SPH fluids.

## Stack

| Role | Library | Version |
|---|---:|---:|
| Rendering / Window / Input | [Raylib](https://www.raylib.com/) | 5.5 |
| Real-time GUI (sliders, buttons) | [Dear ImGui](https://github.com/ocornut/imgui) | 1.92.6 |
| Bridge Raylib ↔ ImGui | [rlImGui](https://github.com/raylib-extras/rlImGui) | main |
| Vector / matrix math | [GLM](https://github.com/g-truc/glm) | 1.0.1 |

Dependencies are downloaded and built automatically by CMake using
`FetchContent` — no manual installation required.

---

## Prerequisites (cross-compile Linux → Windows)

```bash
# Ubuntu / Debian
sudo apt install cmake ninja-build mingw-w64
```

---

## Build

```bash
# Configure and build for Windows (Release)
cmake --preset windows-release
cmake --build --preset windows-release

# The produced executable:
# build/windows-release/bin/PhysicsSimulation.exe
```

For a Debug build (with symbols):

```bash
cmake --preset windows-debug
cmake --build --preset windows-debug
```
