add_library(input
    input.c
    input_debug.c
)

target_include_directories(input PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(input PUBLIC compiler_options SDL3::SDL3 window)
