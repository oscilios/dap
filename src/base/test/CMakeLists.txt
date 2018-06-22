include (DapGtestLinkLibraries)
set (target dap_base_tests)

set (headers )
set (sources
    AbstractFactoryTest.cpp
    AnyTest.cpp
    TypeTraitsTest.cpp
    test.cpp
    )

add_executable (${target} ${sources})
dap_gtest_link_libraries (${target} dap_base)
add_test (${target} ${target} --gtest_output=xml)

