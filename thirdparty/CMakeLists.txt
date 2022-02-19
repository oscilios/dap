cmake_minimum_required(VERSION 3.7.1)
project (dap_thirdparty)
include (ExternalProject)
set (THIRDPARTY_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/install)

ExternalProject_Add(gtest
                    URL         https://github.com/google/googletest/archive/release-1.10.0.tar.gz
                    PREFIX      gtest
                    CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_INSTALL_DIR} -DBUILD_GMOCK=OFF
)
ExternalProject_Add(google_benchmark
                    URL         https://github.com/google/benchmark/archive/v1.5.2.tar.gz
                    PREFIX      benchmark
                    CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_INSTALL_DIR} -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DBENCHMARK_ENABLE_TESTING=OFF
)
ExternalProject_Add(oscpack
                    URL         https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/oscpack/oscpack_1_1_0.zip
                    PREFIX      oscpack
                    BUILD_IN_SOURCE 1
                    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/oscpack/src/oscpack/ip ${THIRDPARTY_INSTALL_DIR}/include/oscpack/ip &&
                                    ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/oscpack/src/oscpack/osc ${THIRDPARTY_INSTALL_DIR}/include/oscpack/osc &&
                                    ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/oscpack/src/oscpack/liboscpack.a ${THIRDPARTY_INSTALL_DIR}/lib
)
ExternalProject_Add(eigen
                    URL         https://gitlab.com/libeigen/eigen/-/archive/3.3.8/eigen-3.3.8.tar.gz
                    PREFIX      eigen
                    CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_INSTALL_DIR}
)
ExternalProject_Add(sndfile
                    URL         https://github.com/libsndfile/libsndfile/releases/download/v1.0.30/libsndfile-1.0.30.tar.bz2
                    PREFIX      ${SNDFILE_BINARY_DIR}
                    CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_INSTALL_DIR}
)
