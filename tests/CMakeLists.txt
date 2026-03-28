# ---------------------------------------------------------------------------
# Executable
# ---------------------------------------------------------------------------
set(EXECUTABLE_NAME "SmokeTests")

add_executable(${EXECUTABLE_NAME}
    smoke.cpp
)

add_executable(ProjectilePhysicsTests
    projectile_physics.cpp
    ${CMAKE_SOURCE_DIR}/project1_balistica/src/physics/Projectile.cpp
)

# rlImGui needs to be compiled alongside ImGui headers
target_sources(${EXECUTABLE_NAME} PRIVATE
    ${rlimgui_SOURCE_DIR}/rlImGui.cpp
)

target_include_directories(${EXECUTABLE_NAME} PRIVATE
    ${rlimgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    raylib
    imgui
    glm::glm
)

target_include_directories(ProjectilePhysicsTests PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/project1_balistica/src
)

# ---------------------------------------------------------------------------
# Windows-specific settings
# ---------------------------------------------------------------------------
if(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Link Windows system libs needed by Raylib
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE
        winmm
        gdi32
        opengl32
    )
endif()
