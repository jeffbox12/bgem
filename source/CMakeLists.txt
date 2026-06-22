add_executable(bgem
    main.c
)

target_include_directories(bgem PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(window)
add_subdirectory(renderer)
add_subdirectory(platform)
add_subdirectory(app)
add_subdirectory(system)
add_subdirectory(core)
add_subdirectory(shader)
add_subdirectory(input)
add_subdirectory(text)
add_subdirectory(layers)
add_subdirectory(compositor)

if (WIN32 OR LINUX)
add_custom_command(TARGET bgem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:bgem>/assets
)
endif()

if(LINUX)
    set_target_properties(bgem PROPERTIES
        BUILD_RPATH "$ORIGIN"
    )
endif()

target_link_libraries(bgem PRIVATE
    compiler_options
    system
    app
    compositor
    layers
    input
    renderer
    shader
    core
    platform
    window
    # SDL3::SDL3
    # Freetype::Freetype
    # harfbuzz::harfbuzz
)

if(APPLE)
    set_target_properties(bgem PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "Bgem"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.bgem.bgem"
        MACOSX_BUNDLE_BUNDLE_VERSION "0.1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1.0"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/cmake/macos/Info.plist.in"
    )
endif()

if(APPLE)
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(
            \"${CMAKE_INSTALL_PREFIX}/bgem.app\"
            \"\"
            \"${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib\"
        )
    " COMPONENT Runtime)
endif()

# SDL_GetBasePath looks in "/Content/Resources/"
if(APPLE)
    file(GLOB_RECURSE BGEM_ASSET_FILES
        "${PROJECT_SOURCE_DIR}/assets/*"
    )

    foreach(ASSET ${BGEM_ASSET_FILES})
        file(RELATIVE_PATH ASSET_REL "${PROJECT_SOURCE_DIR}" "${ASSET}")
        get_filename_component(ASSET_DIR "${ASSET_REL}" DIRECTORY)
        set_source_files_properties("${ASSET}" PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources/${ASSET_DIR}"
        )
    endforeach()

    target_sources(bgem PRIVATE ${BGEM_ASSET_FILES})
endif()

target_sources(bgem PRIVATE
    core/imgui_backends/imgui_impl_sdl3.cpp
    core/imgui_backends/imgui_impl_opengl3.cpp
)

target_compile_definitions(bgem PRIVATE
    IMGUI_IMPL_OPENGL_ES3
)
