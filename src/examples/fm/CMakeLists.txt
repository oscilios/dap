set (target fm_synth)
set (sources
    AudioProcess.cpp
    main.cpp
    FMSynth.cpp
    )
add_executable (${target} ${sources})
target_link_libraries (${target} dap_crtp_utility dap_osc dap_audioio)
