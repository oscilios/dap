set (target dap_crtp_ugen_tests)

file (GLOB headers *.h)
file (GLOB sources *.cpp)

add_executable(${target} ${sources})
target_link_libraries(${target} dap_crtp_ugen dap_fastmath GTest::gtest)
add_test (${target} ${target} --gtest_output=xml)
