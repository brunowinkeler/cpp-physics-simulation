set(EXECUTABLE_NAME "Project1_Balistica")

file(GLOB BALISTICA_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${EXECUTABLE_NAME}
    ${BALISTICA_SOURCE_FILES}
)

# rlImGui needs to be compiled alongside ImGui headers
target_sources(${EXECUTABLE_NAME} PRIVATE
    ${rlimgui_SOURCE_DIR}/rlImGui.cpp
)

target_include_directories(${EXECUTABLE_NAME} PRIVATE
    ${rlimgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}
    ${glm_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    raylib
    imgui
    glm::glm
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
