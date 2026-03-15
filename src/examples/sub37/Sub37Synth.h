#pragma once
#include "base/KeyValueTuple.h"
#include "crtp/nodes/Processor.h"
#include "dsp/EnvelopeGenerator.h"
#include "dsp/LadderFilter.h"
#include "dsp/Mixer.h"
#include "dsp/NoiseGenerator.h"
#include "dsp/Oscillator.h"
#include "dsp/Smoother.h"
#include "dsp/UniformDistribution.h"
#include "fastmath/AudioBuffer.h"
#include <cstddef>

namespace sub37
{
    using dap::crtp::processor;
    using dap::operator""_s;
    using scalar_t     = float;
    using samplerate_t = scalar_t;
    class Synth;
}

class sub37::Synth final
{
    struct Graph
    {
        // Smoothed parameter control
        using control_t = decltype(processor<dap::dsp::FixedSmoother<scalar_t, 512>>::with_inputs<
                                   scalar_t>::named("value"_s));

        using shape_t = dap::dsp::OscillatorFunctions::Shape;

        // Portamento / glide
        using glide_t =
            decltype(processor<dap::dsp::Smoother<scalar_t>>::with_inputs<scalar_t, size_t>::named(
                "value"_s,
                "duration"_s));

        // ADSR envelope
        using envelope_t =
            decltype(processor<dap::dsp::EnvelopeGenerator<scalar_t>>::
                         with_inputs<scalar_t, scalar_t, scalar_t, scalar_t, scalar_t, scalar_t>::
                             named("gate"_s,
                                   "attack"_s,
                                   "decay"_s,
                                   "sustain"_s,
                                   "release"_s,
                                   "samplerate"_s));

        // LFO: oscillator with smoothed gain/freq/phase
        template <typename Amp, typename Freq, typename Ph>
        using osc_tmpl_t =
            decltype(processor<dap::dsp::Oscillator<scalar_t>>::
                         with_inputs<Amp, Freq, Ph, samplerate_t, shape_t>::named("gain"_s,
                                                                                  "frequency"_s,
                                                                                  "phase"_s,
                                                                                  "samplerate"_s,
                                                                                  "shape"_s));

        using lfo_t = osc_tmpl_t<control_t, control_t, control_t>;

        // Oscillator pitch: glide(note) + LFO(depth * waveform(rate))
        using osc_freq_t = decltype(glide_t{} + lfo_t{});

        // Main oscillator type (used for osc1, osc2, sub)
        using osc_t = osc_tmpl_t<control_t, osc_freq_t, control_t>;

        // Noise generator
        using noise_gen_t = dap::dsp::NoiseGenerator<dap::dsp::UniformDistribution>;
        using noise_t =
            decltype(processor<noise_gen_t>::with_inputs<control_t, noise_gen_t::Color>::named(
                "gain"_s,
                "color"_s));

        // Mixer: 4 buses (osc1, osc2, sub, noise)
        template <typename T>
        using bus_t =
            decltype(processor<dap::dsp::Mixer::Bus>::with_inputs<control_t, T>::named("gain"_s,
                                                                                       "signal"_s));

        template <typename... Ts>
        using mixer_t =
            decltype(processor<dap::dsp::Mixer>::with_inputs<bus_t<Ts>...>::prefixed_by("bus_"_s));

        using osc_mixer_t = mixer_t<osc_t, osc_t, osc_t, noise_t>;

        // Amp output: ampEnvelope * mixer
        using osc_output_t = decltype(envelope_t{} * osc_mixer_t{});

        // Filter cutoff: base + filterEnv * amount + filterLFO
        using filter_cutoff_t = decltype(control_t{} + envelope_t{} * control_t{} + lfo_t{});

        // Moog ladder filter
        template <typename T>
        using filter_t =
            decltype(processor<dap::dsp::LadderFilter<scalar_t>>::
                         with_inputs<T, filter_cutoff_t, control_t, samplerate_t>::named(
                             "signal"_s,
                             "frequency"_s,
                             "resonance"_s,
                             "samplerate"_s));

        // Final output type
        using type  = filter_t<osc_output_t>;
        using shape = shape_t;
    };

    using buffer_t = dap::fastmath::AudioBuffer<float>;

    buffer_t m_output;
    Graph::type m_graph;
    float m_osc2BeatFreq = 0.0f;

public:
    // --- Oscillator accessor ---
    class OscControl
    {
        Graph::osc_t& m_osc;
        Graph::control_t& m_busGain;

    public:
        OscControl(Graph::osc_t& osc, Graph::control_t& busGain)
        : m_osc(osc)
        , m_busGain(busGain)
        {
        }

        void setFreq(float hz)
        {
            m_osc.input("frequency"_s).input("x"_s).input("value"_s) = hz;
        }
        void setGlideDuration(size_t samples)
        {
            m_osc.input("frequency"_s).input("x"_s).input("duration"_s) = samples;
        }
        void setGain(float g)
        {
            m_osc.input("gain"_s).input("value"_s) = g;
        }
        void setBusLevel(float g)
        {
            m_busGain.input("value"_s) = g;
        }
        void setShape(Graph::shape s)
        {
            m_osc.input("shape"_s) = s;
        }
        void setSampleRate(float sr)
        {
            m_osc.input("samplerate"_s) = sr;
        }
        void setPhase(float p)
        {
            m_osc.input("phase"_s).input("value"_s) = p;
        }

        // Pitch LFO (mod 1) — each oscillator has its own tree copy
        void setPitchLfoRate(float hz)
        {
            m_osc.input("frequency"_s).input("y"_s).input("frequency"_s).input("value"_s) = hz;
        }
        void setPitchLfoDepth(float d)
        {
            m_osc.input("frequency"_s).input("y"_s).input("gain"_s).input("value"_s) = d;
        }
        void setPitchLfoShape(Graph::shape s)
        {
            m_osc.input("frequency"_s).input("y"_s).input("shape"_s) = s;
        }
        void setPitchLfoSampleRate(float sr)
        {
            m_osc.input("frequency"_s).input("y"_s).input("samplerate"_s) = sr;
        }
        void setPitchLfoPhase(float p)
        {
            m_osc.input("frequency"_s).input("y"_s).input("phase"_s).input("value"_s) = p;
        }
    };

    // --- Amp envelope (also triggers filter envelope) ---
    class AmpEnvelope
    {
        Graph::envelope_t& m_ampEnv;
        Graph::envelope_t& m_filterEnv;

    public:
        AmpEnvelope(Graph::envelope_t& ampEnv, Graph::envelope_t& filterEnv)
        : m_ampEnv(ampEnv)
        , m_filterEnv(filterEnv)
        {
        }

        void setGate(float g)
        {
            m_ampEnv.input("gate"_s)    = g;
            m_filterEnv.input("gate"_s) = g;
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

    // --- Filter envelope: controls cutoff = base + env * amount ---
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

    // --- Filter (resonance + LFO mod) ---
    class Filter
    {
        Graph::type& m_graph;

    public:
        Filter(Graph::type& graph)
        : m_graph(graph)
        {
        }

        void setResonance(float r)
        {
            m_graph.input("resonance"_s).input("value"_s) = r;
        }
        void setSampleRate(float sr)
        {
            m_graph.input("samplerate"_s) = sr;
        }

        // Filter LFO (mod 2)
        void setLfoRate(float hz)
        {
            m_graph.input("frequency"_s).input("y"_s).input("frequency"_s).input("value"_s) = hz;
        }
        void setLfoDepth(float d)
        {
            m_graph.input("frequency"_s).input("y"_s).input("gain"_s).input("value"_s) = d;
        }
        void setLfoShape(Graph::shape s)
        {
            m_graph.input("frequency"_s).input("y"_s).input("shape"_s) = s;
        }
        void setLfoSampleRate(float sr)
        {
            m_graph.input("frequency"_s).input("y"_s).input("samplerate"_s) = sr;
        }
        void setLfoPhase(float p)
        {
            m_graph.input("frequency"_s).input("y"_s).input("phase"_s).input("value"_s) = p;
        }
    };

    // --- Noise accessor ---
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

    Synth(size_t bufferSize, float samplerate);

    // Oscillator accessors (bus 0=osc1, 1=osc2, 2=sub)
    template <size_t N>
    auto oscControl()
    {
        auto& mixer = m_graph.input("signal"_s).input("y"_s);
        return OscControl{mixer.template input<N>().input("signal"_s),
                          mixer.template input<N>().input("gain"_s)};
    }

    auto osc1()
    {
        return oscControl<0>();
    }
    auto osc2()
    {
        return oscControl<1>();
    }
    auto subOsc()
    {
        return oscControl<2>();
    }

    template <size_t N>
    void setBusGain(float gain)
    {
        m_graph.input("signal"_s)
            .input("y"_s)
            .template input<N>()
            .input("gain"_s)
            .input("value"_s) = gain;
    }

    auto envelope()
    {
        return AmpEnvelope{m_graph.input("signal"_s).input("x"_s),
                           m_graph.input("frequency"_s).input("x"_s).input("y"_s).input("x"_s)};
    }

    auto filterEnvelope()
    {
        return FilterEnvelope{m_graph.input("frequency"_s).input("x"_s).input("y"_s).input("x"_s),
                              m_graph.input("frequency"_s).input("x"_s).input("x"_s),
                              m_graph.input("frequency"_s).input("x"_s).input("y"_s).input("y"_s)};
    }

    auto filter()
    {
        return Filter{m_graph};
    }

    auto noise()
    {
        return Noise{m_graph.input("signal"_s).input("y"_s).template input<3>().input("signal"_s)};
    }

    // Osc2 beat frequency (detune in Hz relative to osc1)
    void setOsc2BeatFreq(float hz)
    {
        m_osc2BeatFreq = hz;
    }
    float osc2BeatFreq() const
    {
        return m_osc2BeatFreq;
    }

    // Set pitch for all oscillators (osc2 gets beat freq offset, sub gets octave below)
    void setNote(float freq)
    {
        osc1().setFreq(freq);
        osc2().setFreq(freq + m_osc2BeatFreq);
        subOsc().setFreq(freq * 0.5f);
    }

    // Set glide duration on all oscillators
    void setGlide(size_t samples)
    {
        osc1().setGlideDuration(samples);
        osc2().setGlideDuration(samples);
        subOsc().setGlideDuration(samples);
    }

    // Set pitch LFO 1 params on osc1 and sub
    void setPitchLfo(float rate, float depth, Graph::shape shape = Graph::shape::Triangle)
    {
        auto setLfo = [&](auto osc)
        {
            osc.setPitchLfoRate(rate);
            osc.setPitchLfoDepth(depth);
            osc.setPitchLfoShape(shape);
        };
        setLfo(osc1());
        setLfo(subOsc());
    }

    // Set pitch LFO 2 params on osc2 only
    void setPitchLfo2(float rate, float depth, Graph::shape shape = Graph::shape::Triangle)
    {
        osc2().setPitchLfoRate(rate);
        osc2().setPitchLfoDepth(depth);
        osc2().setPitchLfoShape(shape);
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
