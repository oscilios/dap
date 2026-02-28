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

#if(APPLE)
#  set(MACOSX_RPATH ON)
#  set(CMAKE_MACOSX_RPATH ON)
#  set(CMAKE_SKIP_BUILD_RPATH FALSE)
#  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) # set to true if rpaths should be
#                                            # set during build process
#  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE) # set to true if rpaths should be
#                                              # set at install/bundle step
#  set(MY_RPATH_LIST
#      "@executable_path/../Frameworks" ${CMAKE_BINARY_DIR}/thirdparty/install
#      # "/usr/local/lib" "/usr/lib" "@loader_path/../lib"
#  )
#  set(CMAKE_INSTALL_RPATH ${MY_RPATH_LIST}) # set a list of paths for rpath
#endif()

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
include(thirdparty.cmake)

include_directories(src)
add_subdirectory(src)
