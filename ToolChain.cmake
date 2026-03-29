include_guard()

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_CLANG_TIDY "Enable static code analysis with clang-tidy" OFF)

if(NOT CMAKE_CXX_COMPILER_ID)
    set(CMAKE_CXX_COMPILER_ID AppleClang)
endif()

option(ENABLE_LSAN "Enable LeakSanitizer" OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
add_compile_options(
    -fno-omit-frame-pointer
    $<$<BOOL:${ENABLE_ASAN}>:-fsanitize=address>
    $<$<BOOL:${ENABLE_LSAN}>:-fsanitize=leak>
    $<$<BOOL:${ENABLE_TSAN}>:-fsanitize=thread>
    $<$<BOOL:${ENABLE_UBSAN}>:-fsanitize=undefined>
)
add_link_options(
    $<$<BOOL:${ENABLE_ASAN}>:-fsanitize=address>
    $<$<BOOL:${ENABLE_LSAN}>:-fsanitize=leak>
    $<$<BOOL:${ENABLE_TSAN}>:-fsanitize=thread>
    $<$<BOOL:${ENABLE_UBSAN}>:-fsanitize=undefined>
)


if(ENABLE_CLANG_TIDY)
    set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
    find_program(CLANG_TIDY_EXECUTABLE clang-tidy REQUIRED)
    execute_process(
        COMMAND /usr/bin/xcrun --show-sdk-path
        OUTPUT_VARIABLE XCODE_SDK
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(APPLE)
        set(clang_system_include_path "-isysroot${XCODE_SDK}")
        set(CMAKE_CXX_CLANG_TIDY
            ${CLANG_TIDY_EXECUTABLE}
            --extra-arg=${clang_system_include_path}
            --header-filter=^${CMAKE_CURRENT_LIST_DIR}/.*
            -p
            ${CMAKE_BINARY_DIR}
        )
    else()
        set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXECUTABLE} -p ${CMAKE_BINARY_DIR})
    endif()
    set(CMAKE_C_CLANG_TIDY ${CLANG_TIDY_EXECUTABLE} -p ${CMAKE_BINARY_DIR})
endif()


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

add_compile_options(
    -Wall
    -Wextra
    -pedantic
    -Wreturn-type
    -Wdeprecated-declarations
    -Wunused
    -Wswitch
    -Wmost
    -Wreorder
)
add_compile_options("--system-header-prefix=Eigen")
