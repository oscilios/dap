set (target fm_synth)
set (sources
    main.cpp
    AudioProcess.cpp
    FMSynth.cpp
    )
add_executable (${target} ${sources})
target_link_libraries (${target} dap_crtp_utility dap_osc dap_audioio)
