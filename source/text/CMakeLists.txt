add_library(text
    font.c
    text.c
)

target_include_directories(text PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(text PUBLIC
    compiler_options
    shader
    core
    Freetype::Freetype
    harfbuzz::harfbuzz
)
