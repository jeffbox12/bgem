add_executable(bgem
    main.c
)

target_include_directories(bgem PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(window)
add_subdirectory(renderer)
add_subdirectory(platform)
add_subdirectory(app)
add_subdirectory(system)

add_custom_command(TARGET bgem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:bgem>/assets
)

if(LINUX)
    set_target_properties(bgem PROPERTIES
        BUILD_RPATH "$ORIGIN"
    )
endif()

target_link_libraries(bgem PRIVATE
    compiler_options
    system
    app
    renderer
    platform
    window
    # SDL3::SDL3
    # Freetype::Freetype
    # harfbuzz::harfbuzz
)
