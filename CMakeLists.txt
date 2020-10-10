cmake_minimum_required (VERSION 3.7.1)
cmake_policy (SET CMP0048 NEW)
project (dap VERSION 0.1.0 LANGUAGES "CXX")
include (CTest)

if (POLICY CMP0043)
    # do not ignore COMPILE_DEFINITIONS_<Config> properties
    cmake_policy(SET CMP0043 NEW)
endif ()

option (Enable_ASAN     "Enable Address Sanitizer" OFF)
option (Enable_TSAN     "Enable Thread Sanitizer" OFF)
option (EnableClangTidy "Run clang tidy" OFF)
option (FixClangTidy    "Run clang tidy fixup" OFF)


if (${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    message (FATAL_ERROR "Using source directory as build directory is not permitted")
endif ()


set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS ON)
if (${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    #add_compile_options (-fcolor-diagnostics) # force colored output
    add_compile_options (-stdlib=libc++)
    add_compile_options (-Wall -Wextra -pedantic -Wnon-virtual-dtor -Weffc++)
    add_compile_options ("--system-header-prefix=Eigen")
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
    add_compile_options (-Wall -Wextra -pedantic -Wno-long-long -Wno-deprecated-declarations)
endif ()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message ("Build Type: " ${CMAKE_BUILD_TYPE})
    add_compile_options (-O3 -msse3 -msse4.1 -mfpmath=sse)
else ()
    message ("Build Type: " ${CMAKE_BUILD_TYPE})
    add_definitions (-DDAP_DEBUG)
endif ()

if (APPLE)
    set (CMAKE_OSX_ARCHITECTURES x86_64)
    set (MACOSX_RPATH ON)
    set (CMAKE_MACOSX_RPATH ON)
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
    set (CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
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
# thirdparty dependencies
#########################################################

set (THIRDPARTY_SRC_DIR ${CMAKE_SOURCE_DIR}/thirdparty)
set (THIRDPARTY_DEST_DIR ${CMAKE_BINARY_DIR}/thirdparty)
set (THIRDPARTY_INSTALL_DIR ${THIRDPARTY_DEST_DIR}/install)
execute_process (
    COMMAND
    ${CMAKE_SOURCE_DIR}/build_dependencies.py ${THIRDPARTY_SRC_DIR} ${THIRDPARTY_DEST_DIR} ${CMAKE_BUILD_TYPE}
    RESULT_VARIABLE THIRDPARTY_BUILD_RESULT
    )
if (${THIRDPARTY_BUILD_RESULT})
    message (
        FATAL_ERROR
        "External library dependencies were not found"
        )
endif ()

include_directories (${THIRDPARTY_INSTALL_DIR}/include)
include_directories (${THIRDPARTY_INSTALL_DIR}/include/eigen3)
list (APPEND CMAKE_PREFIX_PATH ${THIRDPARTY_INSTALL_DIR})

find_package(benchmark REQUIRED)
find_package(GTest REQUIRED COMPONENTS gtest)
find_package(SndFile REQUIRED)

#oscpack does not install cmake config files, so let's find it
find_library (oscpack_LIBRARY oscpack REQUIRED)
add_library  (oscpack::oscpack STATIC IMPORTED)
set_target_properties (oscpack::oscpack PROPERTIES IMPORTED_LOCATION ${oscpack_LIBRARY})

if (EnableClangTidy OR FixClangTidy)
    if (FixClangTidy)
        set (fix -fix)
    endif ()
    if (APPLE)
        set (xcode_toolchain /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain)
        set (clang_tidy_includes "-I${xcode_toolchain}/usr/include/c++/v1 -I${THIRDPARTY_INSTALL_DIR}/include")
    else ()
        set (clang_tidy_includes "-I${THIRDPARTY_INSTALL_DIR}/include")
    endif ()
        set (CMAKE_CXX_CLANG_TIDY clang-tidy ${fix} -quiet -extra-arg=${clang_tidy_includes} -header-filter=^${CMAKE_SOURCE_DIR}/src/.*)
endif ()

#########################################################
# project directories
#########################################################
include_directories (src)
add_subdirectory (src)
