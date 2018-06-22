#include "dsp/Oscillator.h"
#include "base/Streamable.h"
#include "crtp/nodes/Node.h"
#include "dap_audio_test.h"
#include "dap_gtest.h"
#include "dsp/Smoother.h"

#include <vector>

using namespace testing;
using namespace dap;
using namespace dap::crtp;
using dap::dsp::OscillatorFunctions;
using node_t = dap::crtp::Node;
template <typename... Ts>
using inputs_t = node_t::Inputs<Ts...>;
template <typename Amp, typename Freq, typename Ph>
using BaseOsc = dap::crtp::ProcessorNode<
    dap::dsp::Oscillator<float>,
    inputs_t<Amp, Freq, Ph, float, OscillatorFunctions::Shape>,
    NODE_INPUT_NAMES("amplitude"_s, "frequency"_s, "phase"_s, "samplerate"_s, "shape"_s)>;

using Control = dap::crtp::ProcessorNode<dap::dsp::Smoother<float>,
                                         inputs_t<float, size_t>,
                                         NODE_INPUT_NAMES("value"_s, "duration"_s)>;

using AmpControl    = Control;
using FreqControl   = Control;
using PhaseControl  = Control;
using Osc           = BaseOsc<AmpControl, FreqControl, PhaseControl>;
using Modulator     = decltype(Osc{} + Control{});
using FreqModulator = Modulator;
using FMOsc         = BaseOsc<AmpControl, FreqModulator, PhaseControl>;

class OscillatorTest : public Test
{
};
DAP_TEST_F(OscillatorTest, SinOscWithoutPortamentoTest)
{
    BaseOsc<float, float, float> osc;

    const float samplerate = 44100;

    osc.input("frequency"_s)  = 440.0f;
    osc.input("amplitude"_s)  = 0.5f;
    osc.input("phase"_s)      = 0;
    osc.input("samplerate"_s) = samplerate;
    osc.input("shape"_s)      = OscillatorFunctions::Shape::Sine;

    const int format        = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    const int channels      = 1;
    const size_t bufferSize = 512;
    std::vector<float> buffer(bufferSize, 0.0f);
    size_t length = 0;
    SndfileHandle file("simple_osc.wav", SFM_WRITE, format, channels, int(samplerate));
    if (file.error() != 0)
    {
        std::cout << file.strError() << std::endl;
        return;
    }

    while (length < samplerate)
    {
        if (length > samplerate / 2)
            osc.input("frequency"_s) = 880.0f;

        const auto frames = std::min(bufferSize, size_t(samplerate) - length);
        for (size_t i = 0; i < frames; i++)
        {
            buffer[i] = osc();
        }
        length += file.writef(buffer.data(), frames);
    }

    DAP_ASSERT_AUDIO_EQ("simple_osc_expected.wav", "simple_osc.wav");
}

DAP_TEST_F(OscillatorTest, SinOscTest)
{
    Osc osc;

    const float samplerate         = 44100;
    const size_t portamentoSamples = 4096;

    osc.input("frequency"_s).input("value"_s)    = 440.0f;
    osc.input("frequency"_s).input("duration"_s) = portamentoSamples;

    osc.input("amplitude"_s).input("value"_s)    = 0.5f;
    osc.input("amplitude"_s).input("duration"_s) = portamentoSamples;

    osc.input("phase"_s).input("value"_s)    = 0;
    osc.input("phase"_s).input("duration"_s) = 0;

    osc.input("samplerate"_s) = samplerate;
    osc.input("shape"_s)      = OscillatorFunctions::Shape::Sine;

    const int format        = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    const int channels      = 1;
    const size_t bufferSize = 512;
    std::vector<float> buffer(bufferSize, 0.0f);
    size_t length = 0;
    SndfileHandle file("osc.wav", SFM_WRITE, format, channels, int(samplerate));
    if (file.error() != 0)
    {
        std::cout << file.strError() << std::endl;
        return;
    }

    while (length < samplerate)
    {
        if (length > samplerate / 2)
            osc.input("frequency"_s).input("value"_s) = 880.0f;

        const auto frames = std::min(bufferSize, size_t(samplerate) - length);
        for (size_t i = 0; i < frames; i++)
        {
            buffer[i] = osc();
        }
        length += file.writef(buffer.data(), frames);
    }

    DAP_ASSERT_AUDIO_EQ("osc_expected.wav", "osc.wav");
}
DAP_TEST_F(OscillatorTest, AdditiveTest)
{
    Osc osc1;
    Osc osc2;

    const float samplerate         = 44100.0f;
    const size_t portamentoSamples = 4096;

    osc1.input("frequency"_s).input("value"_s)    = 400.0f;
    osc1.input("frequency"_s).input("duration"_s) = portamentoSamples;

    osc1.input("amplitude"_s).input("value"_s)    = 0.5f;
    osc1.input("amplitude"_s).input("duration"_s) = portamentoSamples;

    osc1.input("phase"_s).input("value"_s)    = 0;
    osc1.input("phase"_s).input("duration"_s) = 0;

    osc1.input("samplerate"_s) = samplerate;
    osc1.input("shape"_s)      = OscillatorFunctions::Shape::Sine;

    osc2.input("frequency"_s).input("value"_s)    = 440.0f;
    osc2.input("frequency"_s).input("duration"_s) = portamentoSamples;

    osc2.input("amplitude"_s).input("value"_s)    = 0.5f;
    osc2.input("amplitude"_s).input("duration"_s) = portamentoSamples;

    osc2.input("phase"_s).input("value"_s)    = 0;
    osc2.input("phase"_s).input("duration"_s) = 0;

    osc2.input("samplerate"_s) = samplerate;
    osc2.input("shape"_s)      = OscillatorFunctions::Shape::Sine;

    const int format        = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    const int channels      = 1;
    const size_t bufferSize = 512;
    std::vector<float> buffer(bufferSize, 0.0f);
    size_t length = 0;
    SndfileHandle file("additive.wav", SFM_WRITE, format, channels, int(samplerate));
    if (file.error() != 0)
    {
        std::cout << file.strError() << std::endl;
        return;
    }

    auto sum = osc1 + osc2;
    while (length < samplerate)
    {
        const auto frames = std::min(bufferSize, size_t(samplerate) - length);
        for (size_t i = 0; i < frames; i++)
        {
            buffer[i] = sum();
        }
        length += file.writef(buffer.data(), frames);
    }
    DAP_ASSERT_AUDIO_EQ("additive_expected.wav", "additive.wav");
}
DAP_TEST_F(OscillatorTest, RMTest)
{
    using RMOscillator = MultiplyNode<Osc*, Osc*>;

    Osc osc1;
    Osc osc2;
    RMOscillator rm(&osc1, &osc2);

    const float samplerate         = 44100.0f;
    const size_t portamentoSamples = 4096;

    osc1.input("frequency"_s).input("value"_s)    = 110.0f;
    osc1.input("frequency"_s).input("duration"_s) = portamentoSamples;

    osc1.input("amplitude"_s).input("value"_s)    = 0.5f;
    osc1.input("amplitude"_s).input("duration"_s) = portamentoSamples;

    osc1.input("phase"_s).input("value"_s)    = 0;
    osc1.input("phase"_s).input("duration"_s) = 0;

    osc1.input("samplerate"_s) = samplerate;
    osc1.input("shape"_s)      = OscillatorFunctions::Shape::Sine;

    osc2.input("frequency"_s).input("value"_s)    = 440.0f;
    osc2.input("frequency"_s).input("duration"_s) = portamentoSamples;

    osc2.input("amplitude"_s).input("value"_s)    = 0.5f;
    osc2.input("amplitude"_s).input("duration"_s) = portamentoSamples;

    osc2.input("phase"_s).input("value"_s)    = 0;
    osc2.input("phase"_s).input("duration"_s) = 0;

    osc2.input("samplerate"_s) = samplerate;
    osc2.input("shape"_s)      = OscillatorFunctions::Shape::Sine;

    const int format        = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    const int channels      = 1;
    const size_t bufferSize = 512;
    std::vector<float> buffer(bufferSize, 0.0f);
    size_t length = 0;
    SndfileHandle file("rm.wav", SFM_WRITE, format, channels, int(samplerate));
    if (file.error() != 0)
    {
        std::cout << file.strError() << std::endl;
        return;
    }

    while (length < samplerate)
    {
        if (length > samplerate / 2)
            osc1.input("frequency"_s).input("value"_s) = 220.0f;

        const auto frames = std::min(bufferSize, size_t(samplerate) - length);
        for (size_t i = 0; i < frames; i++)
        {
            buffer[i] = rm();
        }
        length += file.writef(buffer.data(), frames);
    }
    DAP_ASSERT_AUDIO_EQ("rm_expected.wav", "rm.wav");
}
DAP_TEST_F(OscillatorTest, ModTest)
{
    const float samplerate = 44100.0;

    Modulator mod;
    auto& osc    = mod.input("x"_s);
    auto& offset = mod.input("y"_s);

    osc.input("frequency"_s).input("value"_s)    = 4400.0f;
    osc.input("frequency"_s).input("duration"_s) = 0;

    osc.input("amplitude"_s).input("value"_s)    = 0.5f;
    osc.input("amplitude"_s).input("duration"_s) = 0;

    osc.input("phase"_s).input("value"_s)    = 0.0f;
    osc.input("phase"_s).input("duration"_s) = 0;

    osc.input("samplerate"_s) = samplerate;
    osc.input("shape"_s)      = OscillatorFunctions::Shape::Sine;

    offset.input("value"_s)    = 10.0f;
    offset.input("duration"_s) = 0;

    float max_ = std::numeric_limits<float>::min();
    float min_ = std::numeric_limits<float>::max();
    for (size_t i = 0; i < 4096; i++)
    {
        const auto x = mod();
        min_         = std::min(x, min_);
        max_         = std::max(x, max_);
    }
    DAP_ASSERT_FLOAT_EQ(9.5f, min_);
    DAP_ASSERT_FLOAT_EQ(10.5f, max_);
}
DAP_TEST_F(OscillatorTest, FMTest)
{
    FMOsc osc;

    const float samplerate         = 44100.0f;
    const size_t portamentoSamples = 4096;

    osc.input("amplitude"_s).input("value"_s)    = 0.5f;
    osc.input("amplitude"_s).input("duration"_s) = portamentoSamples;

    osc.input("phase"_s).input("value"_s)    = 0;
    osc.input("phase"_s).input("duration"_s) = 0;

    osc.input("samplerate"_s) = samplerate;
    osc.input("shape"_s)      = OscillatorFunctions::Shape::Sine;

    auto& mod    = osc.input("frequency"_s);
    auto& modOsc = mod.input("x"_s);
    auto& freq   = mod.input("y"_s);

    freq.input("value"_s)    = 440.0f;
    freq.input("duration"_s) = portamentoSamples;

    modOsc.input("frequency"_s).input("value"_s)    = 130.81f;
    modOsc.input("frequency"_s).input("duration"_s) = portamentoSamples;

    modOsc.input("amplitude"_s).input("value"_s) = 50.0f;
    modOsc.input("amplitude"_s).input("duration"_s) = portamentoSamples;

    modOsc.input("phase"_s).input("value"_s)    = 0;
    modOsc.input("phase"_s).input("duration"_s) = 0;

    modOsc.input("samplerate"_s) = samplerate;
    modOsc.input("shape"_s)      = OscillatorFunctions::Shape::Sine;

    const int format        = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    const int channels      = 1;
    const size_t bufferSize = 512;
    std::vector<float> buffer(bufferSize, 0.0f);
    size_t length = 0;
    SndfileHandle file("fm.wav", SFM_WRITE, format, channels, int(samplerate));
    if (file.error() != 0)
    {
        std::cout << file.strError() << std::endl;
        return;
    }

    while (length < samplerate)
    {
        if (length > samplerate / 2)
        {
            // freq.input<Control::Input::Value>()    = 220.0f; // A
            freq.input("value"_s)                        = 220.0f;
            modOsc.input("frequency"_s).input("value"_s) = 174.61f; // F
        }

        const auto frames = std::min(bufferSize, size_t(samplerate) - length);
        for (size_t i = 0; i < frames; i++)
        {
            ValueNode<float> res = osc;
            buffer[i]            = res();
        }
        length += file.writef(buffer.data(), frames);
    }
    DAP_ASSERT_AUDIO_EQ("fm_expected.wav", "fm.wav");
}
