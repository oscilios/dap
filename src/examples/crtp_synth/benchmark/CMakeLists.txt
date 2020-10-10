set (target crtp_synth_benchmark)
add_executable (${target} ../Synth.cpp main.cpp)
target_link_libraries (${target} dap_crtp_utility dap_dsp benchmark::benchmark)
