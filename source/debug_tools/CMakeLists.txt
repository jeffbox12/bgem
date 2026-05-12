add_library(debug_tools
    debug.cpp
)

add_subdirectory(imgui_backends)

target_include_directories(debug_tools PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(debug_tools PUBLIC compiler_options imgui_backend)
