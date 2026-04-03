# ---------------------------------------------------------------------------
# Executable
# ---------------------------------------------------------------------------
set(EXECUTABLE_NAME "SmokeTests")

add_library(PhysimTestSupport INTERFACE)
target_include_directories(PhysimTestSupport INTERFACE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/project1_balistica/src
    ${CMAKE_SOURCE_DIR}/tests
)
target_link_libraries(PhysimTestSupport INTERFACE
    GTest::gmock_main
)

add_executable(${EXECUTABLE_NAME}
    common/smoke.cpp
)

add_executable(CommonPhysicsTests
    common/integration_method_tests.cpp
    common/trajectory_recorder_tests.cpp
    ${CMAKE_SOURCE_DIR}/core/physics/IntegrationMethod.cpp
    ${CMAKE_SOURCE_DIR}/core/simulation/TrajectoryRecorder.cpp
)
target_link_libraries(CommonPhysicsTests PRIVATE
    PhysimTestSupport
)

add_executable(Project1BalisticaTests
    project1_balistica/projectile_physics_tests.cpp
    project1_balistica/simulation_tests.cpp
    ${CMAKE_SOURCE_DIR}/project1_balistica/src/physics/Projectile.cpp
    ${CMAKE_SOURCE_DIR}/project1_balistica/src/simulation/Simulation.cpp
    ${CMAKE_SOURCE_DIR}/core/physics/IntegrationMethod.cpp
    ${CMAKE_SOURCE_DIR}/core/simulation/TrajectoryRecorder.cpp
)
target_link_libraries(Project1BalisticaTests PRIVATE
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
physim_apply_target_defaults(CommonPhysicsTests)
physim_apply_target_defaults(Project1BalisticaTests)

if(BUILD_TESTING)
    add_test(NAME common_physics_domain COMMAND CommonPhysicsTests)
    set_tests_properties(common_physics_domain PROPERTIES
        LABELS "common;physics"
        TIMEOUT 15
    )

    add_test(NAME project1_balistica_domain COMMAND Project1BalisticaTests)
    set_tests_properties(project1_balistica_domain PROPERTIES
        LABELS "project1;physics"
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
