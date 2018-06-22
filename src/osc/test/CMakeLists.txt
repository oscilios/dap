set (target dap_osc_tests)
add_executable (${target} OscReceiverTest.cpp test.cpp)
dap_gtest_link_libraries (${target} dap_osc oscpack)
add_test (${target} ${target} --gtest_output=xml)
