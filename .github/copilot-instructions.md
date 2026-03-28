# Copilot Instructions for cpp-physics-simulation

## Project intent

This repository contains progressive physics simulations written in C++20.
The current executable module is `project1_balistica`, built with CMake and
rendered with Raylib, Dear ImGui, rlImGui, and GLM.

When generating or editing code in this repository, prioritize code integrity,
build stability, and architectural consistency over large refactors.

## Repository boundaries

- Treat `external/` as vendored or generated dependency content. Do not edit it
  unless the task explicitly requires dependency maintenance.
- Treat `build/` as generated output. Do not rely on it as source of truth and
  do not hand-edit generated files.
- Main source code lives in `project1_balistica/src/` and shared definitions
  live in `core/`.
- Existing source groups are intentional: `app/`, `physics/`, `simulation/`,
  `rendering/`, and `ui/`.

## Architecture rules

- Keep the current separation of concerns:
  - `physics/` contains simulation domain state and integration logic.
  - `simulation/` coordinates execution and time stepping.
  - `rendering/` draws to screen and adapts coordinates for visualization.
  - `ui/` handles Dear ImGui interaction and parameter editing.
  - `app/` wires the application together and owns the main loop.
- Do not move rendering or UI logic into physics classes.
- Do not add hidden global state when a dependency can be passed explicitly.
- Prefer small, local changes that preserve existing behavior unless the task
  explicitly asks for a behavioral change.

## C++ conventions already used in this project

- Use C++20-compatible code only.
- Preserve the `physim` namespace for project code.
- Follow the current header style with classic include guards, not `#pragma once`.
- Match the existing naming style:
  - Types and classes use `PascalCase`.
  - Methods use `camelCase`.
  - Constants may use upper snake case when already established.
- Prefer `float` for simulation values unless there is a clear numerical reason
  to widen precision.
- Keep units explicit and consistent with the existing codebase:
  - position in meters
  - time in seconds
  - speed in meters per second
  - gravity in meters per second squared
  - mass in kilograms
- Use `const` references for read-only parameters where appropriate, following
  the existing code style.
- Keep comments short and useful. New comments should be written in English.

## Change safety rules

- Fix root causes instead of layering ad hoc patches.
- Avoid unrelated refactors while addressing a focused task.
- Do not silently change public behavior, units, or coordinate conventions.
- If changing a public interface, update all call sites in the same change.
- Preserve current build presets and cross-platform intent, especially the
  Windows presets and required system libraries configured in CMake.
- Do not introduce new third-party dependencies unless clearly justified.

## CMake rules

- Keep the existing CMake structure intact unless the task is specifically about
  build system maintenance.
- Respect the current project configuration:
  - root `CMakeLists.txt` defines the C++20 standard and shared output folders
  - dependencies are managed through `cmake/deps.cmake` and `FetchContent`
  - `tests/` and `project1_balistica/` are added as subdirectories
- When adding new source files for `project1_balistica`, place them under the
  existing source tree so they remain compatible with the current source glob.
- Do not remove Windows-specific libraries such as `winmm`, `gdi32`, and
  `opengl32` from targets that already require them.

## Validation expectations

- After meaningful code changes, validate the affected target when possible.
- Prefer the existing CMake presets instead of inventing new build commands.
- If a change affects simulation logic, verify that it does not break launch,
  stepping, landing, or trajectory recording behavior.
- If a change affects rendering or UI, verify that it does not break the main
  loop, camera flow, or ImGui integration.
- If a change is testable, add or update focused tests instead of relying only
  on manual inspection.
- If validation cannot be run, state that clearly.

## Preferred implementation behavior for AI agents

- Read the nearby code before editing.
- Reuse existing types and helpers before creating new abstractions.
- Preserve file organization and include ordering patterns already present in
  the touched files.
- Keep patches small and reviewable.
- Do not edit formatting-only details in unrelated code.
- Do not modify generated, vendored, or dependency files as part of normal
  feature work.

## Output quality bar

Before considering a task complete, check that:

- the code matches the current architecture
- the change is minimal and coherent
- naming and style match the surrounding code
- the build configuration remains consistent
- any new comments or documentation are in English
- validation status is explicit
