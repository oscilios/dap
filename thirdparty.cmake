include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
  SYSTEM
  FIND_PACKAGE_ARGS NAMES GTest
  )

set(BENCHMARK_ENABLE_TESTING OFF)
FetchContent_Declare(
  google_benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.9.4
  SYSTEM
  FIND_PACKAGE_ARGS
  )


FetchContent_Declare(
  oscpack
  GIT_REPOSITORY https://github.com/RossBencina/oscpack.git
  GIT_TAG release_1_1_0
  BUILD_IN_SOURCE 1
  SYSTEM
  FIND_PACKAGE_ARGS
  )

FetchContent_Declare(
  eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG 3.3.8
  SYSTEM
  FIND_PACKAGE_ARGS
  )

FetchContent_Declare(
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
             -DINSTALL_PKGCONFIG_MODULE=OFF
  SYSTEM
  FIND_PACKAGE_ARGS
  )

FetchContent_MakeAvailable(googletest google_benchmark oscpack eigen sndfile)
target_include_directories(oscpack PUBLIC ${oscpack_SOURCE_DIR})
target_compile_options(oscpack PRIVATE -Wno-deprecated-declarations)
