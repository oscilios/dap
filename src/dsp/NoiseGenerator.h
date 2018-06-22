#ifndef DAP_DSP_NOISE_GENERATOR_H
#define DAP_DSP_NOISE_GENERATOR_H

#include "IIRFilter.h"
#include "fastmath/Variable.h"
#include <ostream>

namespace dap
{
    namespace dsp
    {
        template <typename TRandomGenerator>
        class NoiseGenerator;
    }
}

template <typename TRandomGenerator>
class dap::dsp::NoiseGenerator final
{
    using value_type = typename TRandomGenerator::value_type;
    using var_type   = dap::fastmath::Variable<value_type>;
    using Filter     = IIRFilter<var_type, 4>;
    TRandomGenerator m_rand;
    Filter m_f1;
    Filter m_f2;
    Filter m_f3;

public:
    enum class Color
    {
        White,
        Pink,
        Brown,
        OneOverF3
    };

    NoiseGenerator()
    {
        // coefficients for an aproximated 1/f amplitude response roll-off IIR filter
        // see https://www.dsprelated.com/freebooks/sasp/Example_Synthesis_1_F_Noise.html
        // ideally we should be normalizing by 1.1 which seems to be the maximum at 0Hz,
        // however noise will not be at 0Hz, so we can increase gain.
        using Array = std::array<var_type, 4>;
        const Array a{{1.0f, -2.494956002f, 2.017265875f, -0.522189400f}};

        auto computeBCoefs = [](var_type norm) {
            return Array{{var_type(0.049922035f) / norm,
                          var_type(-0.095993537f) / norm,
                          var_type(0.050612699f) / norm,
                          var_type(-0.004408786f) / norm}};
        };

        Array b = computeBCoefs(0.25); // trial error value
        m_f1.set(b.data(), a.data());
        b = computeBCoefs(0.45); // trial error value
        m_f2.set(b.data(), a.data());
        b = computeBCoefs(1.0); // trial error value
        m_f3.set(b.data(), a.data());
    }
    inline auto operator()(value_type gain, Color color)
    {
        switch (color)
        {
            case Color::White:
                return gain*m_rand();
            case Color::Pink:
                return gain*m_f1(m_rand())();
            case Color::Brown:
                return gain*m_f2(m_f1(m_rand()))();
            case Color::OneOverF3:
                return gain*m_f3(m_f2(m_f1(m_rand())))();
        }
        return value_type(0);
    }
    friend std::ostream& operator<<(std::ostream& out, Color color)
    {
        switch (color)
        {
            case Color::White:
                out << "White";
                break;
            case Color::Pink:
                out << "Pink";
                break;
            case Color::Brown:
                out << "Brown";
                break;
            case Color::OneOverF3:
                out << "OneOverF3";
                break;
        }
        return out;
    }
};

#endif // DAP_DSP_NOISE_GENERATOR_H
