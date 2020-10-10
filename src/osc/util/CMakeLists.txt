set (target control_sender)
add_executable (${target} control_sender.cpp)
target_link_libraries (${target} dap_osc oscpack::oscpack)
