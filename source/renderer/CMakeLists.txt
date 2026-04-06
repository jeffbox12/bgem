add_library(renderer
    renderer.c
    shader.c
)

target_include_directories(renderer PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(renderer PUBLIC compiler_options SDL3::SDL3)

if(LINUX)
    target_link_libraries(renderer PUBLIC OpenGL::GL OpenGL::EGL)
else()
    target_link_libraries(renderer PUBLIC unofficial::angle::libEGL unofficial::angle::libGLESv2)
endif()
