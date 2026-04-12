add_library(system
    system.c
)

target_include_directories(system PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(system PUBLIC compiler_options SDL3::SDL3)
