cmake_minimum_required(VERSION 3.7.1)
project(dap_thirdparty)
include(ExternalProject)
set(THIRDPARTY_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/install)

ExternalProject_Add(
  gtest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_INSTALL_DIR} -DBUILD_GMOCK=OFF
             -DCMAKE_POLICY_VERSION_MINIMUM=3.5)

ExternalProject_Add(
  google_benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.9.4
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_INSTALL_DIR}
             -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DBENCHMARK_ENABLE_TESTING=OFF
             -DCMAKE_POLICY_VERSION_MINIMUM=3.5)

ExternalProject_Add(
  oscpack
  GIT_REPOSITORY https://github.com/RossBencina/oscpack.git
  GIT_TAG release_1_1_0
  PREFIX oscpack
  BUILD_IN_SOURCE 1
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_INSTALL_DIR}
             -DCMAKE_POLICY_VERSION_MINIMUM=3.5
  INSTALL_COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_BINARY_DIR}/oscpack/src/oscpack/ip
    ${THIRDPARTY_INSTALL_DIR}/include/oscpack/ip && ${CMAKE_COMMAND} -E
    copy_directory ${CMAKE_CURRENT_BINARY_DIR}/oscpack/src/oscpack/osc
    ${THIRDPARTY_INSTALL_DIR}/include/oscpack/osc && ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_BINARY_DIR}/oscpack/src/oscpack/liboscpack.a
    ${THIRDPARTY_INSTALL_DIR}/lib)

ExternalProject_Add(
  eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG 3.3.8
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_INSTALL_DIR}
             -DCMAKE_POLICY_VERSION_MINIMUM=3.5)

ExternalProject_Add(
  sndfile
  GIT_REPOSITORY https://github.com/libsndfile/libsndfile.git
  GIT_TAG 1.2.2
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_INSTALL_DIR}
             -DCMAKE_POLICY_VERSION_MINIMUM=3.5
             -DBUILD_EXAMPLES=OFF
             -DBUILD_PROGRAMS=OFF
             -DBUILD_REGTEST=OFF
             -DBUILD_TESTING=OFF
             -DENABLE_BOW_DOCS=OFF
             -DENABLE_EXPERIMENTAL_LIBS=OFF
             -DENABLE_EXPERIMENTAL=OFF
             -DENABLE_CPACK=OFF
             -DENABLE_MPEG=OFF
             -DENABKE_PKG_CONFIG=OFF
             -DINSTALL_PKGCONFIG_MODULE=OFF)
