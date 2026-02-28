#pragma once
#include "base/KeyValueTuple.h"
#include "crtp/nodes/Processor.h"
#include "dsp/Oscillator.h"
#include "dsp/Smoother.h"
#include "fastmath/AudioBuffer.h"
#include <cstddef>

namespace fm
{
    using dap::crtp::processor;
    using dap::operator""_s;
    using scalar_t     = float;
    using samplerate_t = scalar_t;
    class Synth;
}

class fm::Synth final
{
    struct Graph
    {
    private:
        // a smoothed control
        using control_t = decltype(
            processor<dap::dsp::FixedSmoother<scalar_t, 512>>::with_inputs<scalar_t>::named(
                "value"_s));

        // alias for oscillator shapes
        using shape_t = dap::dsp::OscillatorFunctions::Shape;

        // define an oscillator and its input names
        template <typename Amp, typename Freq, typename Ph>
        using osc_t =
            decltype(processor<dap::dsp::Oscillator<scalar_t>>::
                         with_inputs<Amp, Freq, Ph, samplerate_t, shape_t>::named("gain"_s,
                                                                                  "frequency"_s,
                                                                                  "phase"_s,
                                                                                  "samplerate"_s,
                                                                                  "shape"_s));

        // oscillator where gain, frequency and phase are smoothed controls
        using control_osc_t = osc_t<control_t, control_t, control_t>;
        // modulated control: the result of adding a control with an oscillator
        using mod_control_t = decltype(control_t{} + control_osc_t{});
        // an fm oscillator
        using fm_t = osc_t<control_t, mod_control_t, control_t>;

    public:
        using type  = osc_t<control_t, mod_control_t, control_t>;
        using shape = shape_t;
    };

    using buffer_t = dap::fastmath::AudioBuffer<float>;

    buffer_t m_output;
    Graph::type m_graph;

    auto& modulator()
    {
        return m_graph.input("frequency"_s).input("y"_s);
    }

public:
    Synth(size_t bufferSize, float samplerate);

    void process()
    {
        for (auto& x : m_output.channel(0))
        {
            x = m_graph();
        }
    }

    const buffer_t& output() const
    {
        return m_output;
    }

    void setCarrierFreq(float hz)
    {
        m_graph.input("frequency"_s).input("x"_s).input("value"_s) = hz;
    }
    void setModFreq(float hz)
    {
        modulator().input("frequency"_s).input("value"_s) = hz;
    }
    void setModIndex(float idx)
    {
        auto g = idx * modulator().input("frequency"_s).input("value"_s);
        modulator().input("gain"_s).input("value"_s) = g;
    }
    void setGain(float g)
    {
        m_graph.input("gain"_s).input("value"_s) = g;
    }
    auto params()
    {
        using std::make_tuple;

        // clang-format off
        return dap::make_key_value_tuple(
            make_tuple("set_carrier_frequency"_s,     &Synth::setCarrierFreq),
            make_tuple("set_modulation_frequency"_s,  &Synth::setModFreq),
            make_tuple("set_modulation_index"_s,      &Synth::setModIndex),
            make_tuple("set_gain"_s,                  &Synth::setGain)
            );
        // clang-format on
    }
    template <char... Chars>
    auto operator[](dap::constexpr_string<Chars...> key)
    {
        auto memberFuncPtr = params()[key];
        return [this, memberFuncPtr](float value) { (this->*memberFuncPtr)(value); };
    }
};
