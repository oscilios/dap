set (target crtp_synth)
set (sources
    main.cpp
    AudioProcess.cpp
    Synth.cpp
    OscEventSystem.cpp
    )
add_executable (${target} ${sources})
target_link_libraries (${target} dap_crtp_utility dap_osc dap_audioio)
add_subdirectory(benchmark)
