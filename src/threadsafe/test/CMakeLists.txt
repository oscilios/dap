include (DapGtestLinkLibraries)
set (target dap_threadsafe_tests)
set (headers)
set (sources
    RingBufferTest.cpp
    StackTest.cpp
    test.cpp
    )
add_executable (${target} ${sources})
dap_gtest_link_libraries (${target} dap_threadsafe)
add_test (${target} ${target} --gtest_output=xml)

