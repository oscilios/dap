#pragma once
#include <algorithm>
#include <random>
#include <vector>

namespace sub37
{

    class Arpeggiator
    {
    public:
        enum class Pattern
        {
            Up,
            Down,
            UpDown,
            Random
        };

        void noteOn(int midiNote)
        {
            auto it = std::lower_bound(m_heldNotes.begin(), m_heldNotes.end(), midiNote);
            if (it == m_heldNotes.end() || *it != midiNote)
                m_heldNotes.insert(it, midiNote);
        }

        void noteOff(int midiNote)
        {
            auto it = std::lower_bound(m_heldNotes.begin(), m_heldNotes.end(), midiNote);
            if (it != m_heldNotes.end() && *it == midiNote)
                m_heldNotes.erase(it);
            if (m_heldNotes.empty())
                reset();
        }

        int step()
        {
            if (m_heldNotes.empty())
                return -1;

            auto n      = static_cast<int>(m_heldNotes.size());
            int total   = n * m_octaveRange;
            int noteIdx = 0;
            int octave  = 0;

            switch (m_pattern)
            {
                case Pattern::Up:
                    m_stepIndex %= total;
                    noteIdx = m_stepIndex % n;
                    octave  = m_stepIndex / n;
                    m_stepIndex++;
                    break;

                case Pattern::Down:
                    m_stepIndex %= total;
                    {
                        int rev = total - 1 - m_stepIndex;
                        noteIdx = rev % n;
                        octave  = rev / n;
                    }
                    m_stepIndex++;
                    break;

                case Pattern::UpDown:
                {
                    int cycle  = total > 1 ? (total - 1) * 2 : 1;
                    int pos    = m_stepIndex % cycle;
                    int linear = pos < total ? pos : (2 * (total - 1) - pos);
                    noteIdx    = linear % n;
                    octave     = linear / n;
                    m_stepIndex++;
                    break;
                }

                case Pattern::Random:
                {
                    int idx = m_dist(m_rng) % total;
                    noteIdx = idx % n;
                    octave  = idx / n;
                    break;
                }
            }

            return m_heldNotes[noteIdx] + octave * 12;
        }

        void reset()
        {
            m_stepIndex = 0;
        }

        void setPattern(Pattern p)
        {
            m_pattern = p;
            reset();
        }

        void setOctaveRange(int range)
        {
            m_octaveRange = std::clamp(range, 1, 4);
            reset();
        }

        bool hasNotes() const
        {
            return !m_heldNotes.empty();
        }

        void clear()
        {
            m_heldNotes.clear();
            reset();
        }

    private:
        std::vector<int> m_heldNotes;
        Pattern m_pattern = Pattern::Up;
        int m_octaveRange = 1;
        int m_stepIndex   = 0;
        std::mt19937 m_rng{std::random_device{}()};
        std::uniform_int_distribution<int> m_dist{0, 9999};
    };

} // namespace sub37
