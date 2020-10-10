set (target dap_base_tests)

set (headers )
set (sources
    AbstractFactoryTest.cpp
    AnyTest.cpp
    TypeTraitsTest.cpp
    test.cpp
    )

add_executable (${target} ${sources})
target_link_libraries (${target} dap_base GTest::gtest)
add_test (${target} ${target} --gtest_output=xml)

