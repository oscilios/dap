#ifndef DAP_DSP_PWM_H
#define DAP_DSP_PWM_H

#include "Phasor.h"
#include "PwmFunctions.h"

namespace dap
{
    namespace dsp
    {
        template <typename T>
        class Pwm;
    }
}

template <typename T>
class dap::dsp::Pwm final
{
    Phasor<T> m_phasor;

public:
    inline auto operator()(T gain, T freq, T phase, T samplerate, T dutyCycle)
    {
        return gain * dsp::PwmFunctions::process(m_phasor(freq, samplerate) + phase, dutyCycle);
    }
};

#endif // DAP_DSP_PWM_H
