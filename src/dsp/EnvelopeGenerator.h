#ifndef DAP_DSP_ENVELOPE_GENERATOR_H
#define DAP_DSP_ENVELOPE_GENERATOR_H

#include "base/Constants.h"
#include "base/TypeTraits.h"
#include <cmath>

namespace dap
{
    namespace dsp
    {
        template <typename T>
        class EnvelopeGenerator;
    }
}

template <typename T>
class dap::dsp::EnvelopeGenerator final
{
public:
    enum class Phase
    {
        Idle,
        Attack,
        Decay,
        Sustain,
        Release
    };

private:
    T m_currentValue{0};
    Phase m_currentPhase{Phase::Idle};
    T m_attackCoeff{0};
    T m_decayCoeff{0};
    T m_releaseCoeff{0};
    bool m_previousGate{false};

    static_assert(dap::isFloatingPoint<T>(), "T must be floating point.");

    // Calculate exponential coefficient for smooth transitions
    inline T calculateCoefficient(T timeSeconds, T sampleRate) const
    {
        if (timeSeconds <= T(0))
            return T(0);
        // Use -4.6 for reaching ~99% in the specified time (more accurate timing)
        return std::exp(-T(4.6) / (timeSeconds * sampleRate));
    }

public:
    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    inline auto operator()(T1 gate, T2 attackTime, T3 decayTime, T4 sustainLevel, T5 releaseTime, T6 sampleRate)
    {
        const bool gateHigh = gate > T(0.5);
        const bool gateTriggered = gateHigh && !m_previousGate;
        const bool gateReleased = !gateHigh && m_previousGate;
        
        m_previousGate = gateHigh;

        // Update coefficients (could be optimized by only updating when parameters change)
        m_attackCoeff = calculateCoefficient(attackTime, sampleRate);
        m_decayCoeff = calculateCoefficient(decayTime, sampleRate);
        m_releaseCoeff = calculateCoefficient(releaseTime, sampleRate);

        // State transitions
        if (gateTriggered)
        {
            m_currentPhase = Phase::Attack;
        }
        else if (gateReleased && m_currentPhase != Phase::Idle)
        {
            m_currentPhase = Phase::Release;
        }

        // Process current phase
        switch (m_currentPhase)
        {
            case Phase::Idle:
                m_currentValue = T(0);
                break;

            case Phase::Attack:
                if (attackTime <= T(0))
                {
                    m_currentValue = T(1);
                    m_currentPhase = Phase::Decay;
                }
                else
                {
                    m_currentValue = T(1) - (T(1) - m_currentValue) * m_attackCoeff;
                    if (m_currentValue >= T(0.99)) // Reach 99% to complete attack phase
                    {
                        m_currentValue = T(1);
                        m_currentPhase = Phase::Decay;
                    }
                }
                break;

            case Phase::Decay:
                if (decayTime <= T(0))
                {
                    m_currentValue = sustainLevel;
                    m_currentPhase = Phase::Sustain;
                }
                else
                {
                    m_currentValue = sustainLevel + (m_currentValue - sustainLevel) * m_decayCoeff;
                    if (std::abs(m_currentValue - sustainLevel) < T(0.01)) // Reach within 1% of sustain level
                    {
                        m_currentValue = sustainLevel;
                        m_currentPhase = Phase::Sustain;
                    }
                }
                break;

            case Phase::Sustain:
                m_currentValue = sustainLevel;
                break;

            case Phase::Release:
                if (releaseTime <= T(0))
                {
                    m_currentValue = T(0);
                    m_currentPhase = Phase::Idle;
                }
                else
                {
                    m_currentValue = m_currentValue * m_releaseCoeff;
                    if (m_currentValue < T(0.01)) // Reach 1% to complete release
                    {
                        m_currentValue = T(0);
                        m_currentPhase = Phase::Idle;
                    }
                }
                break;
        }

        return m_currentValue;
    }
    
    // Reset the envelope to idle state
    void reset()
    {
        m_currentValue = T(0);
        m_currentPhase = Phase::Idle;
        m_previousGate = false;
    }
};

#endif // DAP_DSP_ENVELOPE_GENERATOR_H