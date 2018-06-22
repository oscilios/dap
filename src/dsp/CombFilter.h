#ifndef DAP_DSP_COMB_FILTER_H
#define DAP_DSP_COMB_FILTER_H

#include "DelayLine.h"

namespace dap
{
    namespace dsp
    {
        struct CombFilter
        {
        };
        template <typename T, size_t N>
        class FeedforwardCombFilter;
        template <typename T, size_t N>
        class FeedbackCombFilter;
    }
}

template <typename T, size_t N>
class dap::dsp::FeedforwardCombFilter final : public CombFilter
{
    DelayLine<T, N> m_delay;

public:
    template <typename T1, typename T2, typename T3>
    inline auto operator()(T1 input, T2 delay, T3 gain)
    {
        return input + gain * m_delay(input, delay) / sqrt((T(1) + gain * gain));
    }
};

template <typename T, size_t N>
class dap::dsp::FeedbackCombFilter final : public CombFilter
{
    DelayLine<T, N> m_delay;
    T m_output{0};

public:
    template <typename T1, typename T2, typename T3>
    inline auto operator()(T1 input, T2 delay, T3 gain)
    {
        m_output = (input + gain * m_delay(m_output, delay)) / sqrt((T(1) + gain * gain));
        return m_output;
    }
};
#endif // DAP_DSP_COMB_FILTER_H
