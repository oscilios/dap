#pragma once
#include "base/KeyValueTuple.h"
#include "crtp/nodes/Processor.h"
#include "dsp/BiquadFilter.h"
#include "dsp/EnvelopeGenerator.h"
#include "dsp/LadderFilter.h"
#include "dsp/Mixer.h"
#include "dsp/NoiseGenerator.h"
#include "dsp/Oscillator.h"
#include "dsp/Phaser.h"
#include "dsp/Smoother.h"
#include "dsp/UniformDistribution.h"
#include "fastmath/AudioBuffer.h"
#include <array>
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
        using portamento_mod_control_t = decltype(portamento_t{} + control_osc_t{});

        // friendly aliases
        using vibrato_mod_t = portamento_mod_control_t;

        using envelope_t =
            decltype(processor<dap::dsp::EnvelopeGenerator<scalar_t>>::
                         with_inputs<scalar_t, scalar_t, scalar_t, scalar_t, scalar_t, scalar_t>::
                             named("gate"_s,
                                   "attack"_s,
                                   "decay"_s,
                                   "sustain"_s,
                                   "release"_s,
                                   "samplerate"_s));

        // Envelope-controlled gain: base + env * amount (used for FM and AM)
        using env_gain_t = decltype(control_t{} + envelope_t{} * control_t{});

        // AM oscillator with envelope-controlled depth
        using am_osc_t  = osc_t<env_gain_t, control_t, control_t>;
        using amp_mod_t = decltype(control_t{} + am_osc_t{});

        // FM modulator with envelope-controlled gain
        using fm_mod_osc_t = osc_t<env_gain_t, control_t, control_t>;
        using fm_vib_mod_t = decltype(vibrato_mod_t{} + fm_mod_osc_t{});

        // fm oscillator
        using fm_osc_t = osc_t<amp_mod_t, fm_vib_mod_t, control_t>;

        template <typename T>
        using bus_t =
            decltype(processor<dap::dsp::Mixer::Bus>::with_inputs<control_t, T>::named("gain"_s,
                                                                                       "signal"_s));

        template <typename... Ts>
        using mixer_t =
            decltype(processor<dap::dsp::Mixer>::with_inputs<bus_t<Ts>...>::prefixed_by("bus_"_s));

        // Noise generator as 6th mixer bus
        using noise_gen_t = dap::dsp::NoiseGenerator<dap::dsp::UniformDistribution>;
        using noise_t =
            decltype(processor<noise_gen_t>::with_inputs<control_t, noise_gen_t::Color>::named(
                "gain"_s,
                "color"_s));

        using osc_mixer_t  = mixer_t<fm_osc_t, fm_osc_t, fm_osc_t, fm_osc_t, fm_osc_t, noise_t>;
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
        using phaser_t =
            decltype(processor<dap::dsp::Phaser<scalar_t>>::
                         with_inputs<T, control_t, control_t, control_t, control_t, samplerate_t>::
                             named("signal"_s,
                                   "frequency"_s,
                                   "depth"_s,
                                   "feedback"_s,
                                   "wet"_s,
                                   "samplerate"_s));

        using biquad_filter_t = dap::dsp::BiquadFilter<scalar_t>;

        template <typename T>
        using biquad_t =
            decltype(processor<biquad_filter_t>::with_inputs<T,
                                                             control_t,
                                                             control_t,
                                                             biquad_filter_t::Type,
                                                             control_t,
                                                             samplerate_t>::named("signal"_s,
                                                                                  "frequency"_s,
                                                                                  "Q"_s,
                                                                                  "type"_s,
                                                                                  "gainDb"_s,
                                                                                  "samplerate"_s));

        using phaser_output_t = phaser_t<filter_t<osc_output_t>>;
        using type            = biquad_t<phaser_output_t>;
        using shape           = shape_t;
    };

    using buffer_t = dap::fastmath::AudioBuffer<float>;

    buffer_t m_output;
    Graph::type m_graph;

public:
    // Amplitude envelope — setGate also triggers the filter, FM, and AM envelopes
    class Envelope
    {
        Graph::envelope_t& m_ampEnv;
        Graph::envelope_t& m_filterEnv;
        std::array<Graph::envelope_t*, 10> m_attackEnvs; // 5 FM + 5 AM

    public:
        Envelope(Graph::envelope_t& ampEnv,
                 Graph::envelope_t& filterEnv,
                 std::array<Graph::envelope_t*, 10> attackEnvs)
        : m_ampEnv(ampEnv)
        , m_filterEnv(filterEnv)
        , m_attackEnvs(attackEnvs)
        {
        }

        void setGate(float gate)
        {
            m_ampEnv.input("gate"_s)    = gate;
            m_filterEnv.input("gate"_s) = gate;
            for (auto* env : m_attackEnvs)
                env->input("gate"_s) = gate;
        }
        void setAttack(float t)
        {
            m_ampEnv.input("attack"_s) = t;
        }
        void setDecay(float t)
        {
            m_ampEnv.input("decay"_s) = t;
        }
        void setSustain(float s)
        {
            m_ampEnv.input("sustain"_s) = s;
        }
        void setRelease(float t)
        {
            m_ampEnv.input("release"_s) = t;
        }
        void setSampleRate(float sr)
        {
            m_ampEnv.input("samplerate"_s) = sr;
        }
    };

    // Filter envelope — controls cutoff: frequency = base + filterEnv * amount
    class FilterEnvelope
    {
        Graph::envelope_t& m_env;
        Graph::control_t& m_base;
        Graph::control_t& m_amount;

    public:
        FilterEnvelope(Graph::envelope_t& env, Graph::control_t& base, Graph::control_t& amount)
        : m_env(env)
        , m_base(base)
        , m_amount(amount)
        {
        }

        void setAttack(float t)
        {
            m_env.input("attack"_s) = t;
        }
        void setDecay(float t)
        {
            m_env.input("decay"_s) = t;
        }
        void setSustain(float s)
        {
            m_env.input("sustain"_s) = s;
        }
        void setRelease(float t)
        {
            m_env.input("release"_s) = t;
        }
        void setSampleRate(float sr)
        {
            m_env.input("samplerate"_s) = sr;
        }
        void setBase(float hz)
        {
            m_base.input("value"_s) = hz;
        }
        void setAmount(float hz)
        {
            m_amount.input("value"_s) = hz;
        }
    };

    // Attack envelope — shared ADSR for FM and AM burst at note onset
    class AttackEnvelope
    {
        std::array<Graph::envelope_t*, 10> m_envs; // 5 FM + 5 AM

    public:
        AttackEnvelope(std::array<Graph::envelope_t*, 10> envs)
        : m_envs(envs)
        {
        }

        void setAttack(float t)
        {
            for (auto* e : m_envs)
                e->input("attack"_s) = t;
        }
        void setDecay(float t)
        {
            for (auto* e : m_envs)
                e->input("decay"_s) = t;
        }
        void setSustain(float s)
        {
            for (auto* e : m_envs)
                e->input("sustain"_s) = s;
        }
        void setRelease(float t)
        {
            for (auto* e : m_envs)
                e->input("release"_s) = t;
        }
        void setSampleRate(float sr)
        {
            for (auto* e : m_envs)
                e->input("samplerate"_s) = sr;
        }
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

        void setFreq(float hz)
        {
            getFreq() = hz;
        }
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
            getGainOp().input("gain"_s).input("x"_s).input("value"_s) = depth;
        }
        void setAMEnvAmount(float amount)
        {
            getGainOp().input("gain"_s).input("y"_s).input("y"_s).input("value"_s) = amount;
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
            getFreqOp().input("gain"_s).input("x"_s).input("value"_s) =
                getFreqOp().input("frequency"_s).input("value"_s) * idx;
        }
        void setFMEnvAmount(float amount)
        {
            getFreqOp().input("gain"_s).input("y"_s).input("y"_s).input("value"_s) = amount;
        }
        void setSampleRate(float sr)
        {
            m_op.input("samplerate"_s) = sr;
        }
        void setShape(Graph::shape s)
        {
            m_op.input("shape"_s) = s;
        }
        void setPhase(float p)
        {
            m_op.input("phase"_s).input("value"_s) = p;
        }
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
        Graph::phaser_output_t& m_phaser;

    public:
        PhaserControl(Graph::phaser_output_t& phaser)
        : m_phaser(phaser)
        {
        }

        void setRate(float hz)
        {
            m_phaser.input("frequency"_s).input("value"_s) = hz;
        }
        void setDepth(float d)
        {
            m_phaser.input("depth"_s).input("value"_s) = d;
        }
        void setFeedback(float fb)
        {
            m_phaser.input("feedback"_s).input("value"_s) = fb;
        }
        void setWet(float w)
        {
            m_phaser.input("wet"_s).input("value"_s) = w;
        }
        void setSampleRate(float sr)
        {
            m_phaser.input("samplerate"_s) = sr;
        }
    };

    Synth(size_t bufferSize, float samplerate);

    template <size_t N>
    auto signal()
    {
        return Operator{m_graph.input("signal"_s)
                            .input("signal"_s)
                            .input("signal"_s)
                            .input("y"_s)
                            .input<N>()
                            .input("signal"_s)};
    }

    template <size_t N>
    void setBusGain(float gain)
    {
        m_graph.input("signal"_s)
            .input("signal"_s)
            .input("signal"_s)
            .input("y"_s)
            .input<N>()
            .input("gain"_s)
            .input("value"_s) = gain;
    }

    auto attackEnvPtrs()
    {
        auto& mixer = m_graph.input("signal"_s).input("signal"_s).input("signal"_s).input("y"_s);
        // FM envelopes (5) + AM envelopes (5)
        return std::array<Graph::envelope_t*, 10>{
            // FM: operator.frequency.y (fm_mod_osc).gain.y.x (envelope)
            &mixer.input<0>()
                 .input("signal"_s)
                 .input("frequency"_s)
                 .input("y"_s)
                 .input("gain"_s)
                 .input("y"_s)
                 .input("x"_s),
            &mixer.input<1>()
                 .input("signal"_s)
                 .input("frequency"_s)
                 .input("y"_s)
                 .input("gain"_s)
                 .input("y"_s)
                 .input("x"_s),
            &mixer.input<2>()
                 .input("signal"_s)
                 .input("frequency"_s)
                 .input("y"_s)
                 .input("gain"_s)
                 .input("y"_s)
                 .input("x"_s),
            &mixer.input<3>()
                 .input("signal"_s)
                 .input("frequency"_s)
                 .input("y"_s)
                 .input("gain"_s)
                 .input("y"_s)
                 .input("x"_s),
            &mixer.input<4>()
                 .input("signal"_s)
                 .input("frequency"_s)
                 .input("y"_s)
                 .input("gain"_s)
                 .input("y"_s)
                 .input("x"_s),
            // AM: operator.gain.y (am_osc).gain.y.x (envelope)
            &mixer.input<0>()
                 .input("signal"_s)
                 .input("gain"_s)
                 .input("y"_s)
                 .input("gain"_s)
                 .input("y"_s)
                 .input("x"_s),
            &mixer.input<1>()
                 .input("signal"_s)
                 .input("gain"_s)
                 .input("y"_s)
                 .input("gain"_s)
                 .input("y"_s)
                 .input("x"_s),
            &mixer.input<2>()
                 .input("signal"_s)
                 .input("gain"_s)
                 .input("y"_s)
                 .input("gain"_s)
                 .input("y"_s)
                 .input("x"_s),
            &mixer.input<3>()
                 .input("signal"_s)
                 .input("gain"_s)
                 .input("y"_s)
                 .input("gain"_s)
                 .input("y"_s)
                 .input("x"_s),
            &mixer.input<4>()
                 .input("signal"_s)
                 .input("gain"_s)
                 .input("y"_s)
                 .input("gain"_s)
                 .input("y"_s)
                 .input("x"_s)};
    }

    auto envelope()
    {
        // amplitude envelope, filter envelope, and attack envelopes (FM + AM)
        return Envelope{m_graph.input("signal"_s).input("signal"_s).input("signal"_s).input("x"_s),
                        m_graph.input("signal"_s)
                            .input("signal"_s)
                            .input("frequency"_s)
                            .input("y"_s)
                            .input("x"_s),
                        attackEnvPtrs()};
    }

    auto attackEnvelope()
    {
        return AttackEnvelope{attackEnvPtrs()};
    }

    auto filterEnvelope()
    {
        return FilterEnvelope{
            m_graph.input("signal"_s)
                .input("signal"_s)
                .input("frequency"_s)
                .input("y"_s)
                .input("x"_s),
            m_graph.input("signal"_s).input("signal"_s).input("frequency"_s).input("x"_s),
            m_graph.input("signal"_s)
                .input("signal"_s)
                .input("frequency"_s)
                .input("y"_s)
                .input("y"_s)};
    }

    auto filter()
    {
        return Filter{m_graph.input("signal"_s).input("signal"_s)};
    }

    auto phaser()
    {
        return PhaserControl{m_graph.input("signal"_s)};
    }

    class Noise
    {
        Graph::noise_t& m_noise;

    public:
        Noise(Graph::noise_t& noise)
        : m_noise(noise)
        {
        }

        void setGain(float g)
        {
            m_noise.input("gain"_s).input("value"_s) = g;
        }
        void setColor(Graph::noise_gen_t::Color c)
        {
            m_noise.input("color"_s) = c;
        }
    };

    auto noise()
    {
        return Noise{m_graph.input("signal"_s)
                         .input("signal"_s)
                         .input("signal"_s)
                         .input("y"_s)
                         .input<5>()
                         .input("signal"_s)};
    }

    class EQ
    {
        Graph::type& m_eq;

    public:
        EQ(Graph::type& eq)
        : m_eq(eq)
        {
        }

        void setFrequency(float hz)
        {
            m_eq.input("frequency"_s).input("value"_s) = hz;
        }
        void setQ(float q)
        {
            m_eq.input("Q"_s).input("value"_s) = q;
        }
        void setGainDb(float db)
        {
            m_eq.input("gainDb"_s).input("value"_s) = db;
        }
        void setType(Graph::biquad_filter_t::Type t)
        {
            m_eq.input("type"_s) = t;
        }
        void setSampleRate(float sr)
        {
            m_eq.input("samplerate"_s) = sr;
        }
    };

    auto eq()
    {
        return EQ{m_graph};
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
