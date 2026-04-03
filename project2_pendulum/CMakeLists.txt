set(EXECUTABLE_NAME "Project2_Pendulum")

set(PROJECT2_PENDULUM_SOURCES
    src/main.cpp
    src/app/PendulumApplication.cpp
    src/physics/SimplePendulum.cpp
    src/physics/DoublePendulum.cpp
    src/rendering/PendulumBackgroundRenderer.cpp
    src/rendering/PendulumCamera.cpp
    src/rendering/PendulumOverlayRenderer.cpp
    src/rendering/PendulumRenderer.cpp
    src/rendering/PendulumSceneRenderer.cpp
    src/rendering/PendulumTrajectoryRenderer.cpp
    src/simulation/PendulumSimulation.cpp
    src/ui/PendulumUiMenus.cpp
    ../core/physics/IntegrationMethod.cpp
    ../core/simulation/TrajectoryRecorder.cpp
)

add_executable(${EXECUTABLE_NAME}
    ${PROJECT2_PENDULUM_SOURCES}
)

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
    ${glm_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    raylib
    imgui
    glm::glm
)

physim_apply_target_defaults(${EXECUTABLE_NAME})
physim_link_platform_libraries(${EXECUTABLE_NAME})
