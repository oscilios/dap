#ifndef DAP_AUDIO_TEST_H
#define DAP_AUDIO_TEST_H

#include <gtest/gtest.h>
#include <sndfile.hh>

inline void DAP_ASSERT_AUDIO_EQ(const char* expectedFilename, const char* actualFilename)
{
    SndfileHandle expected(expectedFilename, SFM_READ);
    ASSERT_TRUE(expected.error() == 0) <<  "error: expected audio file not found";
    SndfileHandle actual(actualFilename, SFM_READ);
    ASSERT_TRUE(actual.error() == 0) << "error: actual audio file not foudn";
    const auto expectedChannels = expected.channels();
    const auto actualChannels   = actual.channels();
    ASSERT_EQ(expectedChannels, actualChannels);
    ASSERT_EQ(expected.samplerate(), actual.samplerate());
    const auto expectedFrames = expected.frames();
    const auto actualFrames   = actual.frames();
    ASSERT_EQ(expectedFrames, actualFrames);
    std::vector<float> expectedBuffer(expectedFrames * expectedChannels, 0.0f);
    std::vector<float> actualBuffer(actualFrames * actualChannels, 0.0f);
    expected.read(expectedBuffer.data(), expectedFrames * expectedChannels);
    actual.read(actualBuffer.data(), actualFrames * expectedChannels);
    ASSERT_EQ(expectedBuffer, actualBuffer);
}
#endif // DAP_AUDIO_TEST_H
