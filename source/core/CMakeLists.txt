add_library(core
    path.c
    timer.c
    frame_limiter.c
    debug.cpp
    debug_defaults.c
)

add_subdirectory(imgui_backends)

target_include_directories(core PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(core PUBLIC compiler_options imgui_backend SDL3::SDL3)
