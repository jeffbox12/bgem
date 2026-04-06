add_library(platform
    platform_window_linux.c
)

target_include_directories(platform PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(platform PUBLIC compiler_options SDL3::SDL3 OpenGL::GL OpenGL::EGL PkgConfig::WAYLAND_EGL)
