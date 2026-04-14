set(EXECUTABLE_NAME "Project3_Gas_Brownian")

set(PROJECT3_GAS_BROWNIAN_SOURCES
    src/main.cpp
    src/app/GasBrownianApplication.cpp
    src/physics/GasCollision.cpp
    src/physics/GasPreset.cpp
    src/rendering/GasCamera.cpp
    src/rendering/GasOverlayRenderer.cpp
    src/rendering/GasSceneRenderer.cpp
    src/simulation/GasSimulation.cpp
    src/ui/GasUiMenus.cpp
    ../core/simulation/TrajectoryRecorder.cpp
)

add_executable(${EXECUTABLE_NAME}
    ${PROJECT3_GAS_BROWNIAN_SOURCES}
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
