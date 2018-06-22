#ifndef DAP_DSP_DELAY_LINE_H
#define DAP_DSP_DELAY_LINE_H

#include "base/TypeTraits.h"

namespace dap
{
    namespace dsp
    {
        template <typename T, size_t N>
        class DelayLine;
    }
}

template <typename T, size_t N>
class dap::dsp::DelayLine final
{
    static_assert(IsPowerOfTwo<N>::value, "N must be a power of two");
    static_assert(N > 0, "N must be greater than zero");
    using bitmask = std::integral_constant<size_t, N - 1>;
    std::array<T, N> m_buffer{};
    size_t m_write{0};

public:
    template <typename T1, typename T2, DAP_REQUIRES(!isIntegral<T2>())>
    inline auto operator()(T1 input, T2 delay)
    {
        const float d         = std::min<float>(N - 1, delay);
        m_buffer[m_write]     = input;
        const auto int_delay  = static_cast<size_t>(d);
        const auto frac_delay = d - static_cast<size_t>(int_delay);
        const size_t read     = (m_write - int_delay) & bitmask::value;
        const auto cur        = m_buffer[read];
        const auto prev       = m_buffer[(read - 1u) & bitmask::value];
        m_write               = (m_write + 1u) & bitmask::value;
        return cur + frac_delay * (prev - cur);
    }
    template <typename T1, typename T2, DAP_REQUIRES(isIntegral<T2>())>
    inline auto operator()(T1 input, T2 delay)
    {
        m_buffer[m_write] = input;
        const auto read   = (m_write - std::min<T2>(N - 1, delay)) & bitmask::value;
        m_write           = (m_write + 1u) & bitmask::value;
        return m_buffer[read];
    }
};
#endif // DAP_DSP_DELAY_LINE_H
