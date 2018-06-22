#ifndef DAP_DSP_PHASER_H
#define DAP_DSP_PHASER_H

#include "fastmath/Var.h"
#include "AllPass.h"
#include "Phasor.h"
#include "OscillatorFunctions.h"

namespace dap
{
    namespace dsp
    {
        template <typename T>
        class Phaser;
    }
}
template <typename T>
class dap::dsp::Phaser final
{
    static constexpr int m_stageCount{6};
    constexpr auto freqs(int i)
    {
        constexpr std::array<T, m_stageCount> f{{T(16), T(33), T(48), T(98), T(160), T(260)}};
        return f[i];
    }
    std::array<AllPass<T>, m_stageCount> m_allpass;
    Phasor<T> m_phasor;
    T m_output;

public:
    inline auto operator()(T input, T frequency, T depth, T feedback, T wet, T samplerate)
    {
        const auto lfo =
            T(1) + depth * T(0.5) *
                       (T(1) + OscillatorFunctions::process(m_phasor(frequency, samplerate),
                                                            OscillatorFunctions::SineTag{}));
        m_output = (T(1) - wet) * input + wet *
                    m_allpass[5](
                    m_allpass[4](
                    m_allpass[3](
                    m_allpass[2](
                    m_allpass[1](
                    m_allpass[0]((input + feedback*m_output)
                                      , freqs(0) * lfo, samplerate)
                                      , freqs(1) * lfo, samplerate)
                                      , freqs(2) * lfo, samplerate)
                                      , freqs(3) * lfo, samplerate)
                                      , freqs(4) * lfo, samplerate)
                                      , freqs(5) * lfo, samplerate);
        return m_output;
    }
};
#endif // DAP_DSP_PHASER_H
