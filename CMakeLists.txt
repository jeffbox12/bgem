cmake_minimum_required(VERSION 3.28)

project(bgem VERSION 0.1.0 LANGUAGES C CXX)

if(APPLE)
    enable_language(OBJC)
endif()

if (APPLE)
    if (NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
        message(FATAL_ERROR "Only ARM64 Macs are supported.")
    endif()
else()
    if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|aarch64|arm64")
        message(FATAL_ERROR "Only x64 or ARM64 architectures are supported.")
    endif()
endif()

find_package(freetype CONFIG REQUIRED)
find_package(harfbuzz CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
if (NOT LINUX)
    find_package(unofficial-angle CONFIG REQUIRED)
else ()
    find_package(OpenGL REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(WAYLAND_EGL REQUIRED IMPORTED_TARGET wayland-egl)
endif ()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(compiler_options)
include(version)

configure_file(
    include/bgem_version.h.in
    ${CMAKE_SOURCE_DIR}/include/bgem_version.h
    @ONLY
)

add_subdirectory(source)
