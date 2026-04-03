cmake_minimum_required(VERSION 3.20)

# CMake 4.x: allow FetchContent dependencies to use older cmake_minimum_required
# calls (e.g. Raylib may declare cmake_minimum_required(VERSION 3.0)).
set(CMAKE_POLICY_VERSION_MINIMUM 3.10)

project(PhysicsSimulation
    VERSION 0.1.0
    LANGUAGES C CXX
)

include(CTest)

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

include(cmake/project-options.cmake)

# ---------------------------------------------------------------------------
# Dependencies (Raylib, ImGui, rlImGui, GLM)
# Store FetchContent downloads outside the build tree so that deleting
# build/ does not force a re-download.  The external/ directory is
# shared across all presets/configurations.
# ---------------------------------------------------------------------------
set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/external" CACHE PATH "" FORCE)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE BOOL "" FORCE)
include(cmake/deps.cmake)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
add_subdirectory(project1_balistica)
add_subdirectory(project2_pendulum)
