add_library(layers
    background.c
    ui.c
    application.c
    overlay.c
    notifications.c
)

target_include_directories(layers PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(layers PUBLIC compiler_options SDL3::SDL3)
