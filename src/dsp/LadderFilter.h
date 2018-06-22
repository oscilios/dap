#ifndef DAP_DSP_LADDER_FILTER_H
#define DAP_DSP_LADDER_FILTER_H

#include <cmath>
#include "base/TypeTraits.h"

namespace dap
{
    namespace dsp
    {
        template <typename T>
        class LadderFilter;
    }
}

template <typename T>
class dap::dsp::LadderFilter final
{
    struct Stage
    {
        T output{0};
        inline auto operator()(T x, T gain)
        {
            output += gain * (std::tanh(x) - std::tanh(output));
            return output;
        }
    };

    T m_y{0};
    T m_y1{0};
    Stage m_stage1;
    Stage m_stage2;
    Stage m_stage3;

public:
    inline auto operator()(T x, T frequency, T resonance, T samplerate)
    {
        auto g = T(1) - std::exp(T(-2) * std::tan(M_PI / samplerate * frequency));
        m_y += g * (std::tanh(x - clip(resonance, T(0), T(4)) * (m_stage3.output + m_y1) * T(0.5)) -
                    std::tanh(m_y));
        m_y1 = m_stage3.output;
        m_stage3(m_stage2(m_stage1(m_y, g), g), g);
        return m_y;
    }
};
#endif // DAP_DSP_LADDER_FILTER_H
