#ifndef DAP_DSP_FLANGER_H
#define DAP_DSP_FLANGER_H

#include "DelayLine.h"
#include "OscillatorFunctions.h"
#include "Phasor.h"

namespace dap
{
    namespace dsp
    {
        template <typename T, size_t N = 4096>
        class Flanger;
    }
}

template <typename T, size_t N>
class dap::dsp::Flanger final
{
    DelayLine<T, N> m_delay;
    Phasor<T> m_phasor;
    T m_feedback_state{0};

public:
    // delay:     base delay in samples (e.g. 1–10 ms worth)
    // depth:     LFO sweep depth in samples
    // rate:      LFO rate in Hz
    // feedback:  feedback amount, typically -0.95 .. 0.95
    // wet:       dry/wet mix, 0 = dry, 1 = full wet
    inline auto operator()(T input, T delay, T depth, T rate, T feedback, T wet, T samplerate)
    {
        const auto lfo = OscillatorFunctions::process(m_phasor(rate, samplerate),
                                                      OscillatorFunctions::SineTag{});

        const auto mod_delay      = delay + depth * lfo;
        const auto clamped_delay  = std::max(T(1), std::min(mod_delay, T(N - 1)));
        const auto delayed_signal = m_delay(input + feedback * m_feedback_state, clamped_delay);
        m_feedback_state          = delayed_signal;

        return (T(1) - wet) * input + wet * delayed_signal;
    }
};

#endif // DAP_DSP_FLANGER_H
