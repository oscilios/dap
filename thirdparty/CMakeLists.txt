cmake_minimum_required(VERSION 2.8)
project (dap_thirdparty)

include (ExternalProject)
include (ProcessorCount)

set (THIRDPARTY_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/install)
set (THIRDPARTY_DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}/downloads)
set (PKG_CONFIG_PATH ${THIRDPARTY_INSTALL_DIR}/lib/pkgconfig)
set (PARALLEL_JOBS 1)
ProcessorCount(PARALLEL_JOBS)

if (APPLE)
    execute_process (COMMAND /usr/bin/xcrun --show-sdk-path
                     OUTPUT_VARIABLE XCODE_SDK
    )

    if (NOT XCODE_SDK)
        message (FATAL_ERROR "Could not determine XCODE_SDK path")
    else ()
        string (STRIP ${XCODE_SDK} XCODE_SDK)
        set (CARBON_HEADERS "${XCODE_SDK}/System/Library/Frameworks/Carbon.framework/Versions/Current/Headers")
        message (STATUS "XCODE_SDK: ${XCODE_SDK}")
        message (STATUS "CARBON_HEADERS: ${CARBON_HEADERS}")
    endif ()
endif (APPLE)

###############################################################################
# gtest (does not have a install target)
###############################################################################
set (GTEST_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/googletest-release-1.8.0)
set (GTEST_SOURCE_DIR ${THIRDPARTY_DOWNLOAD_DIR}/googletest-release-1.8.0)
ExternalProject_Add(gtest
                    URL         https://github.com/google/googletest/archive/release-1.8.0.tar.gz
                    PREFIX      ${GTEST_BINARY_DIR}
                    SOURCE_DIR  ${GTEST_SOURCE_DIR}
                    INSTALL_DIR ${THIRDPARTY_INSTALL_DIR}
                    BUILD_COMMAND make -j${PARALLEL_JOBS}
                    CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_INSTALL_DIR} -DBUILD_GMOCK=OFF -DBUILD_GTEST=ON -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory ${GTEST_SOURCE_DIR}/googletest/include ${THIRDPARTY_INSTALL_DIR}/include &&
                                    ${CMAKE_COMMAND} -E copy ${GTEST_BINARY_DIR}/src/gtest-build/googletest/libgtest.a ${THIRDPARTY_INSTALL_DIR}/lib
)
set (GBENCHMARK_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/google_benchmark_1.4.1)
set (GBENCHMARK_SOURCE_DIR ${THIRDPARTY_DOWNLOAD_DIR}/google_benchmark_1.4.1)
ExternalProject_Add(google_benchmark
                    URL         https://github.com/google/benchmark/archive/v1.4.1.zip
                    PREFIX      ${GBENCHMARK_BINARY_DIR}
                    SOURCE_DIR  ${GBENCHMARK_SOURCE_DIR}
                    INSTALL_DIR ${THIRDPARTY_INSTALL_DIR}
                    BUILD_COMMAND make -j${PARALLEL_JOBS}
                    CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_INSTALL_DIR} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DBENCHMARK_ENABLE_TESTING=OFF -DBUILD_SHARED_LIBS=ON
)
if (APPLE)
    ExternalProject_Add_Step(
        google_benchmark google_benchmark_fixup
        DEPENDEES install
        COMMAND install_name_tool -id @rpath/libbenchmark.dylib ${THIRDPARTY_INSTALL_DIR}/lib/libbenchmark.dylib
        COMMENT "Fixing google benchmark install names"
        )
endif ()

################################################################################
# oscpack (does not have a install target via cmake)
################################################################################
set (OSCPACK_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/oscpack_1_1_0)
set (OSCPACK_SOURCE_DIR ${THIRDPARTY_DOWNLOAD_DIR}/oscpack_1_1_0)
ExternalProject_Add(oscpack
                    URL         https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/oscpack/oscpack_1_1_0.zip
                    PREFIX      ${OSCPACK_BINARY_DIR}
                    SOURCE_DIR  ${OSCPACK_SOURCE_DIR}
                    INSTALL_DIR ${THIRDPARTY_INSTALL_DIR}
                    BUILD_IN_SOURCE 1
                    CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_INSTALL_DIR} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DBUILD_SHARED_LIBS=True
                    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory ${OSCPACK_SOURCE_DIR}/ip ${THIRDPARTY_INSTALL_DIR}/include/oscpack/ip &&
                                    ${CMAKE_COMMAND} -E copy_directory ${OSCPACK_SOURCE_DIR}/osc ${THIRDPARTY_INSTALL_DIR}/include/oscpack/osc &&
                                    ${CMAKE_COMMAND} -E copy ${OSCPACK_SOURCE_DIR}/liboscpack.dylib ${THIRDPARTY_INSTALL_DIR}/lib
)
################################################################################
# eigen
################################################################################
set (EIGEN_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/eigen-eigen-07105f7124f9)
set (EIGEN_SOURCE_DIR ${THIRDPARTY_DOWNLOAD_DIR}/eigen-eigen-07105f7124f9)
ExternalProject_Add(eigen
                    URL         http://bitbucket.org/eigen/eigen/get/3.2.8.tar.bz2
                    PREFIX      ${EIGEN_BINARY_DIR}
                    SOURCE_DIR  ${EIGEN_SOURCE_DIR}
                    INSTALL_DIR ${THIRDPARTY_INSTALL_DIR}
                    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory ${EIGEN_SOURCE_DIR}/Eigen ${THIRDPARTY_INSTALL_DIR}/include/Eigen
)

################################################################################
# sndfile
################################################################################
set (SNDFILE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/libsndfile)
set (SNDFILE_SOURCE_DIR ${THIRDPARTY_DOWNLOAD_DIR}/libsndfile)
ExternalProject_Add(sndfile
                    URL         http://www.mega-nerd.com/libsndfile/files/libsndfile-1.0.28.tar.gz
                    PREFIX      ${SNDFILE_BINARY_DIR}
                    SOURCE_DIR  ${SNDFILE_SOURCE_DIR}
                    CONFIGURE_COMMAND PKG_CONFIG_PATH=${PKG_CONFIG_PATH}
                                      ${SNDFILE_SOURCE_DIR}/configure
                                      --prefix=${THIRDPARTY_INSTALL_DIR}
                    INSTALL_DIR ${THIRDPARTY_INSTALL_DIR}
                    BUILD_COMMAND make -j${PARALLEL_JOBS}
                    INSTALL_COMMAND make install
)
