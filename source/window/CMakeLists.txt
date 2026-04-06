add_library(window
    init_window.c
)

target_include_directories(window PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(window PUBLIC compiler_options SDL3::SDL3)

if(LINUX)
    target_link_libraries(window PUBLIC OpenGL::GL OpenGL::EGL)
else()
    target_link_libraries(window PUBLIC unofficial::angle::libEGL unofficial::angle::libGLESv2)
endif()
