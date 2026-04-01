# ---------------------------------------------------------------------------
# Executable
# ---------------------------------------------------------------------------
set(EXECUTABLE_NAME "SmokeTests")

add_library(PhysimTestSupport INTERFACE)
target_include_directories(PhysimTestSupport INTERFACE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/project1_balistica/src
)
target_link_libraries(PhysimTestSupport INTERFACE
    GTest::gmock_main
)

add_executable(${EXECUTABLE_NAME}
    smoke.cpp
)

add_executable(ProjectilePhysicsTests
    projectile_physics.cpp
    simulation_tests.cpp
    trajectory_recorder_tests.cpp
    ${CMAKE_SOURCE_DIR}/project1_balistica/src/physics/Projectile.cpp
    ${CMAKE_SOURCE_DIR}/project1_balistica/src/simulation/Simulation.cpp
    ${CMAKE_SOURCE_DIR}/project1_balistica/src/simulation/TrajectoryRecorder.cpp
)
target_link_libraries(ProjectilePhysicsTests PRIVATE
    PhysimTestSupport
)

# rlImGui needs to be compiled alongside ImGui headers
target_sources(${EXECUTABLE_NAME} PRIVATE
    ${rlimgui_SOURCE_DIR}/rlImGui.cpp
)

if(MSVC)
    set_source_files_properties(${rlimgui_SOURCE_DIR}/rlImGui.cpp PROPERTIES COMPILE_OPTIONS "/w")
else()
    set_source_files_properties(${rlimgui_SOURCE_DIR}/rlImGui.cpp PROPERTIES COMPILE_OPTIONS "-w")
endif()

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

physim_apply_target_defaults(${EXECUTABLE_NAME})
physim_apply_target_defaults(ProjectilePhysicsTests)

if(BUILD_TESTING)
    add_test(NAME projectile_physics_domain COMMAND ProjectilePhysicsTests)
    set_tests_properties(projectile_physics_domain PROPERTIES
        LABELS "domain;physics"
        TIMEOUT 15
    )

    add_test(NAME smoke_graphics COMMAND SmokeTests --ctest)
    set_tests_properties(smoke_graphics PROPERTIES
        ENVIRONMENT "ASAN_OPTIONS=detect_leaks=0"
        LABELS "graphics;smoke"
        RUN_SERIAL TRUE
        TIMEOUT 15
    )
endif()

physim_link_platform_libraries(${EXECUTABLE_NAME})
