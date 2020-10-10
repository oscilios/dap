#include "crtp/nodes/Node.h"
#include "dap_audio_test.h"
#include <gtest/gtest.h>
#include "dsp/FeedbackLine.h"
#include "dsp/NoiseGenerator.h"
#include "dsp/Smoother.h"
#include "dsp/UniformDistribution.h"

#include <vector>

using namespace testing;
using namespace dap;
using namespace dap::crtp;

using node_t = dap::crtp::Node;
template <typename... Ts>
using inputs_t = node_t::Inputs<Ts...>;
using Control  = dap::crtp::ProcessorNode<dap::dsp::Smoother<float>,
                                         inputs_t<float, size_t>,
                                         NODE_INPUT_NAMES("value"_s, "duration"_s)>;
using NoiseGen = dap::dsp::NoiseGenerator<dap::dsp::UniformDistribution>;
using noise_t  = dap::crtp::ProcessorNode<NoiseGen,
                                         inputs_t<Control, NoiseGen::Color>,
                                         NODE_INPUT_NAMES("amplitude"_s, "color"_s)>;

TEST(NoiseTest, OneOverF3)
{
    noise_t noise;

    const size_t portamentoSamples                 = 4096;
    noise.input("color"_s)                         = NoiseGen::Color::OneOverF3;
    noise.input("amplitude"_s).input("value"_s)    = 0.5f;
    noise.input("amplitude"_s).input("duration"_s) = portamentoSamples;

    const int format        = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    const int channels      = 1;
    const size_t bufferSize = 512;
    std::vector<float> buffer(bufferSize, 0.0f);
    size_t length     = 0;
    size_t sampleRate = 44100;
    SndfileHandle file("noise.wav", SFM_WRITE, format, channels, sampleRate);
    if (file.error() != 0)
    {
        std::cout << file.strError() << std::endl;
        return;
    }

    while (length < sampleRate)
    {
        if (length > sampleRate / 2)
            noise.input("amplitude"_s).input("value"_s) = 0.1f;

        const auto frames = std::min(bufferSize, sampleRate - length);
        for (size_t i = 0; i < frames; i++)
            buffer[i] = noise();
        length += file.writef(buffer.data(), frames);
    }
}
TEST(NoiseTest, feedback)
{
    using Delay =
        dap::crtp::ProcessorNode<dap::dsp::FeedbackLine<float, 32768>,
                                 inputs_t<noise_t, float, float>,
                                 NODE_INPUT_NAMES("signal"_s, "delay"_s, "feedback"_s)>;
    Delay delay;

    auto& noise                                    = delay.input("signal"_s);
    const size_t portamentoSamples                 = 512;
    noise.input("color"_s)                         = NoiseGen::Color::White;
    noise.input("amplitude"_s).input("value"_s)    = 0.5f;
    noise.input("amplitude"_s).input("duration"_s) = portamentoSamples;

    delay.input("delay"_s)    = 256;
    delay.input("feedback"_s) = 0.9f;

    const int format        = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    const int channels      = 1;
    const size_t bufferSize = 512;
    std::vector<float> buffer(bufferSize, 0.0f);
    size_t length     = 0;
    size_t sampleRate = 44100;
    SndfileHandle file("feedback_noise.wav", SFM_WRITE, format, channels, sampleRate);
    if (file.error() != 0)
    {
        std::cout << file.strError() << std::endl;
        return;
    }

    while (length < sampleRate)
    {
        const auto frames = std::min(bufferSize, sampleRate - length);
        for (size_t i = 0; i < frames; i++)
            buffer[i] = delay() * 0.25f;
        length += file.writef(buffer.data(), frames);
    }
}
