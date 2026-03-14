cmake_minimum_required(VERSION 3.20)

# CMake 4.x: allow FetchContent dependencies to use older cmake_minimum_required
# calls (e.g. Raylib may declare cmake_minimum_required(VERSION 3.0)).
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(PhysicsSimulation
    VERSION 0.1.0
    LANGUAGES C CXX
)

# ---------------------------------------------------------------------------
# C++ Standard
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Output directories
# ---------------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ---------------------------------------------------------------------------
# Dependencies (Raylib, ImGui, rlImGui, GLM)
# ---------------------------------------------------------------------------
include(cmake/deps.cmake)

# ---------------------------------------------------------------------------
# Executable
# ---------------------------------------------------------------------------
add_executable(PhysicsSimulation
    src/main.cpp
)

# rlImGui needs to be compiled alongside ImGui headers
target_sources(PhysicsSimulation PRIVATE
    ${rlimgui_SOURCE_DIR}/rlImGui.cpp
)

target_include_directories(PhysicsSimulation PRIVATE
    ${rlimgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}
)

target_link_libraries(PhysicsSimulation PRIVATE
    raylib
    imgui
    glm::glm
)

# ---------------------------------------------------------------------------
# Windows-specific settings
# ---------------------------------------------------------------------------
if(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Link Windows system libs needed by Raylib
    target_link_libraries(PhysicsSimulation PRIVATE
        winmm
        gdi32
        opengl32
    )

    # Static runtime for MinGW (avoids distributing DLLs)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_link_options(PhysicsSimulation PRIVATE -static-libgcc -static-libstdc++)
    endif()
endif()
