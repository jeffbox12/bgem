# cmake/compiler_optionss.cmake

# Create the "Interface" target
add_library(compiler_options INTERFACE)

# Add Standard Requirements
set_target_properties(compiler_options PROPERTIES
    INTERFACE_C_STANDARD 23
    INTERFACE_C_STANDARD_REQUIRED ON
    INTERFACE_C_EXTENSIONS OFF
    INTERFACE_COMPILE_WARNING_AS_ERROR ON
)

# Add Compiler Flags
target_compile_options(compiler_options INTERFACE
    $<$<C_COMPILER_ID:MSVC>:
        /W4
        /Zc:preprocessor
        $<$<CONFIG:Debug>:/Zi>
    >
    $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>>:
        -Wall
        -Wextra
        -Wpedantic
        $<$<CONFIG:Debug>:-g>
    >
)

# Add Global Definitions
target_compile_definitions(compiler_options INTERFACE
    $<$<CONFIG:Debug>:DEBUG>
)

# Optional AddressSanitizer + UndefinedBehaviorSanitizer
option(ENABLE_ASAN_UBSAN "Enable ASan + UBsan" OFF)

if (ENABLE_ASAN_UBSAN)
    target_compile_options(compiler_options INTERFACE
        $<$<C_COMPILER_ID:MSVC>:
            /fsanitize=address
        >
        $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>>:
            -fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=undefined
        >
    )
    target_link_options(compiler_options INTERFACE
        $<$<C_COMPILER_ID:MSVC>:
            /fsanitize=address
            /incremental:no
        >
        $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>>:
            -fsanitize=address,undefined
        >
    )
endif()
