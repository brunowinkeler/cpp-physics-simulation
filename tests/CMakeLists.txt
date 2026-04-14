# ---------------------------------------------------------------------------
# Executable
# ---------------------------------------------------------------------------
set(EXECUTABLE_NAME "SmokeTests")

add_library(PhysimTestSupport INTERFACE)
target_include_directories(PhysimTestSupport INTERFACE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/project1_balistica/src
    ${CMAKE_SOURCE_DIR}/project2_pendulum/src
    ${CMAKE_SOURCE_DIR}/project3_gas_brownian/src
    ${CMAKE_SOURCE_DIR}/tests
)
target_link_libraries(PhysimTestSupport INTERFACE
    GTest::gmock_main
)

add_executable(${EXECUTABLE_NAME}
    common/smoke.cpp
)

add_executable(Project2PendulumSmokeTests
    project2_pendulum/smoke.cpp
    ${CMAKE_SOURCE_DIR}/project2_pendulum/src/physics/SimplePendulum.cpp
    ${CMAKE_SOURCE_DIR}/project2_pendulum/src/physics/DoublePendulum.cpp
    ${CMAKE_SOURCE_DIR}/project2_pendulum/src/rendering/PendulumBackgroundRenderer.cpp
    ${CMAKE_SOURCE_DIR}/project2_pendulum/src/rendering/PendulumCamera.cpp
    ${CMAKE_SOURCE_DIR}/project2_pendulum/src/rendering/PendulumOverlayRenderer.cpp
    ${CMAKE_SOURCE_DIR}/project2_pendulum/src/rendering/PendulumRenderer.cpp
    ${CMAKE_SOURCE_DIR}/project2_pendulum/src/rendering/PendulumSceneRenderer.cpp
    ${CMAKE_SOURCE_DIR}/project2_pendulum/src/rendering/PendulumTrajectoryRenderer.cpp
    ${CMAKE_SOURCE_DIR}/project2_pendulum/src/simulation/PendulumSimulation.cpp
    ${CMAKE_SOURCE_DIR}/project2_pendulum/src/ui/PendulumUiMenus.cpp
    ${CMAKE_SOURCE_DIR}/core/physics/IntegrationMethod.cpp
    ${CMAKE_SOURCE_DIR}/core/simulation/TrajectoryRecorder.cpp
)

add_executable(Project3GasBrownianSmokeTests
    project3_gas_brownian/smoke.cpp
    ${CMAKE_SOURCE_DIR}/project3_gas_brownian/src/physics/GasCollision.cpp
    ${CMAKE_SOURCE_DIR}/project3_gas_brownian/src/physics/GasPreset.cpp
    ${CMAKE_SOURCE_DIR}/project3_gas_brownian/src/rendering/GasCamera.cpp
    ${CMAKE_SOURCE_DIR}/project3_gas_brownian/src/rendering/GasOverlayRenderer.cpp
    ${CMAKE_SOURCE_DIR}/project3_gas_brownian/src/rendering/GasSceneRenderer.cpp
    ${CMAKE_SOURCE_DIR}/project3_gas_brownian/src/simulation/GasSimulation.cpp
    ${CMAKE_SOURCE_DIR}/project3_gas_brownian/src/ui/GasUiMenus.cpp
    ${CMAKE_SOURCE_DIR}/core/simulation/TrajectoryRecorder.cpp
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

add_executable(Project2PendulumTests
    project2_pendulum/pendulum_physics_tests.cpp
    project2_pendulum/pendulum_simulation_tests.cpp
    ${CMAKE_SOURCE_DIR}/project2_pendulum/src/physics/SimplePendulum.cpp
    ${CMAKE_SOURCE_DIR}/project2_pendulum/src/physics/DoublePendulum.cpp
    ${CMAKE_SOURCE_DIR}/project2_pendulum/src/simulation/PendulumSimulation.cpp
    ${CMAKE_SOURCE_DIR}/core/physics/IntegrationMethod.cpp
    ${CMAKE_SOURCE_DIR}/core/simulation/TrajectoryRecorder.cpp
)
target_link_libraries(Project2PendulumTests PRIVATE
    PhysimTestSupport
)

add_executable(Project3GasBrownianTests
    project3_gas_brownian/gas_physics_tests.cpp
    project3_gas_brownian/gas_simulation_tests.cpp
    ${CMAKE_SOURCE_DIR}/project3_gas_brownian/src/physics/GasCollision.cpp
    ${CMAKE_SOURCE_DIR}/project3_gas_brownian/src/physics/GasPreset.cpp
    ${CMAKE_SOURCE_DIR}/project3_gas_brownian/src/simulation/GasSimulation.cpp
    ${CMAKE_SOURCE_DIR}/core/simulation/TrajectoryRecorder.cpp
)
target_link_libraries(Project3GasBrownianTests PRIVATE
    PhysimTestSupport
)

function(physim_configure_smoke_target target)
    target_sources(${target} PRIVATE
        ${rlimgui_SOURCE_DIR}/rlImGui.cpp
    )

    if(MSVC)
        set_source_files_properties(${rlimgui_SOURCE_DIR}/rlImGui.cpp PROPERTIES COMPILE_OPTIONS "/w")
    else()
        set_source_files_properties(${rlimgui_SOURCE_DIR}/rlImGui.cpp PROPERTIES COMPILE_OPTIONS "-w")
    endif()

    target_include_directories(${target} PRIVATE
        ${rlimgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}
    )

    target_link_libraries(${target} PRIVATE
        raylib
        imgui
        glm::glm
    )

    physim_apply_target_defaults(${target})
    physim_link_platform_libraries(${target})
endfunction()

target_include_directories(Project2PendulumSmokeTests PRIVATE
    ${CMAKE_SOURCE_DIR}/project2_pendulum/src
)

target_include_directories(Project3GasBrownianSmokeTests PRIVATE
    ${CMAKE_SOURCE_DIR}/project3_gas_brownian/src
)

physim_configure_smoke_target(${EXECUTABLE_NAME})
physim_configure_smoke_target(Project2PendulumSmokeTests)
physim_configure_smoke_target(Project3GasBrownianSmokeTests)

physim_apply_target_defaults(CommonPhysicsTests)
physim_apply_target_defaults(Project1BalisticaTests)
physim_apply_target_defaults(Project2PendulumTests)
physim_apply_target_defaults(Project3GasBrownianTests)

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

    add_test(NAME project2_pendulum_domain COMMAND Project2PendulumTests)
    set_tests_properties(project2_pendulum_domain PROPERTIES
        LABELS "project2;physics"
        TIMEOUT 15
    )

    add_test(NAME project3_gas_brownian_domain COMMAND Project3GasBrownianTests)
    set_tests_properties(project3_gas_brownian_domain PROPERTIES
        LABELS "project3;physics"
        TIMEOUT 15
    )

    add_test(NAME smoke_graphics COMMAND SmokeTests --ctest)
    set_tests_properties(smoke_graphics PROPERTIES
        ENVIRONMENT "ASAN_OPTIONS=detect_leaks=0"
        LABELS "graphics;smoke"
        RUN_SERIAL TRUE
        TIMEOUT 15
    )

    add_test(NAME project2_pendulum_smoke_graphics COMMAND Project2PendulumSmokeTests --ctest)
    set_tests_properties(project2_pendulum_smoke_graphics PROPERTIES
        ENVIRONMENT "ASAN_OPTIONS=detect_leaks=0"
        LABELS "graphics;smoke;project2"
        RUN_SERIAL TRUE
        TIMEOUT 15
    )

    add_test(NAME project3_gas_brownian_smoke_graphics COMMAND Project3GasBrownianSmokeTests --ctest)
    set_tests_properties(project3_gas_brownian_smoke_graphics PROPERTIES
        ENVIRONMENT "ASAN_OPTIONS=detect_leaks=0"
        LABELS "graphics;smoke;project3"
        RUN_SERIAL TRUE
        TIMEOUT 15
    )
endif()
