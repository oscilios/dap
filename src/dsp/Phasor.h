#ifndef DAP_DSP_PHASOR_H
#define DAP_DSP_PHASOR_H

#include "base/Constants.h"
#include "base/TypeTraits.h"

namespace dap
{
    namespace dsp
    {
        template <typename T>
        class Phasor;
    }
}

template <typename T>
class dap::dsp::Phasor final
{
    T m_phase{0};

    static_assert(dap::isFloatingPoint<T>(), "T must be floating point.");

public:
    template <typename T1, typename T2>
    inline auto operator()(T1 freq, T2 sampleRate)
    {
        auto wrap = [](T&& x) { return x - std::floor(x / TWO_PI) * TWO_PI; };
        m_phase   = wrap(std::move(m_phase + TWO_PI * freq / sampleRate));
        return m_phase;
    }
};

#endif // DAP_DSP_PHASOR_H
