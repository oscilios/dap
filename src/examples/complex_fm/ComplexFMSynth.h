#pragma once
#include "base/KeyValueTuple.h"
#include "crtp/nodes/Processor.h"
#include "dsp/EnvelopeGenerator.h"
#include "dsp/LadderFilter.h"
#include "dsp/Mixer.h"
#include "dsp/Oscillator.h"
#include "dsp/Phaser.h"
#include "dsp/Smoother.h"
#include "fastmath/AudioBuffer.h"
#include <cstddef>

namespace complex_fm
{
    using dap::crtp::processor;
    using dap::operator""_s;
    using scalar_t     = float;
    using samplerate_t = scalar_t;
    class Synth;
}

class complex_fm::Synth final
{
    struct Graph
    {
        // Basic building blocks
        using control_t = decltype(processor<dap::dsp::FixedSmoother<scalar_t, 512>>::with_inputs<
                                   scalar_t>::named("value"_s));
        using shape_t   = dap::dsp::OscillatorFunctions::Shape;

        using portamento_t =
            decltype(processor<dap::dsp::Smoother<scalar_t>>::with_inputs<scalar_t, size_t>::named(
                "value"_s,
                "duration"_s));

        // Oscillator template
        template <typename Amp, typename Freq, typename Ph>
        using osc_t =
            decltype(processor<dap::dsp::Oscillator<scalar_t>>::
                         with_inputs<Amp, Freq, Ph, samplerate_t, shape_t>::named("gain"_s,
                                                                                  "frequency"_s,
                                                                                  "phase"_s,
                                                                                  "samplerate"_s,
                                                                                  "shape"_s));

        // Control oscillator for modulation
        using control_osc_t = osc_t<control_t, control_t, control_t>;

        // modulated control:
        using mod_control_t            = decltype(control_t{} + control_osc_t{});
        using portamento_mod_control_t = decltype(portamento_t{} + control_osc_t{});

        // friendly aliases
        using vibrato_mod_t = portamento_mod_control_t;
        using amp_mod_t     = mod_control_t;

        using fm_vib_mod_t = decltype(vibrato_mod_t{} + control_osc_t{});

        // fm oscillator
        using fm_osc_t = osc_t<amp_mod_t, fm_vib_mod_t, control_t>;

        template <typename T>
        using bus_t =
            decltype(processor<dap::dsp::Mixer::Bus>::with_inputs<control_t, T>::named("gain"_s,
                                                                                       "signal"_s));

        template <typename... Ts>
        using mixer_t =
            decltype(processor<dap::dsp::Mixer>::with_inputs<bus_t<Ts>...>::prefixed_by("bus_"_s));

        using envelope_t =
            decltype(processor<dap::dsp::EnvelopeGenerator<scalar_t>>::
                         with_inputs<scalar_t, scalar_t, scalar_t, scalar_t, scalar_t, scalar_t>::
                             named("gate"_s,
                                   "attack"_s,
                                   "decay"_s,
                                   "sustain"_s,
                                   "release"_s,
                                   "samplerate"_s));

        using osc_mixer_t  = mixer_t<fm_osc_t, fm_osc_t, fm_osc_t>;
        using osc_output_t = decltype(envelope_t{} * osc_mixer_t{});

        // Filter cutoff: base + filterEnv * amount
        using filter_env_freq_t = decltype(control_t{} + envelope_t{} * control_t{});

        template <typename T>
        using filter_t =
            decltype(processor<dap::dsp::LadderFilter<scalar_t>>::
                         with_inputs<T, filter_env_freq_t, control_t, samplerate_t>::named(
                             "signal"_s,
                             "frequency"_s,
                             "resonance"_s,
                             "samplerate"_s));

        template <typename T>
        using phaser_t = decltype(
            processor<dap::dsp::Phaser<scalar_t>>::
                with_inputs<T, control_t, control_t, control_t, control_t, samplerate_t>::named(
                    "signal"_s,
                    "frequency"_s,
                    "depth"_s,
                    "feedback"_s,
                    "wet"_s,
                    "samplerate"_s));

        using type  = phaser_t<filter_t<osc_output_t>>;
        using shape = shape_t;
    };

    using buffer_t = dap::fastmath::AudioBuffer<float>;

    buffer_t    m_output;
    Graph::type m_graph;

public:
    // Amplitude envelope — setGate also triggers the filter envelope
    class Envelope
    {
        Graph::envelope_t& m_ampEnv;
        Graph::envelope_t& m_filterEnv;

    public:
        Envelope(Graph::envelope_t& ampEnv, Graph::envelope_t& filterEnv)
        : m_ampEnv(ampEnv)
        , m_filterEnv(filterEnv)
        {
        }

        void setGate(float gate)
        {
            m_ampEnv.input("gate"_s)    = gate;
            m_filterEnv.input("gate"_s) = gate;
        }
        void setAttack(float t) { m_ampEnv.input("attack"_s) = t; }
        void setDecay(float t) { m_ampEnv.input("decay"_s) = t; }
        void setSustain(float s) { m_ampEnv.input("sustain"_s) = s; }
        void setRelease(float t) { m_ampEnv.input("release"_s) = t; }
        void setSampleRate(float sr) { m_ampEnv.input("samplerate"_s) = sr; }
    };

    // Filter envelope — controls cutoff: frequency = base + filterEnv * amount
    class FilterEnvelope
    {
        Graph::envelope_t& m_env;
        Graph::control_t&  m_base;
        Graph::control_t&  m_amount;

    public:
        FilterEnvelope(Graph::envelope_t& env,
                       Graph::control_t&  base,
                       Graph::control_t&  amount)
        : m_env(env)
        , m_base(base)
        , m_amount(amount)
        {
        }

        void setAttack(float t) { m_env.input("attack"_s) = t; }
        void setDecay(float t) { m_env.input("decay"_s) = t; }
        void setSustain(float s) { m_env.input("sustain"_s) = s; }
        void setRelease(float t) { m_env.input("release"_s) = t; }
        void setSampleRate(float sr) { m_env.input("samplerate"_s) = sr; }
        void setBase(float hz) { m_base.input("value"_s) = hz; }
        void setAmount(float hz) { m_amount.input("value"_s) = hz; }
    };

    class Operator
    {
        Graph::fm_osc_t& m_op;

    public:
        Operator(Graph::fm_osc_t& op)
        : m_op(op)
        {
        }

        auto& getFreq()
        {
            return m_op.input("frequency"_s).input("x"_s).input("x"_s).input("value"_s);
        }
        auto& getFreqPortamentoDuration()
        {
            return m_op.input("frequency"_s).input("x"_s).input("x"_s).input("duration"_s);
        }
        auto& getGainOp()
        {
            return m_op.input("gain"_s).input("y"_s);
        }
        auto& getVibratoOp()
        {
            return m_op.input("frequency"_s).input("x"_s).input("y"_s);
        }
        auto& getFreqOp()
        {
            return m_op.input("frequency"_s).input("y"_s);
        }

        void setFreq(float hz) { getFreq() = hz; }
        void setCarrierLevel(float level)
        {
            m_op.input("gain"_s).input("x"_s).input("value"_s) = level;
        }
        void setAmplitudeModRate(float hz)
        {
            getGainOp().input("frequency"_s).input("value"_s) = hz;
        }
        void setAmplitudeModDepth(float depth)
        {
            getGainOp().input("gain"_s).input("value"_s) = depth;
        }
        void setVibratoRate(float hz)
        {
            getVibratoOp().input("frequency"_s).input("value"_s) = hz;
        }
        void setVibratoDepth(float depth)
        {
            getVibratoOp().input("gain"_s).input("value"_s) = depth;
        }
        void setModFreq(float hz)
        {
            getFreqOp().input("frequency"_s).input("value"_s) = hz;
        }
        void setModIdx(float idx)
        {
            getFreqOp().input("gain"_s).input("value"_s) =
                getFreqOp().input("frequency"_s).input("value"_s) * idx;
        }
        void setSampleRate(float sr) { m_op.input("samplerate"_s) = sr; }
        void setShape(Graph::shape s) { m_op.input("shape"_s) = s; }
        void setPhase(float p) { m_op.input("phase"_s).input("value"_s) = p; }
    };

    class Filter
    {
        Graph::filter_t<Graph::osc_output_t>& m_filter;

    public:
        Filter(Graph::filter_t<Graph::osc_output_t>& filter)
        : m_filter(filter)
        {
        }

        void setResonance(float r)
        {
            m_filter.input("resonance"_s).input("value"_s) = r;
        }
        void setSampleRate(float sr)
        {
            m_filter.input("samplerate"_s) = sr;
        }
    };

    class PhaserControl
    {
        Graph::type& m_phaser;

    public:
        PhaserControl(Graph::type& phaser)
        : m_phaser(phaser)
        {
        }

        void setRate(float hz) { m_phaser.input("frequency"_s).input("value"_s) = hz; }
        void setDepth(float d) { m_phaser.input("depth"_s).input("value"_s) = d; }
        void setFeedback(float fb) { m_phaser.input("feedback"_s).input("value"_s) = fb; }
        void setWet(float w) { m_phaser.input("wet"_s).input("value"_s) = w; }
        void setSampleRate(float sr) { m_phaser.input("samplerate"_s) = sr; }
    };

    Synth(size_t bufferSize, float samplerate);

    template <size_t N>
    auto signal()
    {
        return Operator{m_graph.input("signal"_s)
                            .input("signal"_s)
                            .input("y"_s)
                            .input<N>()
                            .input("signal"_s)};
    }

    auto envelope()
    {
        // amplitude envelope (x side of multiply) and filter envelope (inside filter freq node)
        return Envelope{
            m_graph.input("signal"_s).input("signal"_s).input("x"_s),
            m_graph.input("signal"_s).input("frequency"_s).input("y"_s).input("x"_s)};
    }

    auto filterEnvelope()
    {
        return FilterEnvelope{
            m_graph.input("signal"_s).input("frequency"_s).input("y"_s).input("x"_s),
            m_graph.input("signal"_s).input("frequency"_s).input("x"_s),
            m_graph.input("signal"_s).input("frequency"_s).input("y"_s).input("y"_s)};
    }

    auto filter()
    {
        return Filter{m_graph.input("signal"_s)};
    }

    auto phaser()
    {
        return PhaserControl{m_graph};
    }

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
};
