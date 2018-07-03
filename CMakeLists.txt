cmake_minimum_required (VERSION 3.7.1)
cmake_policy (SET CMP0048 NEW)
project (dap VERSION 0.1.0 LANGUAGES "CXX")
include (CTest)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules")

if (POLICY CMP0043)
    # do not ignore COMPILE_DEFINITIONS_<Config> properties
    cmake_policy(SET CMP0043 NEW)
endif ()

option (Enable_ASAN     "Enable Address Sanitizer" OFF)
option (Enable_TSAN     "Enable Thread Sanitizer" OFF)
option (EnableClangTidy "Run the clang tidy tool" OFF)
option (EnableIWYU      "Enable include-what-you-use in targets" OFF)


if (${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    message (FATAL_ERROR "Using source directory as build directory is not permitted")
endif ()

#########################################################
# system flags
#########################################################
if (APPLE)
    set (target_arch x86_64)
    #set (target_arch i386)  # uncomment for 32-bit architecture
    set (CMAKE_OSX_ARCHITECTURES ${target_arch})
    set (CMAKE_C_FLAGS   ${CMAKE_C_FLAGS}   "-arch ${target_arch}")
    set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-arch ${target_arch}")
    set (CMAKE_LD_FLAGS  ${CMAKE_LD_FLAGS}  "-arch ${target_arch}")
    set (CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++1z")
    set (CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
endif ()

#########################################################
# c++ 1x flags
#########################################################
if (${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    #add_compile_options (-fcolor-diagnostics) # force colored output
    add_compile_options (-std=c++1z)
    add_compile_options (-stdlib=libc++)
    add_compile_options (-Wall -Wextra -pedantic -Wnon-virtual-dtor -Weffc++)
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
    add_compile_options (-std=c++17)
    add_compile_options (-Wall -Wextra -pedantic -Wno-long-long -Wno-deprecated-declarations)
endif ()

#########################################################
# optimization flags
#########################################################

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message ("Build Type: " ${CMAKE_BUILD_TYPE})
    add_compile_options (-O3 -msse3 -msse4.1 -mfpmath=sse)
else ()
    message ("Build Type: " ${CMAKE_BUILD_TYPE})
    #add_compile_options (-fsanitize=address)
    add_definitions (-DDAP_DEBUG)
endif ()

if (APPLE)
    set (MACOSX_RPATH ON)
    set(CMAKE_MACOSX_RPATH ON)
    set (CMAKE_SKIP_BUILD_RPATH FALSE)
    set (CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)   # set to true if rpaths should be set during build process
    set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE) # set to true if rpaths should be set at install/bundle step
    set (MY_RPATH_LIST
        "@executable_path/../Frameworks"
        ${CMAKE_BINARY_DIR}/thirdparty/install
        #"/usr/local/lib"
        #"/usr/lib"
        #"@loader_path/../lib"
        )
    set (CMAKE_INSTALL_RPATH ${MY_RPATH_LIST}) # set a list of paths for rpath
endif ()

if (Enable_ASAN)
    add_compile_options(-fsanitize=address -fsanitize-recover=address -fno-omit-frame-pointer)
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fsanitize-recover=address -fno-omit-frame-pointer")
endif ()
if (ENABLE_TSAN)
    add_compile_options(-fsanitize=thread)
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")
endif ()

set (CMAKE_SHARED_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})

if (XCODE)
    set (CMAKE_XCODE_ATTRIBUTE_GCC_WARN_ABOUT_RETURN_TYPE               "YES")
    set (CMAKE_XCODE_ATTRIBUTE_GCC_WARN_ABOUT_MISSING_NEWLINE           "YES")
    set (CMAKE_XCODE_ATTRIBUTE_GCC_WARN_UNUSED_VALUE                    "YES")
    set (CMAKE_XCODE_ATTRIBUTE_GCC_WARN_UNUSED_VARIABLE                 "YES")
    set (CMAKE_XCODE_ATTRIBUTE_GCC_WARN_SIGN_COMPARE                    "YES")
    set (CMAKE_XCODE_ATTRIBUTE_GCC_WARN_UNUSED_FUNCTION                 "YES")
    set (CMAKE_XCODE_ATTRIBUTE_GCC_WARN_INITIALIZER_NOT_FULLY_BRACKETED "YES")
    set (CMAKE_XCODE_ATTRIBUTE_GCC_WARN_HIDDEN_VIRTUAL_FUNCTIONS        "YES")
    set (CMAKE_XCODE_ATTRIBUTE_GCC_WARN_UNINITIALIZED_AUTOS             "YES")
    set (CMAKE_XCODE_ATTRIBUTE_GCC_WARN_NON_VIRTUAL_DESTRUCTOR          "YES")
    set (CMAKE_XCODE_ATTRIBUTE_GCC_WARN_SHADOW                          "YES")
    set (CMAKE_XCODE_ATTRIBUTE_GCC_WARN_PEDANTIC                        "YES")
    set (GCC_WARN_EFFECTIVE_CPLUSPLUS_VIOLATIONS                        "YES")
    set (CMAKE_XCODE_ATTRIBUTE_CLANG_WARN_BOOL_CONVERSION               "YES")
    set (CMAKE_XCODE_ATTRIBUTE_CLANG_WARN_EMPTY_BODY                    "YES")
    set (CMAKE_XCODE_ATTRIBUTE_CLANG_WARN_ENUM_CONVERSION               "YES")
    set (CMAKE_XCODE_ATTRIBUTE_CLANG_WARN_INT_CONVERSION                "YES")
    set (CMAKE_XCODE_ATTRIBUTE_CLANG_WARN_CONSTANT_CONVERSION           "YES")
endif ()


#########################################################
# src and thirdparty dependencies
#########################################################

set (THIRDPARTY_SRC_DIR ${CMAKE_SOURCE_DIR}/thirdparty)
set (THIRDPARTY_DEST_DIR ${CMAKE_BINARY_DIR}/thirdparty)
set (THIRDPARTY_INSTALL_DIR ${THIRDPARTY_DEST_DIR}/install)
#if (NOT IS_DIRECTORY ${THIRDPARTY_INSTALL_DIR})
    execute_process (COMMAND ${CMAKE_SOURCE_DIR}/build_dependencies.py
                             ${THIRDPARTY_SRC_DIR} ${THIRDPARTY_DEST_DIR} ${CMAKE_BUILD_TYPE}
                     RESULT_VARIABLE THIRDPARTY_BUILD_RESULT
                    )
    if (${THIRDPARTY_BUILD_RESULT})
        message (FATAL_ERROR "External library dependencies were not found")
    endif ()
#endif ()

include_directories (${THIRDPARTY_INSTALL_DIR}/include)
list (APPEND CMAKE_PREFIX_PATH ${THIRDPARTY_INSTALL_DIR})

foreach (lib
        sndfile
        oscpack
        benchmark
        )
    find_library (${lib}_LIBRARY ${lib} REQUIRED)
    add_library  (${lib} SHARED IMPORTED)
    message (STATUS "${lib}_LIBRARY ${${lib}_LIBRARY}")
    set_target_properties (${lib} PROPERTIES IMPORTED_LOCATION ${${lib}_LIBRARY})
endforeach ()

#########################################################
# clang tools
#########################################################
if (EnableIWYU)
    find_program (iwyu_exe NAMES include-what-you-use iwyu)
    if (NOT iwyu_exe)
        message (FATAL_ERROR "Could not find the program include-what-you-use")
    else ()
        if (APPLE)
            set (CMAKE_OSX_SYSROOT /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk)
            set (XCODE_SDK ${CMAKE_OSX_SYSROOT}/usr/include/c++/4.2.1)
            set (IWYU_C_COMPILE_OPTIONS
                "-Xiwyu"
                "--verbose=3"
                "-Xiwyu"
                "--no_comments"
                "-Xiwyu"
                "--mapping_file=${CMAKE_SOURCE_DIR}/iwyu_mappings/iwyu.imp"
                )
            set (xcode_toolchain /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain)
            set (IWYU_CXX_COMPILE_OPTIONS ${IWYU_C_COMPILE_OPTIONS} "-I${xcode_toolchain}/usr/include/c++/v1" "-std=c++14" "-stdlib=libc++")
        endif ()
        set (CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${iwyu_exe} ${IWYU_CXX_COMPILE_OPTIONS})
        set (CMAKE_C_INCLUDE_WHAT_YOU_USE   ${iwyu_exe} ${IWYU_C_COMPILE_OPTIONS})
    endif()
endif ()

#########################################################
# project directories
#########################################################
include_directories (src) # force to include prepending directory that contains a header
add_subdirectory (src)

if (EnableClangTidy)
    find_program (clang_tidy_exe NAMES clang-tidy)
    if (NOT clang_tidy_exe)
        message (FATAL_ERROR "Could not find the program clang-tidy")
    else ()
        string (REPLACE ";" " " clang_cxx_flags ${CMAKE_CXX_FLAGS})
        set (eigen_defs -DEIGEN_NO_MALLOC)
        set (clang_tidy_opts "${clang_cxx_flags} ${eigen_defs} -DNDEBUG -I${THIRDPARTY_INSTALL_DIR}/include -I${CMAKE_SOURCE_DIR}/src")
        if (APPLE)
            set (xcode_toolchain /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain)
            set (clang_tidy_opts "${clang_tidy_opts} -I${xcode_toolchain}/usr/include/c++/v1")
        endif ()
        set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
        set (clang_tidy_line_filter_file ${CMAKE_SOURCE_DIR}/clang_tidy_line_filters.json)
        set (clang_tidy_checks "*,clang-diagnostic-*,clang-analyzer-*,dolby-*,-clang-analyzer-alpha*,-google-build-using-namespace,-llvm-header-guard,-llvm-include-order,-google-readability-namespace-comments,-google-runtime-references*,-llvm-namespace-comment,-google-explicit-constructor,-google-readability-braces-around-statements,-hicpp-*,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-readability-braces-around-statements,-cppcoreguidelines-pro-bounds-constant-array-index,-cppcoreguidelines-pro-type-member-init,-readability-avoid-const-params-in-decls,-cppcoreguidelines-no-malloc,-cppcoreguidelines-pro-bounds-array-to-pointer-decay,-fuchsia-overloaded-operator,-fuchsia-default-arguments")
        #set (clang_tidy_checks "-checks=-*,clang-analyzer-*,-clang-analyzer-alpha*,readability-*,performance-*,modernize-*,cert-*,boost-*,misc-*,-misc-unconventional-assign-operator,cppcoreguidelines-*,dolby-*")
        add_custom_target(clang_tidy ALL
                          COMMAND echo ${CMAKE_SOURCE_DIR}/run_clang_tidy.py -checks=${clang_tidy_checks} -header-filter=^/${CMAKE_SOURCE_DIR}/src/.* -clang_options=${clang_tidy_opts}
                          COMMAND ${CMAKE_SOURCE_DIR}/run_clang_tidy.py -checks=${clang_tidy_checks} -header-filter=^${CMAKE_SOURCE_DIR}/src/.* -line_filter_file=${clang_tidy_line_filter_file} -clang_options=${clang_tidy_opts}
                          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                          COMMENT "running clang-tidy"
                          )
    endif ()
endif ()

