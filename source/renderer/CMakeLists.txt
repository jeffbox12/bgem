add_library(renderer
    renderer.c
)

target_include_directories(renderer PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(renderer PUBLIC compiler_options SDL3::SDL3)
