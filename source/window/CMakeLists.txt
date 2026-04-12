add_library(window
    window.c
)

target_include_directories(window PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(window PUBLIC compiler_options SDL3::SDL3)
