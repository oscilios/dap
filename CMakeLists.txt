cmake_minimum_required(VERSION 4.0)
project(
  dap
  VERSION 0.1.0
  LANGUAGES "CXX")
include(CTest)

if(${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
  message(
    FATAL_ERROR "Using source directory as build directory is not permitted")
endif()

if(CMAKE_BUILD_TYPE EQUAL Debug)
  add_definitions(-DDAP_DEBUG)
endif()

if(APPLE)
  set(MACOSX_RPATH ON)
  set(CMAKE_MACOSX_RPATH ON)
  set(CMAKE_SKIP_BUILD_RPATH FALSE)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) # set to true if rpaths should be
                                            # set during build process
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE) # set to true if rpaths should be
                                              # set at install/bundle step
  set(MY_RPATH_LIST
      "@executable_path/../Frameworks" ${CMAKE_BINARY_DIR}/thirdparty/install
      # "/usr/local/lib" "/usr/lib" "@loader_path/../lib"
  )
  set(CMAKE_INSTALL_RPATH ${MY_RPATH_LIST}) # set a list of paths for rpath
endif()

# ##############################################################################
# thirdparty dependencies
# ##############################################################################

set(THIRDPARTY_SRC_DIR ${CMAKE_SOURCE_DIR}/thirdparty)
set(THIRDPARTY_DEST_DIR ${CMAKE_BINARY_DIR}/thirdparty)
set(THIRDPARTY_INSTALL_DIR ${THIRDPARTY_DEST_DIR}/install)
execute_process(
  COMMAND ${CMAKE_SOURCE_DIR}/build_dependencies.py ${THIRDPARTY_SRC_DIR}
          ${THIRDPARTY_DEST_DIR} ${CMAKE_BUILD_TYPE}
  RESULT_VARIABLE THIRDPARTY_BUILD_RESULT)
if(${THIRDPARTY_BUILD_RESULT})
  message(FATAL_ERROR "External library dependencies were not found")
endif()

include_directories(${THIRDPARTY_INSTALL_DIR}/include)
include_directories(${THIRDPARTY_INSTALL_DIR}/include/eigen3)
list(APPEND CMAKE_PREFIX_PATH ${THIRDPARTY_INSTALL_DIR})

find_package(benchmark REQUIRED)
find_package(GTest REQUIRED)
find_package(SndFile REQUIRED)

# oscpack does not install cmake config files, so let's find it
find_library(oscpack_LIBRARY oscpack REQUIRED)
add_library(oscpack::oscpack STATIC IMPORTED)
set_target_properties(oscpack::oscpack PROPERTIES IMPORTED_LOCATION
                                                  ${oscpack_LIBRARY})
# ##############################################################################
# project directories
# ##############################################################################
include_directories(src)
add_subdirectory(src)
