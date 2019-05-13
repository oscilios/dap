#include "OscEventSystem.h"
#include "AudioProcess.h"

using dap::Any;
using any_vector = std::vector<Any>;
using crtp_synth::OscEventSystem;

namespace
{
    template <typename T>
    static auto first(const any_vector& v)
    {
        return v.at(0).to<T>();
    }
}

#define SYNTH_CALLBACK(name, type) \
    m_oscReceiver.addCallback(#name, [&p](any_vector&& v) { p[#name##_s] = first<type>(v); })
#define SYNTH_LAMBDA_CALLBACK(name, type, lambda)                    \
    m_oscReceiver.addCallback(#name, [&p, &lambda](any_vector&& v) { \
        p[#name##_s] = lambda(first<type>(v));                       \
    })

OscEventSystem::OscEventSystem(AudioProcess& p)
{
    using namespace dap;

    auto noiseColor = [](const std::string& color) {
        using color_t = noise_gen_t::Color;
        return (color == "white")
                   ? color_t::White
                   : (color == "pink") ? color_t::Pink
                                       : (color == "brown") ? color_t::Brown : color_t::White;
    };

    auto oscShape = [](const std::string& shape) {
        return shape == "sine"
                   ? osc_shape_t::Sine
                   : shape == "saw" ? osc_shape_t::Saw
                                    : shape == "invsaw" ? osc_shape_t::InverseSaw
                                                        : shape == "square" ? osc_shape_t::Square
                                                                            : osc_shape_t::Triangle;
    };

    // clang-format off
    SYNTH_CALLBACK(/synth/set/bus0/gain/value,             float);
    SYNTH_CALLBACK(/synth/set/bus1/gain/value,             float);
    SYNTH_CALLBACK(/synth/set/bus2/gain/value,             float);
    SYNTH_CALLBACK(/synth/set/filter/frequency/value,      float);
    SYNTH_CALLBACK(/synth/set/filter/resonance/value,      float);

    SYNTH_CALLBACK(/synth/set/phaser/frequency/value,      float);
    SYNTH_CALLBACK(/synth/set/phaser/depth/value,          float);
    SYNTH_CALLBACK(/synth/set/phaser/feedback/value,       float);
    SYNTH_CALLBACK(/synth/set/phaser/wet/value,            float);

    SYNTH_CALLBACK(/synth/set/noise/gain/value,            float);
    SYNTH_CALLBACK(/synth/set/osc5/frequency/value,        float);
    SYNTH_CALLBACK(/synth/set/osc5/frequency/portamento,   size_t);
    SYNTH_CALLBACK(/synth/set/osc5/gain/value,             float);
    SYNTH_CALLBACK(/synth/set/osc5/phase/value,            float);
    SYNTH_CALLBACK(/synth/set/osc6/frequency/value,        float);
    SYNTH_CALLBACK(/synth/set/osc6/frequency/portamento,   size_t);
    SYNTH_CALLBACK(/synth/set/osc6/gain/value,             float);
    SYNTH_CALLBACK(/synth/set/osc6/phase/value,            float);
    SYNTH_CALLBACK(/synth/set/osc1/frequency/value,        float);
    SYNTH_CALLBACK(/synth/set/osc1/gain/value,             float);
    SYNTH_CALLBACK(/synth/set/osc1/phase/value,            float);
    SYNTH_CALLBACK(/synth/set/osc2/frequency/value,        float);
    SYNTH_CALLBACK(/synth/set/osc2/gain/value,             float);
    SYNTH_CALLBACK(/synth/set/osc2/phase/value,            float);
    SYNTH_CALLBACK(/synth/set/osc3/frequency/value,        float);
    SYNTH_CALLBACK(/synth/set/osc3/gain/value,             float);
    SYNTH_CALLBACK(/synth/set/osc3/phase/value,            float);
    SYNTH_CALLBACK(/synth/set/osc4/frequency/value,        float);
    SYNTH_CALLBACK(/synth/set/osc4/gain/value,             float);
    SYNTH_CALLBACK(/synth/set/osc4/phase/value,            float);

    SYNTH_LAMBDA_CALLBACK(/synth/set/noise/color, std::string, noiseColor);
    SYNTH_LAMBDA_CALLBACK(/synth/set/osc5/shape,  std::string, oscShape);
    SYNTH_LAMBDA_CALLBACK(/synth/set/osc6/shape,  std::string, oscShape);
    SYNTH_LAMBDA_CALLBACK(/synth/set/osc1/shape,  std::string, oscShape);
    SYNTH_LAMBDA_CALLBACK(/synth/set/osc2/shape,  std::string, oscShape);
    SYNTH_LAMBDA_CALLBACK(/synth/set/osc3/shape,  std::string, oscShape);
    SYNTH_LAMBDA_CALLBACK(/synth/set/osc4/shape,  std::string, oscShape);
    // clang-format on
}
