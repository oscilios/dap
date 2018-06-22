include (DapGtestLinkLibraries)
set (target dap_crtp_ugen_tests)

file (GLOB headers *.h)
file (GLOB sources *.cpp)

add_executable(${target} ${sources})
dap_gtest_link_libraries(${target} dap_crtp_ugen dap_fastmath)
add_test (${target} ${target} --gtest_output=xml)
