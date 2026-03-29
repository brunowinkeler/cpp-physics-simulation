include(FetchContent)

# ---------------------------------------------------------------------------
# Raylib 5.5
# Rendering, window, input, audio and 3D module
# ---------------------------------------------------------------------------
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
set(RAYLIB_BUILD_MODE Release CACHE STRING "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.5
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(raylib)

# ---------------------------------------------------------------------------
# Dear ImGui v1.92.6
# Real-time control panel (sliders, buttons, graphs)
# Does not provide a CMakeLists: we compile it manually as a library.
# Version 1.92+ is required for compatibility with rlImGui main.
# ---------------------------------------------------------------------------
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.92.6
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})

# ---------------------------------------------------------------------------
# rlImGui pinned to the revision currently validated by this project
# Bridge between Raylib and Dear ImGui — only rlImGui.h / rlImGui.cpp
# Source files are added directly to the executable in the root CMakeLists.txt
# so they can access includes from both raylib and imgui.
# ---------------------------------------------------------------------------
FetchContent_Declare(
    rlimgui
    GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
    GIT_TAG        286e11acd6c785004c9550c7ed3762add2ae3d47
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(rlimgui)

# ---------------------------------------------------------------------------
# GLM 1.0.1
# Vector/matrix math — header-only, no compilation required
# ---------------------------------------------------------------------------
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(glm)
