#ifndef DAP_DSP_BIQUAD_FILTER_H
#define DAP_DSP_BIQUAD_FILTER_H

#include "IIRFilter.h"
#include "base/TypeTraits.h"
#include <cmath>

namespace dap
{
    namespace dsp
    {
        template <typename T>
        class BiquadFilter;
    }
}

template <typename T>
class dap::dsp::BiquadFilter final
{
    static_assert(dap::isFloatingPoint<T>(), "T must be floating point.");

public:
    enum class Type
    {
        LowPass = 0,
        HighPass,
        BandPass,
        Notch,
        Peak,
        LowShelf,
        HighShelf
    };

    inline auto operator()(T input, T frequency, T Q, Type type, T gainDb, T samplerate)
    {
        if (type != m_currentType)
        {
            m_filter.reset();
            m_currentType = type;
        }

        const T w0    = T(2) * T(M_PI) * frequency / samplerate;
        const T cosw0 = std::cos(w0);
        const T sinw0 = std::sin(w0);
        const T alpha = sinw0 / (T(2) * std::max(Q, T(0.001)));

        T b0, b1, b2, a0, a1, a2;

        switch (type)
        {
            case Type::LowPass:
                b0 = (T(1) - cosw0) / T(2);
                b1 = T(1) - cosw0;
                b2 = (T(1) - cosw0) / T(2);
                a0 = T(1) + alpha;
                a1 = T(-2) * cosw0;
                a2 = T(1) - alpha;
                break;

            case Type::HighPass:
                b0 = (T(1) + cosw0) / T(2);
                b1 = -(T(1) + cosw0);
                b2 = (T(1) + cosw0) / T(2);
                a0 = T(1) + alpha;
                a1 = T(-2) * cosw0;
                a2 = T(1) - alpha;
                break;

            case Type::BandPass:
                b0 = alpha;
                b1 = T(0);
                b2 = -alpha;
                a0 = T(1) + alpha;
                a1 = T(-2) * cosw0;
                a2 = T(1) - alpha;
                break;

            case Type::Notch:
                b0 = T(1);
                b1 = T(-2) * cosw0;
                b2 = T(1);
                a0 = T(1) + alpha;
                a1 = T(-2) * cosw0;
                a2 = T(1) - alpha;
                break;

            case Type::Peak:
            {
                const T A = std::pow(T(10), gainDb / T(40));
                b0        = T(1) + alpha * A;
                b1        = T(-2) * cosw0;
                b2        = T(1) - alpha * A;
                a0        = T(1) + alpha / A;
                a1        = T(-2) * cosw0;
                a2        = T(1) - alpha / A;
                break;
            }

            case Type::LowShelf:
            {
                const T A     = std::pow(T(10), gainDb / T(40));
                const T sqrtA = std::sqrt(A);
                b0            = A * ((A + T(1)) - (A - T(1)) * cosw0 + T(2) * sqrtA * alpha);
                b1            = T(2) * A * ((A - T(1)) - (A + T(1)) * cosw0);
                b2            = A * ((A + T(1)) - (A - T(1)) * cosw0 - T(2) * sqrtA * alpha);
                a0            = (A + T(1)) + (A - T(1)) * cosw0 + T(2) * sqrtA * alpha;
                a1            = T(-2) * ((A - T(1)) + (A + T(1)) * cosw0);
                a2            = (A + T(1)) + (A - T(1)) * cosw0 - T(2) * sqrtA * alpha;
                break;
            }

            case Type::HighShelf:
            {
                const T A     = std::pow(T(10), gainDb / T(40));
                const T sqrtA = std::sqrt(A);
                b0            = A * ((A + T(1)) + (A - T(1)) * cosw0 + T(2) * sqrtA * alpha);
                b1            = T(-2) * A * ((A - T(1)) + (A + T(1)) * cosw0);
                b2            = A * ((A + T(1)) + (A - T(1)) * cosw0 - T(2) * sqrtA * alpha);
                a0            = (A + T(1)) - (A - T(1)) * cosw0 + T(2) * sqrtA * alpha;
                a1            = T(2) * ((A - T(1)) - (A + T(1)) * cosw0);
                a2            = (A + T(1)) - (A - T(1)) * cosw0 - T(2) * sqrtA * alpha;
                break;
            }
        }

        // Normalize coefficients (IIRFilter assumes a[0] = 1)
        const T inv_a0 = T(1) / a0;
        const T b[3]   = {b0 * inv_a0, b1 * inv_a0, b2 * inv_a0};
        const T a[3]   = {T(1), a1 * inv_a0, a2 * inv_a0};
        m_filter.set(b, a);

        return m_filter(input);
    }

private:
    IIRFilter<T, 3> m_filter;
    Type m_currentType{static_cast<Type>(-1)};

};

#endif // DAP_DSP_BIQUAD_FILTER_H
