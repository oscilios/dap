set (target dap_fastmath_tests)

set (headers)
set (sources
    ArrayOpsTest.cpp
    ArrayTest.cpp
    AudioBufferTest.cpp
    FunctionTest.cpp
    TaylorTest.cpp
    VarArrayTest.cpp
    VariableTest.cpp
    test.cpp
    )

add_executable (${target} ${sources} ${headers})
target_link_libraries (${target} dap_fastmath GTest::gtest)
add_test (${target} ${target} --gtest_output=xml)

