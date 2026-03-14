#include "dsp/BiquadFilter.h"
#include "dap_audio_test.h"
#include <cmath>
#include <gtest/gtest.h>
#include <vector>

using namespace testing;
using namespace dap::dsp;

using Biquad = BiquadFilter<float>;

namespace
{
    constexpr float samplerate = 44100.0f;
    constexpr float frequency  = 1000.0f;
    constexpr float Q          = 0.707f;
    constexpr float gainDb     = 6.0f;
    constexpr int bufferSize   = 512;

    // Generate a buffer with a unit impulse (1 at sample 0, 0 elsewhere)
    std::vector<float> impulse(int size)
    {
        std::vector<float> buf(size, 0.0f);
        buf[0] = 1.0f;
        return buf;
    }

    std::string toString(Biquad::Type type)
    {
        switch (type)
        {
            case Biquad::Type::LowPass:
                return "LowPass";
            case Biquad::Type::HighPass:
                return "HighPass";
            case Biquad::Type::BandPass:
                return "BandPass";
            case Biquad::Type::Notch:
                return "Notch";
            case Biquad::Type::Peak:
                return "Peak";
            case Biquad::Type::LowShelf:
                return "LowShelf";
            case Biquad::Type::HighShelf:
                return "HighShelf";
        }
    }

    // Process an impulse through the filter and check no sample is NaN or Inf
    void processAndCheckFinite(Biquad& filter, Biquad::Type type)
    {
        const auto input = impulse(bufferSize);
        std::vector<float> outBuffer(bufferSize, 0);

        const int format   = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
        const int channels = 1;
        std::vector<float> buffer(bufferSize, 0.0f);

        const auto filename = toString(type) + ".wav";

        SndfileHandle file(filename.c_str(), SFM_WRITE, format, channels, int(samplerate));

        if (file.error() != 0)
        {
            std::cout << file.strError() << std::endl;
            return;
        }

        for (int i = 0; i < bufferSize; ++i)
        {
            float out    = filter(input[i], frequency, Q, type, gainDb, samplerate);
            outBuffer[i] = out;
            ASSERT_FALSE(std::isnan(out)) << "NaN at sample " << i;
            ASSERT_FALSE(std::isinf(out)) << "Inf at sample " << i;
        }

        file.writef(outBuffer.data(), outBuffer.size());
    }
} // namespace

TEST(BiquadFilterTest, lowPass)
{
    Biquad filter;
    processAndCheckFinite(filter, Biquad::Type::LowPass);
}

TEST(BiquadFilterTest, highPass)
{
    Biquad filter;
    processAndCheckFinite(filter, Biquad::Type::HighPass);
}

TEST(BiquadFilterTest, bandPass)
{
    Biquad filter;
    processAndCheckFinite(filter, Biquad::Type::BandPass);
}

TEST(BiquadFilterTest, notch)
{
    Biquad filter;
    processAndCheckFinite(filter, Biquad::Type::Notch);
}

TEST(BiquadFilterTest, peak)
{
    Biquad filter;
    processAndCheckFinite(filter, Biquad::Type::Peak);
}

TEST(BiquadFilterTest, lowShelf)
{
    Biquad filter;
    processAndCheckFinite(filter, Biquad::Type::LowShelf);
}

TEST(BiquadFilterTest, highShelf)
{
    Biquad filter;
    processAndCheckFinite(filter, Biquad::Type::HighShelf);
}

TEST(BiquadFilterTest, typeSwitchResetsState)
{
    Biquad filter;
    auto input = impulse(bufferSize);

    // Run as LowPass for half the buffer
    for (int i = 0; i < bufferSize / 2; ++i)
    {
        filter(input[i], frequency, Q, Biquad::Type::LowPass, 0.0f, samplerate);
    }

    // Switch to HighPass — IIRFilter state should reset
    for (int i = bufferSize / 2; i < bufferSize; ++i)
    {
        float out = filter(0.0f, frequency, Q, Biquad::Type::HighPass, 0.0f, samplerate);
        ASSERT_FALSE(std::isnan(out)) << "NaN at sample " << i << " after type switch";
        ASSERT_FALSE(std::isinf(out)) << "Inf at sample " << i << " after type switch";
    }

    // After reset, feeding silence should produce silence
    for (int i = 0; i < bufferSize; ++i)
    {
        float out = filter(0.0f, frequency, Q, Biquad::Type::HighPass, 0.0f, samplerate);
        ASSERT_NEAR(out, 0.0f, 1e-6f) << "Expected silence at sample " << i;
    }
}

TEST(BiquadFilterTest, lowPassAttenuatesHighFrequency)
{
    Biquad filter;
    float cutoff = 200.0f;

    // Generate a high frequency sine (10kHz) well above the cutoff
    float highFreq      = 10000.0f;
    float peakAmplitude = 0.0f;

    // Let the filter settle
    for (int i = 0; i < 4096; ++i)
    {
        float input = std::sin(2.0f * float(M_PI) * highFreq * float(i) / samplerate);
        float out   = filter(input, cutoff, Q, Biquad::Type::LowPass, 0.0f, samplerate);
        if (i > 2048)
        {
            peakAmplitude = std::max(peakAmplitude, std::fabs(out));
        }
    }

    // 10kHz through a 200Hz LPF should be heavily attenuated
    ASSERT_LT(peakAmplitude, 0.01f);
}

TEST(BiquadFilterTest, highPassAttenuatesLowFrequency)
{
    Biquad filter;
    float cutoff = 5000.0f;

    // Generate a low frequency sine (100Hz) well below the cutoff
    float lowFreq       = 100.0f;
    float peakAmplitude = 0.0f;

    for (int i = 0; i < 4096; ++i)
    {
        float input = std::sin(2.0f * float(M_PI) * lowFreq * float(i) / samplerate);
        float out   = filter(input, cutoff, Q, Biquad::Type::HighPass, 0.0f, samplerate);
        if (i > 2048)
        {
            peakAmplitude = std::max(peakAmplitude, std::fabs(out));
        }
    }

    // 100Hz through a 5kHz HPF should be heavily attenuated
    ASSERT_LT(peakAmplitude, 0.01f);
}

TEST(BiquadFilterTest, noNanWithZeroQ)
{
    Biquad filter;
    auto input = impulse(bufferSize);

    // Q of 0 could cause division by zero in alpha = sin(w0) / (2*Q)
    for (int i = 0; i < bufferSize; ++i)
    {
        float out = filter(input[i], frequency, 0.0f, Biquad::Type::LowPass, 0.0f, samplerate);
        ASSERT_FALSE(std::isnan(out)) << "NaN at sample " << i << " with Q=0";
    }
}

TEST(BiquadFilterTest, noNanWithZeroFrequency)
{
    Biquad filter;
    auto input = impulse(bufferSize);

    for (int i = 0; i < bufferSize; ++i)
    {
        float out = filter(input[i], 0.0f, Q, Biquad::Type::LowPass, 0.0f, samplerate);
        ASSERT_FALSE(std::isnan(out)) << "NaN at sample " << i << " with freq=0";
    }
}

TEST(BiquadFilterTest, noNanWithNyquistFrequency)
{
    Biquad filter;
    auto input    = impulse(bufferSize);
    float nyquist = samplerate / 2.0f;

    for (int i = 0; i < bufferSize; ++i)
    {
        float out = filter(input[i], nyquist, Q, Biquad::Type::LowPass, 0.0f, samplerate);
        ASSERT_FALSE(std::isnan(out)) << "NaN at sample " << i << " at Nyquist";
    }
}

TEST(BiquadFilterTest, noNanWithNegativeGainDb)
{
    Biquad filter;
    auto input = impulse(bufferSize);

    for (int i = 0; i < bufferSize; ++i)
    {
        float out = filter(input[i], frequency, Q, Biquad::Type::Peak, -12.0f, samplerate);
        ASSERT_FALSE(std::isnan(out)) << "NaN at sample " << i << " with negative gainDb";
    }
}

TEST(BiquadFilterTest, noNanWithLargeGainDb)
{
    Biquad filter;
    auto input = impulse(bufferSize);

    for (int i = 0; i < bufferSize; ++i)
    {
        float out = filter(input[i], frequency, Q, Biquad::Type::LowShelf, 24.0f, samplerate);
        ASSERT_FALSE(std::isnan(out)) << "NaN at sample " << i << " with large gainDb";
        ASSERT_FALSE(std::isinf(out)) << "Inf at sample " << i << " with large gainDb";
    }
}
