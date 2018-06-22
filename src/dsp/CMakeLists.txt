set (target dap_dsp)
set (headers
    ${CMAKE_CURRENT_SOURCE_DIR}/AllPass.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CombFilter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/DelayLine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/FeedbackLine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/IIRFilter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/NoiseGenerator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/OscillatorFunctions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Phaser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Phasor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PwmFunctions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Smoother.h
    ${CMAKE_CURRENT_SOURCE_DIR}/UniformDistribution.h
    )
add_library (${target} INTERFACE)
target_sources (${target} INTERFACE ${headers})
target_link_libraries (${target} INTERFACE dap_base)
add_subdirectory (test)
