#ifndef DAP_DSP_ALLPASS_H
#define DAP_DSP_ALLPASS_H

#include <cmath>

namespace dap
{
    namespace dsp
    {
        template <typename T>
        class AllPass;
    }
}
template <typename T>
class dap::dsp::AllPass final
{
    T m_output{0};
    T m_d{0};

public:
    inline auto operator()(T input, T frequency, T samplerate)
    {
        const T w = std::tan(M_PI * frequency / samplerate);
        const T k = (w - T(1)) / (w + T(1));

        m_output = k * input + m_d;
        m_d      = input - k * m_output;
        return m_output;
    }
};
#endif // DAP_DSP_ALLPASS_H
