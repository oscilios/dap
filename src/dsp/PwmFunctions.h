#ifndef DAP_DSP_PWM_FUNCTIONS_H
#define DAP_DSP_PWM_FUNCTIONS_H

#include "base/Constants.h"

namespace dap
{
    namespace dsp
    {
        class PwmFunctions;
    }
}

class dap::dsp::PwmFunctions final
{
public:
    template <typename T>
    static auto process(T phase, T dutyCycle)
    {
        auto square = [](T&& ph, T&& tau) { return ph < tau ? 1.0f : 0.0f; };
        return square(std::move(phase), std::move(TWO_PI * clip(T(0), T(1), dutyCycle)));
    }
};

#endif // DAP_DSP_PWM_FUNCTIONS_H
