#ifndef DAP_DSP_OSCILLATOR_H
#define DAP_DSP_OSCILLATOR_H

#include "Phasor.h"
#include "OscillatorFunctions.h"

namespace dap
{
    namespace dsp
    {
        template <typename T>
        class Oscillator;
    }
}

template <typename T>
class dap::dsp::Oscillator final
{
    Phasor<T> m_phasor;

public:
    inline auto operator()(T gain, T freq, T phase, T samplerate, OscillatorFunctions::Shape shape)
    {
        return gain*dsp::OscillatorFunctions::process(m_phasor(freq, samplerate) + phase, shape);
    }
};

#endif // DAP_DSP_OSCILLATOR_H
