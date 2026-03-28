#pragma once
#include <algorithm>
#include <array>

namespace sub37
{

    class Sequencer
    {
    public:
        static constexpr int MaxSteps = 32;

        struct Step
        {
            int note    = -1; // MIDI note, -1 = rest
            bool active = false;
        };

        int currentStep() const
        {
            return m_currentStep;
        }

        // Returns note at current step and advances; -1 for rest/inactive
        int step()
        {
            int idx       = m_currentStep;
            m_currentStep = (m_currentStep + 1) % m_stepCount;
            if (m_steps[idx].active && m_steps[idx].note >= 0)
                return m_steps[idx].note;
            return -1;
        }

        void reset()
        {
            m_currentStep = 0;
        }

        void setStepCount(int n)
        {
            m_stepCount = std::clamp(n, 1, MaxSteps);
            if (m_currentStep >= m_stepCount)
                m_currentStep = 0;
        }

        int stepCount() const
        {
            return m_stepCount;
        }

        void setNote(int idx, int note)
        {
            if (idx >= 0 && idx < MaxSteps)
            {
                m_steps[idx].note   = note;
                m_steps[idx].active = (note >= 0);
            }
        }

        int getNote(int idx) const
        {
            if (idx >= 0 && idx < MaxSteps)
                return m_steps[idx].note;
            return -1;
        }

        bool isActive(int idx) const
        {
            return idx >= 0 && idx < MaxSteps && m_steps[idx].active;
        }

        void clearStep(int idx)
        {
            if (idx >= 0 && idx < MaxSteps)
            {
                m_steps[idx].note   = -1;
                m_steps[idx].active = false;
            }
        }

        void clear()
        {
            for (auto& s : m_steps)
            {
                s.note   = -1;
                s.active = false;
            }
            m_currentStep = 0;
        }

    private:
        std::array<Step, MaxSteps> m_steps = {};
        int m_stepCount                    = 16;
        int m_currentStep                  = 0;
    };

} // namespace sub37
