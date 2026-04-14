# cpp-physics-simulation

Progressive physics simulations in C++20, currently including a ballistic projectile module, a pendulum laboratory, and a gas and Brownian motion laboratory rendered with Raylib and Dear ImGui.

## Stack

| Role | Library | Version |
|---|---:|---:|
| Rendering / Window / Input | [Raylib](https://www.raylib.com/) | 5.5 |
| Real-time GUI | [Dear ImGui](https://github.com/ocornut/imgui) | 1.92.6 |
| Bridge Raylib ↔ ImGui | [rlImGui](https://github.com/raylib-extras/rlImGui) | 286e11a |
| Vector / matrix math | [GLM](https://github.com/g-truc/glm) | 1.0.1 |

Dependencies are fetched and built by CMake through `FetchContent`.

## Current Modules

### Project 1 — Ballistics

- Supported integration methods: Runge-Kutta 4 and Symplectic Euler.
- Simulation states: `Idle`, `Running`, `Paused`, and `Landed`.
- Launch parameters are editable only while the simulation is idle.
- Launch history archives unique runs, keeps trajectory styling stable, and stores summary metrics such as range, flight time, and apex.
- Current trajectory recording uses an explicit retention policy: samples are recorded at up to 240 Hz and compacted to a maximum of 4096 points per trajectory.
- Ships with a project-local theory reference in [project1_balistica/PHYSICS_THEORY.md](project1_balistica/PHYSICS_THEORY.md).

### Project 2 — Pendulum Lab

- Includes both a simple pendulum model and a double pendulum model in the same executable.
- Supports Runge-Kutta 4 and Symplectic Euler for both models.
- Tracks energy, simulation time, and one or two trail recorders depending on the active mode.
- Pendulum trails keep a fixed-size rolling window of recent samples, avoiding the progressive line coarsening that came from whole-history compaction.
- Exposes gravity, damping, lengths, masses, initial conditions, visualization toggles, and fixed physics timestep through ImGui.
- Includes auxiliary analysis plots for energy history and phase-space trajectories.
- Ships with a project-local theory reference in [project2_pendulum/PHYSICS_THEORY.md](project2_pendulum/PHYSICS_THEORY.md).

### Project 3 — Gas And Brownian Lab

- Simulates a 2D ideal-gas-style particle bath with an optional Brownian tracer particle.
- Uses elastic wall and particle collisions plus a uniform collision grid for scalable broad-phase detection.
- Exposes gas presets inspired by real gases, particle counts from hundreds to thousands, fixed physics timestep, and tracer controls through ImGui.
- Includes an instantaneous-pressure time plot with sliding-window retention and an optional rolling tracer trajectory.
- Ships with a project-local theory reference in [project3_gas_brownian/PHYSICS_THEORY.md](project3_gas_brownian/PHYSICS_THEORY.md).

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
./build/linux-debug/bin/Project2_Pendulum
./build/linux-debug/bin/Project3_Gas_Brownian
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
build/windows-release/bin/Project2_Pendulum.exe
build/windows-release/bin/Project3_Gas_Brownian.exe
```

## Tests

Configure and build the Linux debug preset first:

```bash
cmake --preset linux-debug
cmake --build --preset linux-debug
ctest --test-dir build/linux-debug --output-on-failure
```

Registered tests:

- `common_physics_domain`: shared tests for integration-method plumbing and trajectory recording.
- `project1_balistica_domain`: deterministic domain tests for integration, landing interpolation, state transitions, and launch history.
- `project2_pendulum_domain`: deterministic domain tests for simple and double pendulum physics plus pendulum simulation stepping.
- `project3_gas_brownian_domain`: deterministic domain tests for gas collisions, macroscopic metrics, and Brownian-tracer history behavior.
- `smoke_graphics`: short graphical smoke run through Raylib, ImGui, and rlImGui.
- `project2_pendulum_smoke_graphics`: short graphical smoke run specific to the pendulum module.
- `project3_gas_brownian_smoke_graphics`: short graphical smoke run specific to the gas and Brownian module.

Test tree layout:

- `tests/common/`: project-independent or cross-project tests.
- `tests/project1_balistica/`: tests specific to the ballistic module.
- `tests/project2_pendulum/`: tests specific to the pendulum module.
- `tests/project3_gas_brownian/`: tests specific to the gas and Brownian module.

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

Repository layout currently follows these boundaries:

- `core/`: shared constants and the small set of cross-project components that are already stable enough to reuse.
- `project1_balistica/`: ballistic module, with `app/`, `physics/`, `simulation/`, `rendering/`, and `ui/` under `src/`.
- `project2_pendulum/`: pendulum module, with `app/`, `physics/`, `simulation/`, `rendering/`, and `ui/` under `src/`.
- `project3_gas_brownian/`: gas and Brownian module, with `app/`, `physics/`, `simulation/`, `rendering/`, and `ui/` under `src/`.
- `tests/`: common tests plus project-specific domain tests.

Each new project folder should contain its own `PHYSICS_THEORY.md` reference documenting the governing equations, assumptions, units, and validation targets for that project.

High-level flow:

1. `Application` owns a `SimulationSession` and orchestrates input, stepping, drawing, and UI.
2. `Simulation` advances the projectile with a fixed physics timestep and archives completed launches.
3. `SimulationSceneRenderer` renders the world, projectile, and trajectories.
4. `SimulationOverlayRenderer` renders HUD information and interaction hints.
5. `UiMenus` edits launch parameters and inspects archived runs.

High-level flow in Project 2:

1. `PendulumApplication` owns a `PendulumSession` and orchestrates input, stepping, drawing, and UI.
2. `PendulumSimulation` advances either the simple or double pendulum with a fixed physics timestep and records one or two trails.
3. `PendulumSceneRenderer` renders the grid, trails, rods, and masses.
4. `PendulumOverlayRenderer` renders HUD information and interaction hints.
5. `PendulumUiMenus` edits pendulum parameters and visualization settings.

High-level flow in Project 3:

1. `GasBrownianApplication` owns a `GasSession` and orchestrates input, stepping, drawing, and UI.
2. `GasSimulation` advances the particle bath with a fixed physics timestep, resolves collisions, computes macroscopic metrics, and records pressure and tracer histories.
3. `GasSceneRenderer` renders the box, particle bath, tracer trail, and optional grid.
4. `GasOverlayRenderer` renders HUD information and interaction hints.
5. `GasUiMenus` edits gas presets, particle counts, tracer settings, and inspects the pressure history.

## Coordinate System

- Physics uses world coordinates with `y` increasing upward.
- Rendering uses a single adapter, `ProjectileCoordinatesAdapter`, to map world space into Raylib draw space.
- Background, projectile, and trajectory renderers all go through the same conversion path, so pan and zoom remain coherent.

## Known Limitations

- Archived trajectories keep display-oriented sampled points, not every raw integrator step.
- The graphical smoke test still depends on a valid display server when it is not running in skip mode.
- The repository currently uses separate executables per project rather than a single launcher application.
