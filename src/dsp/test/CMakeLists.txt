set(target dap_crtp_dsp_tests)

set(headers)
set(sources
    BiquadFilterTest.cpp
    DelayTest.cpp
    IIRFilterTest.cpp
    MixerTest.cpp
    NoiseGeneratorTest.cpp
    VarTests.cpp
    test.cpp)

add_executable(${target} ${sources})
target_link_libraries(${target} dap_dsp dap_fastmath GTest::gtest SndFile::sndfile)
add_test(${target} ${target} --gtest_output=xml)
