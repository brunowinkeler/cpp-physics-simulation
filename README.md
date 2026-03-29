# cpp-physics-simulation

Progressive physics simulations in C++20, starting with a ballistic projectile module rendered with Raylib and Dear ImGui.

## Stack

| Role | Library | Version |
|---|---:|---:|
| Rendering / Window / Input | [Raylib](https://www.raylib.com/) | 5.5 |
| Real-time GUI | [Dear ImGui](https://github.com/ocornut/imgui) | 1.92.6 |
| Bridge Raylib ↔ ImGui | [rlImGui](https://github.com/raylib-extras/rlImGui) | 286e11a |
| Vector / matrix math | [GLM](https://github.com/g-truc/glm) | 1.0.1 |

Dependencies are fetched and built by CMake through `FetchContent`.

## Current Projectile Module

- Supported integration methods: Runge-Kutta 4 and Symplectic Euler.
- Simulation states: `Idle`, `Running`, `Paused`, and `Landed`.
- Launch parameters are editable only while the simulation is idle.
- Launch history archives unique runs, keeps trajectory styling stable, and stores summary metrics such as range, flight time, and apex.
- Current trajectory recording uses an explicit retention policy: samples are recorded at up to 240 Hz and compacted to a maximum of 4096 points per trajectory.

## Prerequisites

Linux native build:

```bash
sudo apt install cmake ninja-build
```

Linux to Windows cross-compile with MinGW:

```bash
sudo apt install cmake ninja-build mingw-w64
```

## Build Presets

Available configure and build presets:

- `linux-debug`: Linux debug build with sanitizers enabled.
- `linux-release`: Linux release build.
- `windows-debug`: MinGW cross-compiled Windows debug build.
- `windows-release`: MinGW cross-compiled Windows release build.

## Build And Run

Linux debug:

```bash
cmake --preset linux-debug
cmake --build --preset linux-debug
./build/linux-debug/bin/Project1_Balistica
```

Linux release:

```bash
cmake --preset linux-release
cmake --build --preset linux-release
./build/linux-release/bin/Project1_Balistica
```

Windows release cross-build:

```bash
cmake --preset windows-release
cmake --build --preset windows-release
```

Generated Windows executable:

```text
build/windows-release/bin/Project1_Balistica.exe
```

## Tests

Configure and build the Linux debug preset first:

```bash
cmake --preset linux-debug
cmake --build --preset linux-debug
ctest --test-dir build/linux-debug --output-on-failure
```

Registered tests:

- `projectile_physics_domain`: deterministic domain tests for integration, landing interpolation, state transitions, history archival, and trajectory retention.
- `smoke_graphics`: short graphical smoke run through Raylib, ImGui, and rlImGui.

Notes about the smoke test:

- It accepts `--ctest` and exits automatically after a few frames.
- It skips cleanly when no graphical display is available.
- Leak detection is disabled only for this test under ASan because the observed reports come from the external graphics stack, not from project-owned simulation code.

## Controls

Simulation:

- `Space`: start or pause the current launch.
- `R`: reset the current launch.

Camera:

- `W`, `A`, `S`, `D`: pan.
- `Q`, `E`: zoom out and zoom in.
- `F`: reset camera.
- Mouse drag: pan.
- Mouse wheel: zoom.

## Architecture

Source layout follows the current module boundaries in `project1_balistica/src/`:

- `app/`: application bootstrap and frame loop orchestration.
- `physics/`: projectile state, forces, and numerical integration.
- `simulation/`: simulation time stepping, state machine, trajectory recording, launch history, and the `SimulationSession` that groups domain state.
- `rendering/`: camera, coordinate adapters, world rendering, overlay rendering, and trajectory visualization.
- `ui/`: Dear ImGui parameter editing and launch history interaction.

High-level flow:

1. `Application` owns a `SimulationSession` and orchestrates input, stepping, drawing, and UI.
2. `Simulation` advances the projectile with a fixed physics timestep and archives completed launches.
3. `SimulationSceneRenderer` renders the world, projectile, and trajectories.
4. `SimulationOverlayRenderer` renders HUD information and interaction hints.
5. `UiMenus` edits launch parameters and inspects archived runs.

## Coordinate System

- Physics uses world coordinates with `y` increasing upward.
- Rendering uses a single adapter, `ProjectileCoordinatesAdapter`, to map world space into Raylib draw space.
- Background, projectile, and trajectory renderers all go through the same conversion path, so pan and zoom remain coherent.

## Known Limitations

- Archived trajectories keep display-oriented sampled points, not every raw integrator step.
- The graphical smoke test still depends on a valid display server when it is not running in skip mode.
- The repository currently ships only the ballistic module as the active executable.
