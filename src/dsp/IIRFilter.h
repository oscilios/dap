#ifndef DAP_DSP_IIRFILTER_H
#define DAP_DSP_IIRFILTER_H

#include <array>

namespace dap
{
    namespace dsp
    {
        template <typename T, int TCoefsSize>
        class IIRFilter;
    }
}

// typically:
//            b0 + b1z[-1] + b2z[-2] + ...
// H(z) = g --------------------------------
//            a0 + a1z[-1] + a2z[-2] + ...
//
//  a0*y[n] = g*(b0*x[n] + b1*x[n-1] + b2*x[n-2] +...) - (a1*x[n-1] + a2*x[n-2] + ...)
//

template <typename T, int TCoefsSize>
class dap::dsp::IIRFilter final
{
    using Array = std::array<T, TCoefsSize>;

    Array m_a;
    Array m_b;
    Array m_d;

public:
    IIRFilter()
    {
        static_assert(TCoefsSize > 1, "size of coefs must be at least 1.");

        m_a.fill(0);
        m_b.fill(0);
        m_d.fill(0);
    }
    void set(const T* b, const T* a)
    {
        std::copy(a, a + TCoefsSize, m_a.begin());
        std::copy(b, b + TCoefsSize, m_b.begin());
    }
    void reset()
    {
        m_d.fill(0);
    }
    /*
       direct form II
        y[n] = b[0]*x[n] + d[0];
        d[0] = b[1]*x[n] - a[1]*y[n] + d[1];
        d[1] = b[2]*x[n] - a[2]*y[n] + d[2];
        d[2] = b[3]*x[n] - a[3]*y[n];
       */
    inline auto operator()(T x)
    {
        T y = m_b[0] * x + m_d[0];
        for (int i = 1; i < TCoefsSize - 1; ++i)
        {
            m_d[i - 1] = m_b[i] * x - m_a[i] * y + m_d[i];
        }
        m_d[TCoefsSize - 2] = m_b[TCoefsSize - 1] * x - m_a[TCoefsSize - 1] * y;
        return y;
    }
};
#endif // DAP_DSP_IIRFILTER_H
