add_library(app
    app.c
    app_loop.c
)

target_include_directories(app PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(app PUBLIC compiler_options SDL3::SDL3 input)
