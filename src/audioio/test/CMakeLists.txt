set (target dap_audioio_tests)

set (headers
    PassThroughAudioProcess.h
    )
set (sources
    AudioInputTest.cpp
    AudioOutputTest.cpp
    AudioProcessTest.cpp
    PassThroughAudioProcess.cpp
    test.cpp
    )

add_executable (${target} ${sources} ${headers})
target_link_libraries (${target} dap_audioio GTest::gtest)
add_test (${target} ${target} --gtest_output=xml)
