add_library(imgui_backend
    imgui_impl_opengl3_loader.h
    imgui_impl_opengl3.cpp
    imgui_impl_opengl3.h
    imgui_impl_sdl3.cpp
    imgui_impl_sdl3.h
)

find_package(imgui)

target_link_libraries(imgui_backend PUBLIC compiler_options imgui::imgui)
