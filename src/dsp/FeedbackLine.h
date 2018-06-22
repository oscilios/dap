#ifndef DAP_DSP_FEEDBACK_LINE_H
#define DAP_DSP_FEEDBACK_LINE_H

#include "DelayLine.h"

namespace dap
{
    namespace dsp
    {
        template <typename T, size_t N>
        class FeedbackLine;
    }
}
template <typename T, size_t N>
class dap::dsp::FeedbackLine final
{
    DelayLine<T, N> m_delay;
    T m_output{0};

public:
    template <typename T1, typename T2, typename T3>
    inline auto operator()(T1 input, T2 delay, T3 feedback)
    {
        m_output =
            m_delay((input + m_output * feedback) / sqrt((T(1) + feedback * feedback)), delay);
        return m_output;
    }
};
#endif // DAP_DSP_FEEDBACK_LINE_H
