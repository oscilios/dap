#include "dsp/Pwm.h"
#include "crtp/nodes/Node.h"
#include "dap_audio_test.h"
#include "dap_gtest.h"
#include "dsp/CombFilter.h"
#include "dsp/Smoother.h"

#include <vector>

using namespace testing;
using namespace dap;
using namespace dap::crtp;

using node_t = dap::crtp::Node;
template <typename... Ts>
using inputs_t = node_t::Inputs<Ts...>;
using BasePwm  = dap::crtp::ProcessorNode<
    dap::dsp::Pwm<float>,
    inputs_t<float, float, float, float, float>,
    NODE_INPUT_NAMES("amplitude"_s, "frequency"_s, "phase"_s, "samplerate"_s, "dutycycle"_s)>;

class PwmTest : public Test
{
};
DAP_TEST_F(PwmTest, SimplePwm)
{
    BasePwm osc;

    const float samplerate = 44100;

    osc.input("frequency"_s)  = 10.0f;
    osc.input("amplitude"_s)  = 0.5f;
    osc.input("phase"_s)      = 0;
    osc.input("samplerate"_s) = samplerate;
    osc.input("dutycycle"_s)  = 0.1f;

    const int format        = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    const int channels      = 1;
    const size_t bufferSize = 512;
    std::vector<float> buffer(bufferSize, 0.0f);
    size_t length = 0;
    SndfileHandle file("simple_pwm.wav", SFM_WRITE, format, channels, int(samplerate));
    if (file.error() != 0)
    {
        std::cout << file.strError() << std::endl;
        return;
    }

    while (length < samplerate)
    {
        if (length > samplerate / 2)
            osc.input("frequency"_s) = 40.0f;

        const auto frames = std::min(bufferSize, size_t(samplerate) - length);
        for (size_t i = 0; i < frames; i++)
        {
            buffer[i] = osc();
        }
        length += file.writef(buffer.data(), frames);
    }

    DAP_ASSERT_AUDIO_EQ("simple_pwm_expected.wav", "simple_pwm.wav");
}
DAP_TEST_F(PwmTest, FeedbackPwm)
{
    using CombFilter =
        dap::crtp::ProcessorNode<dap::dsp::FeedbackCombFilter<float, 32768>,
                                 inputs_t<BasePwm, float, float>,
                                 NODE_INPUT_NAMES("input"_s, "delay"_s, "feedback"_s)>;
    CombFilter comb;
    auto& osc = comb.input("input"_s);

    const float samplerate = 44100;

    osc.input("frequency"_s)  = 4.0f;
    osc.input("amplitude"_s)  = 0.5f;
    osc.input("phase"_s)      = 0;
    osc.input("samplerate"_s) = samplerate;
    osc.input("dutycycle"_s)  = 0.01f;

    comb.input("delay"_s)    = samplerate / 440.0f;
    comb.input("feedback"_s) = 0.95f;

    const int format        = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    const int channels      = 1;
    const size_t bufferSize = 512;
    std::vector<float> buffer(bufferSize, 0.0f);
    size_t length = 0;
    SndfileHandle file("feedback_pwm.wav", SFM_WRITE, format, channels, int(samplerate));
    if (file.error() != 0)
    {
        std::cout << file.strError() << std::endl;
        return;
    }

    while (length < samplerate)
    {
        const auto frames = std::min(bufferSize, size_t(samplerate) - length);
        for (size_t i = 0; i < frames; i++)
        {
            buffer[i] = comb();
        }
        length += file.writef(buffer.data(), frames);
    }

    DAP_ASSERT_AUDIO_EQ("feedback_pwm_expected.wav", "feedback_pwm.wav");
}
