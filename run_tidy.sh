#!/bin/bash
# this script should be run in the build directory
DEV_DIR=$(dirname $0)
DEFS='-DEIGEN_NO_MALLOC'
echo ${DEV_DIR}/run_clang_tidy.py -clang_options="-std=c++14 -stdlib=libc++ ${DEFS} -I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1 -Ithirdparty/install/include -I${DEV_DIR}/src"
${DEV_DIR}/run_clang_tidy.py -clang_options="-std=c++14 -stdlib=libc++ ${DEFS} -I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1 -Ithirdparty/install/include -I${DEV_DIR}/src"

