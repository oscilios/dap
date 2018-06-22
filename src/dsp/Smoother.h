#ifndef DAP_DSP_SMOOTHER_H
#define DAP_DSP_SMOOTHER_H

#include "base/Constants.h"
#include <cmath>

namespace dap
{
    namespace dsp
    {
        template <typename T>
        class Smoother;
        template <typename T, size_t Samples>
        class FixedSmoother;
    }
}
template <typename T>
class dap::dsp::Smoother
{
    T m_value{0};

    static_assert(dap::isFloatingPoint<T>(), "T must be floating point.");

public:
    inline auto operator()(T target, size_t samples = 4096)
    {
        auto a = std::exp(-TWO_PI / T(samples));
        m_value = a * m_value + (T(1) - a) * target;
        return m_value;
    }
};
template <typename T, size_t Samples>
class dap::dsp::FixedSmoother
{
    T m_value{0};
    T m_a{std::exp(-TWO_PI / T(Samples))};
    T m_b{T(1.0) - std::exp(-TWO_PI / T(Samples))}; // 1-m_a

    static_assert(dap::isFloatingPoint<T>(), "T must be floating point.");

public:
    inline auto operator()(T target)
    {
        m_value = m_a * m_value + m_b * target;
        return m_value;
    }
};

#endif // DAP_DSP_SMOOTHER_H
