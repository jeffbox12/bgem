add_library(platform
    platform_window_macos.m
)

find_library(METAL Metal)

target_include_directories(platform PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(platform PUBLIC compiler_options SDL3::SDL3 unofficial::angle::libEGL unofficial::angle::libGLESv2 "-framework Foundation" "-framework Cocoa" "-framework QuartzCore" "-framework CoreGraphics" "-framework Metal" ${METAL})
