set (target dap_crtp_ugen)
set (headers
    ${CMAKE_CURRENT_SOURCE_DIR}/Mixer.h
    )
add_library (${target} INTERFACE)
target_sources (${target} INTERFACE ${headers})
target_link_libraries (${target} INTERFACE dap_base dap_crtp_nodes dap_dsp)
add_subdirectory (test)
