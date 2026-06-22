add_library(compositor
    blend.c
    compositor.c
)

target_include_directories(compositor PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(compositor PUBLIC compiler_options layers renderer text)
