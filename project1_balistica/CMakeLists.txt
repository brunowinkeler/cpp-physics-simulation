set(EXECUTABLE_NAME "Project1_Balistica")

set(PROJECT1_BALISTICA_SOURCES
    src/main.cpp
    src/app/Application.cpp
    src/physics/Projectile.cpp
    src/rendering/BackgroundRenderer.cpp
    src/rendering/Camera.cpp
    src/rendering/ProjectileCoordinatesAdapter.cpp
    src/rendering/ProjectileRenderer.cpp
    src/rendering/SimulationOverlayRenderer.cpp
    src/rendering/SimulationSceneRenderer.cpp
    src/rendering/TrajectoryRenderer.cpp
    src/simulation/Simulation.cpp
    src/ui/UiMenus.cpp
    ../core/physics/IntegrationMethod.cpp
    ../core/simulation/TrajectoryRecorder.cpp
)

add_executable(${EXECUTABLE_NAME}
    ${PROJECT1_BALISTICA_SOURCES}
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
