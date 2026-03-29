#pragma once
#include "base/KeyValueTuple.h"
#include "crtp/nodes/Processor.h"
#include "dsp/CombFilter.h"
#include "dsp/EnvelopeGenerator.h"
#include "dsp/LadderFilter.h"
#include "dsp/Mixer.h"
#include "dsp/NoiseGenerator.h"
#include "dsp/Oscillator.h"
#include "dsp/Pwm.h"
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

        // Pitch envelope: env * amount (in Hz)
        using pitch_env_t = decltype(envelope_t{} * control_t{});

        // Oscillator pitch: glide(note) + LFO + pitchEnv * amount
        using osc_freq_t = decltype(glide_t{} + lfo_t{} + pitch_env_t{});

        // Main oscillator type (used for osc1, osc2, sub)
        using osc_t = osc_tmpl_t<control_t, osc_freq_t, control_t>;

        // Noise generator
        using noise_gen_t = dap::dsp::NoiseGenerator<dap::dsp::UniformDistribution>;
        using noise_t =
            decltype(processor<noise_gen_t>::with_inputs<control_t, noise_gen_t::Color>::named(
                "gain"_s,
                "color"_s));

        // PWM duty cycle: base + LFO
        using pwm_dutycycle_t = decltype(control_t{} + lfo_t{});

        // PWM oscillator: gain, freq (with glide+LFO), phase, samplerate, dutyCycle(base+LFO)
        using pwm_t =
            decltype(processor<dap::dsp::Pwm<scalar_t>>::with_inputs<control_t,
                                                                     osc_freq_t,
                                                                     control_t,
                                                                     samplerate_t,
                                                                     pwm_dutycycle_t>::
                         named("gain"_s, "frequency"_s, "phase"_s, "samplerate"_s, "dutycycle"_s));

        // Mixer: 5 buses (osc1, osc2, sub, noise, pwm)
        template <typename T>
        using bus_t =
            decltype(processor<dap::dsp::Mixer::Bus>::with_inputs<control_t, T>::named("gain"_s,
                                                                                       "signal"_s));

        template <typename... Ts>
        using mixer_t =
            decltype(processor<dap::dsp::Mixer>::with_inputs<bus_t<Ts>...>::prefixed_by("bus_"_s));

        using osc_mixer_t = mixer_t<osc_t, osc_t, osc_t, noise_t, pwm_t>;

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

        // Feedback comb filter (65536 samples ≈ 1.5s at 44.1kHz)
        template <typename T>
        using feedback_t = decltype(processor<dap::dsp::FeedbackCombFilter<scalar_t, 65536>>::
                                        with_inputs<T, control_t, control_t>::named("input"_s,
                                                                                    "delay"_s,
                                                                                    "feedback"_s));

        // Final output type
        using type  = feedback_t<filter_t<osc_output_t>>;
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
            m_osc.input("frequency"_s).input("x"_s).input("x"_s).input("value"_s) = hz;
        }
        void setGlideDuration(size_t samples)
        {
            m_osc.input("frequency"_s).input("x"_s).input("x"_s).input("duration"_s) = samples;
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
            m_osc.input("frequency"_s)
                .input("x"_s)
                .input("y"_s)
                .input("frequency"_s)
                .input("value"_s) = hz;
        }
        void setPitchLfoDepth(float d)
        {
            m_osc.input("frequency"_s).input("x"_s).input("y"_s).input("gain"_s).input("value"_s) =
                d;
        }
        void setPitchLfoShape(Graph::shape s)
        {
            m_osc.input("frequency"_s).input("x"_s).input("y"_s).input("shape"_s) = s;
        }
        void setPitchLfoSampleRate(float sr)
        {
            m_osc.input("frequency"_s).input("x"_s).input("y"_s).input("samplerate"_s) = sr;
        }
        void setPitchLfoPhase(float p)
        {
            m_osc.input("frequency"_s).input("x"_s).input("y"_s).input("phase"_s).input("value"_s) =
                p;
        }
    };

    // --- Amp envelope (also triggers filter + pitch envelopes) ---
    class AmpEnvelope
    {
        Graph::envelope_t& m_ampEnv;
        Graph::envelope_t& m_filterEnv;
        Graph::envelope_t& m_pitchEnv1;
        Graph::envelope_t& m_pitchEnv2;

    public:
        AmpEnvelope(Graph::envelope_t& ampEnv,
                    Graph::envelope_t& filterEnv,
                    Graph::envelope_t& pitchEnv1,
                    Graph::envelope_t& pitchEnv2)
        : m_ampEnv(ampEnv)
        , m_filterEnv(filterEnv)
        , m_pitchEnv1(pitchEnv1)
        , m_pitchEnv2(pitchEnv2)
        {
        }

        void setGate(float g)
        {
            m_ampEnv.input("gate"_s)    = g;
            m_filterEnv.input("gate"_s) = g;
            m_pitchEnv1.input("gate"_s) = g;
            m_pitchEnv2.input("gate"_s) = g;
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

    // --- Pitch envelope: controls osc1 + osc2 pitch = base + env * amount ---
    class PitchEnvelope
    {
        Graph::envelope_t& m_env1;
        Graph::control_t& m_amount1;
        Graph::envelope_t& m_env2;
        Graph::control_t& m_amount2;

    public:
        PitchEnvelope(Graph::envelope_t& env1,
                      Graph::control_t& amount1,
                      Graph::envelope_t& env2,
                      Graph::control_t& amount2)
        : m_env1(env1)
        , m_amount1(amount1)
        , m_env2(env2)
        , m_amount2(amount2)
        {
        }

        void setAttack(float t)
        {
            m_env1.input("attack"_s) = t;
            m_env2.input("attack"_s) = t;
        }
        void setDecay(float t)
        {
            m_env1.input("decay"_s) = t;
            m_env2.input("decay"_s) = t;
        }
        void setSustain(float s)
        {
            m_env1.input("sustain"_s) = s;
            m_env2.input("sustain"_s) = s;
        }
        void setRelease(float t)
        {
            m_env1.input("release"_s) = t;
            m_env2.input("release"_s) = t;
        }
        void setSampleRate(float sr)
        {
            m_env1.input("samplerate"_s) = sr;
            m_env2.input("samplerate"_s) = sr;
        }
        void setAmount(float hz)
        {
            m_amount1.input("value"_s) = hz;
            m_amount2.input("value"_s) = hz;
        }
    };

    // --- Filter (resonance + LFO mod) ---
    using filter_node_t = Graph::filter_t<Graph::osc_output_t>;

    class Filter
    {
        filter_node_t& m_filter;

    public:
        Filter(filter_node_t& filter)
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

        // Filter LFO (mod 2)
        void setLfoRate(float hz)
        {
            m_filter.input("frequency"_s).input("y"_s).input("frequency"_s).input("value"_s) = hz;
        }
        void setLfoDepth(float d)
        {
            m_filter.input("frequency"_s).input("y"_s).input("gain"_s).input("value"_s) = d;
        }
        void setLfoShape(Graph::shape s)
        {
            m_filter.input("frequency"_s).input("y"_s).input("shape"_s) = s;
        }
        void setLfoSampleRate(float sr)
        {
            m_filter.input("frequency"_s).input("y"_s).input("samplerate"_s) = sr;
        }
        void setLfoPhase(float p)
        {
            m_filter.input("frequency"_s).input("y"_s).input("phase"_s).input("value"_s) = p;
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

    // --- PWM accessor ---
    class PwmControl
    {
        Graph::pwm_t& m_pwm;
        Graph::control_t& m_busGain;

    public:
        PwmControl(Graph::pwm_t& pwm, Graph::control_t& busGain)
        : m_pwm(pwm)
        , m_busGain(busGain)
        {
        }

        void setFreq(float hz)
        {
            m_pwm.input("frequency"_s).input("x"_s).input("x"_s).input("value"_s) = hz;
        }
        void setGlideDuration(size_t samples)
        {
            m_pwm.input("frequency"_s).input("x"_s).input("x"_s).input("duration"_s) = samples;
        }
        void setGain(float g)
        {
            m_pwm.input("gain"_s).input("value"_s) = g;
        }
        void setBusLevel(float g)
        {
            m_busGain.input("value"_s) = g;
        }
        void setSampleRate(float sr)
        {
            m_pwm.input("samplerate"_s) = sr;
        }
        void setPhase(float p)
        {
            m_pwm.input("phase"_s).input("value"_s) = p;
        }
        void setDutyCycle(float dc)
        {
            m_pwm.input("dutycycle"_s).input("x"_s).input("value"_s) = dc;
        }

        // Duty cycle LFO
        void setDcLfoRate(float hz)
        {
            m_pwm.input("dutycycle"_s).input("y"_s).input("frequency"_s).input("value"_s) = hz;
        }
        void setDcLfoDepth(float d)
        {
            m_pwm.input("dutycycle"_s).input("y"_s).input("gain"_s).input("value"_s) = d;
        }
        void setDcLfoShape(Graph::shape s)
        {
            m_pwm.input("dutycycle"_s).input("y"_s).input("shape"_s) = s;
        }
        void setDcLfoSampleRate(float sr)
        {
            m_pwm.input("dutycycle"_s).input("y"_s).input("samplerate"_s) = sr;
        }
        void setDcLfoPhase(float p)
        {
            m_pwm.input("dutycycle"_s).input("y"_s).input("phase"_s).input("value"_s) = p;
        }

        // Pitch LFO
        void setPitchLfoRate(float hz)
        {
            m_pwm.input("frequency"_s)
                .input("x"_s)
                .input("y"_s)
                .input("frequency"_s)
                .input("value"_s) = hz;
        }
        void setPitchLfoDepth(float d)
        {
            m_pwm.input("frequency"_s).input("x"_s).input("y"_s).input("gain"_s).input("value"_s) =
                d;
        }
        void setPitchLfoShape(Graph::shape s)
        {
            m_pwm.input("frequency"_s).input("x"_s).input("y"_s).input("shape"_s) = s;
        }
        void setPitchLfoSampleRate(float sr)
        {
            m_pwm.input("frequency"_s).input("x"_s).input("y"_s).input("samplerate"_s) = sr;
        }
        void setPitchLfoPhase(float p)
        {
            m_pwm.input("frequency"_s).input("x"_s).input("y"_s).input("phase"_s).input("value"_s) =
                p;
        }
    };

    Synth(size_t bufferSize, float samplerate);

    // Oscillator accessors (bus 0=osc1, 1=osc2, 2=sub)
    auto& filterNode()
    {
        return m_graph.input("input"_s);
    }

    template <size_t N>
    auto oscControl()
    {
        auto& mixer = filterNode().input("signal"_s).input("y"_s);
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
        filterNode()
            .input("signal"_s)
            .input("y"_s)
            .template input<N>()
            .input("gain"_s)
            .input("value"_s) = gain;
    }

    auto envelope()
    {
        auto& mixer = filterNode().input("signal"_s).input("y"_s);
        return AmpEnvelope{filterNode().input("signal"_s).input("x"_s),
                           filterNode().input("frequency"_s).input("x"_s).input("y"_s).input("x"_s),
                           mixer.template input<0>()
                               .input("signal"_s)
                               .input("frequency"_s)
                               .input("y"_s)
                               .input("x"_s),
                           mixer.template input<1>()
                               .input("signal"_s)
                               .input("frequency"_s)
                               .input("y"_s)
                               .input("x"_s)};
    }

    auto filterEnvelope()
    {
        return FilterEnvelope{
            filterNode().input("frequency"_s).input("x"_s).input("y"_s).input("x"_s),
            filterNode().input("frequency"_s).input("x"_s).input("x"_s),
            filterNode().input("frequency"_s).input("x"_s).input("y"_s).input("y"_s)};
    }

    auto pitchEnvelope()
    {
        auto& mixer = filterNode().input("signal"_s).input("y"_s);
        return PitchEnvelope{mixer.template input<0>()
                                 .input("signal"_s)
                                 .input("frequency"_s)
                                 .input("y"_s)
                                 .input("x"_s),
                             mixer.template input<0>()
                                 .input("signal"_s)
                                 .input("frequency"_s)
                                 .input("y"_s)
                                 .input("y"_s),
                             mixer.template input<1>()
                                 .input("signal"_s)
                                 .input("frequency"_s)
                                 .input("y"_s)
                                 .input("x"_s),
                             mixer.template input<1>()
                                 .input("signal"_s)
                                 .input("frequency"_s)
                                 .input("y"_s)
                                 .input("y"_s)};
    }

    auto filter()
    {
        return Filter{filterNode()};
    }

    auto noise()
    {
        return Noise{
            filterNode().input("signal"_s).input("y"_s).template input<3>().input("signal"_s)};
    }

    auto pwm()
    {
        auto& mixer = filterNode().input("signal"_s).input("y"_s);
        return PwmControl{mixer.template input<4>().input("signal"_s),
                          mixer.template input<4>().input("gain"_s)};
    }

    // Feedback delay line controls
    void setDelayTime(float samples)
    {
        m_graph.input("delay"_s).input("value"_s) = samples;
    }
    void setFeedback(float fb)
    {
        m_graph.input("feedback"_s).input("value"_s) = fb;
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
        pwm().setFreq(freq);
    }

    // Set glide duration on all oscillators
    void setGlide(size_t samples)
    {
        osc1().setGlideDuration(samples);
        osc2().setGlideDuration(samples);
        subOsc().setGlideDuration(samples);
        pwm().setGlideDuration(samples);
    }

    // Set pitch LFO 1 params on osc1, sub, pwm1, pwm2
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
        setLfo(pwm());
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
