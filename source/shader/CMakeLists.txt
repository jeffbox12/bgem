add_library(shader
    shader.c
    shader_catalog.c
)

target_include_directories(shader PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(shader PUBLIC compiler_options SDL3::SDL3)

if(LINUX)
    target_link_libraries(shader PUBLIC OpenGL::GL OpenGL::EGL)
else()
    target_link_libraries(shader PUBLIC unofficial::angle::libEGL unofficial::angle::libGLESv2)
endif()
