include_guard(GLOBAL)

option(PHYSIM_ENABLE_WARNINGS "Enable compiler warnings for project targets" ON)
option(PHYSIM_ENABLE_SANITIZERS "Enable sanitizers for supported debug builds" OFF)

function(physim_apply_target_defaults target)
    if(PHYSIM_ENABLE_WARNINGS)
        if(MSVC)
            target_compile_options(${target} PRIVATE /W4 /permissive-)
        else()
            target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
        endif()
    endif()

    if(PHYSIM_ENABLE_SANITIZERS)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND NOT (WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows"))
            target_compile_options(${target} PRIVATE
                $<$<CONFIG:Debug>:-fsanitize=address,undefined>
                $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
            )
            target_link_options(${target} PRIVATE
                $<$<CONFIG:Debug>:-fsanitize=address,undefined>
            )
        endif()
    endif()
endfunction()

function(physim_link_platform_libraries target)
    if(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
        target_link_libraries(${target} PRIVATE
            winmm
            gdi32
            opengl32
        )
    endif()
endfunction()
