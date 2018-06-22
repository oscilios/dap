include (DapGtestLinkLibraries)
set (target dap_crtp_nodes_tests)

set (headers
    )
set (sources
    NodeTest.cpp
    NoiseTest.cpp
    OscillatorTest.cpp
    ProcessorNodeTest.cpp
    PwmTest.cpp
    test.cpp
    )

add_executable(${target} ${sources})
dap_gtest_link_libraries(${target} dap_crtp_nodes dap_fastmath sndfile)
add_test (${target} ${target} --gtest_output=xml)

foreach (f
        additive_expected.wav
        fm_expected.wav
        osc_expected.wav
        simple_osc_expected.wav
        rm_expected.wav
        simple_pwm_expected.wav
        feedback_pwm_expected.wav
        )
    add_custom_command(
        TARGET
        ${target}
        POST_BUILD
        COMMAND
        ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${f} $<TARGET_FILE_DIR:${target}>
        )
endforeach ()
