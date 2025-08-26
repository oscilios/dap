set (target dap_osc_tests)
add_executable (${target} OscReceiverTest.cpp test.cpp)
target_link_libraries (${target} dap_osc oscpack GTest::gtest)
add_test (${target} ${target} --gtest_output=xml)
