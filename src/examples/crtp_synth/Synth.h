#ifndef DAP_EXAMPLES_CRTP_SYNTH_SYNTH_H
#define DAP_EXAMPLES_CRTP_SYNTH_SYNTH_H

#include "Types.h"
#include "base/KeyValueTuple.h"
#include "fastmath/AudioBuffer.h"

namespace crtp_synth
{
    class Synth;
}

// TODO: convert this graph into an algorihtm that derives from NodeExpression, so we can later
// reuse and connect algorithms

// clang-format off
/*
 *     +-----------------------+      +-----------------------+                        +-----------------------+      +-----------------------+
 *     |amp/freq/phase controls|      |amp/freq/phase controls|                        |amp/freq/phase controls|      |amp/freq/phase controls|
 *     +-----------------------+      +-----------------------+                        +-----------------------+      +-----------------------+
 *                |  +-------+                   |   +-------+                                    |  +-------+                   |   +-------+
 *                |  | shape |                   |   | shape |                                    |  | shape |                   |   | shape |
 *                |  +-------+                   |   +-------+                                    |  +-------+                   |   +-------+
 *                |      |                       |       |                                        |      |                       |       |
 *     +-------------------+          +-------------------+      +-------+             +-------------------+          +-------------------+      +-------+
 *     |        OSC1       |          |        OSC2       |      | shape |             |        OSC3       |          |        OSC4       |      | shape |
 *     +-------------------+          +-------------------+      +-------+             +-------------------+          +-------------------+      +-------+
 *                |                             |                    |                            |                             |                    |
 *                |                             |                    |                            |                             |                    |
 *             amp|                         freq|                    |                         amp|                         freq|                    |
 *                +-----------------------------+--------------------+                            +-----------------------------+--------------------+
 *                                              |                                                                               |
 *                                              |                                                                               |
 *                                              |                                                                               |
 *                                    +-------------------+           +-------------------+                           +-------------------+
 *                                    |        OSC5       |           |       noise       |                           |        OSC6       |
 *                                    +-------------------+           +-------------------+                           +-------------------+
 *                                             |                                 |                                              |
 *                                             +------------------+--------------+----------------------------------------------+
 *                                                                |
 *                                                       +-------------------+
 *                                                       |       Mixer       |
 *                                                       +-------------------+
 *                                                                |
 *                                                                |
 *                                                       +-------------------+
 *                                                       |       Filter      |
 *                                                       +-------------------+
 *                                                                |
 *                                                                |
 *                                                       +-------------------+
 *                                                       |       Phaser      |
 *                                                       +-------------------+
 */
// clang-format on

class crtp_synth::Synth final
{
    using buffer_t = dap::fastmath::AudioBuffer<float>;

private:
    buffer_t m_output;
    phaser_t<filter_t<mixer_t<am_fm_t, am_fm_t, noise_t>>> m_graph;

public:
    Synth(size_t bufferSize, scalar_t samplerate);
    void setSamplerate(scalar_t samplerate);
    void process()
    {
#if defined(__clang__)
#pragma clang loop unroll_count(16)
#endif
        for (auto& x : m_output.channel(0))
        {
            x = m_graph();
        }
    }
    const buffer_t& output() const
    {
        return m_output;
    }
    constexpr auto params()
    {
        using std::make_tuple;

        auto& filter    = m_graph.input<0>();
        auto& mixer     = filter.input<0>();

        // left branch
        auto& osc5 = mixer.input<0>().input("signal"_s);
        auto& osc1 = osc5.input("gain"_s).input("x"_s);
        auto& osc2 = osc5.input("frequency"_s).input("x"_s);
        // right branch
        auto& osc6 = mixer.input<1>().input("signal"_s);
        auto& osc3 = osc6.input("gain"_s).input("x"_s);
        auto& osc4 = osc6.input("frequency"_s).input("x"_s);

        // noise
        auto& noise = mixer.input<2>().input("signal"_s);

        // clang-format off
        return dap::make_key_value_tuple(
            make_tuple("/synth/set/filter/frequency/value"_s,      &filter.input("frequency"_s).input("value"_s)),
            make_tuple("/synth/set/filter/resonance/value"_s,      &filter.input("resonance"_s).input("value"_s)),

            make_tuple("/synth/set/phaser/frequency/value"_s,      &m_graph.input("frequency"_s).input("value"_s)),
            make_tuple("/synth/set/phaser/depth/value"_s,          &m_graph.input("depth"_s).input("value"_s)),
            make_tuple("/synth/set/phaser/feedback/value"_s,       &m_graph.input("feedback"_s).input("value"_s)),
            make_tuple("/synth/set/phaser/wet/value"_s,            &m_graph.input("wet"_s).input("value"_s)),
            make_tuple("/synth/set/bus0/gain/value"_s,             &mixer.input<0>().input("gain"_s).input("value"_s)),
            make_tuple("/synth/set/bus1/gain/value"_s,             &mixer.input<1>().input("gain"_s).input("value"_s)),
            make_tuple("/synth/set/bus2/gain/value"_s,             &mixer.input<2>().input("gain"_s).input("value"_s)),
            // noise
            make_tuple("/synth/set/noise/gain/value"_s,            &noise.input("gain"_s).input("value"_s)),
            make_tuple("/synth/set/noise/color"_s,                 &noise.input("color"_s)),
            // osc5
            make_tuple("/synth/set/osc5/frequency/value"_s,        &osc5.input("frequency"_s).input("y"_s).input("value"_s)),
            make_tuple("/synth/set/osc5/frequency/portamento"_s,   &osc5.input("frequency"_s).input("y"_s).input("duration"_s)),
            make_tuple("/synth/set/osc5/gain/value"_s,             &osc5.input("gain"_s).input("y"_s).input("value"_s)),
            make_tuple("/synth/set/osc5/phase/value"_s,            &osc5.input("phase"_s).input("value"_s)),
            make_tuple("/synth/set/osc5/shape"_s,                  &osc5.input("shape"_s)),
            // osc6
            make_tuple("/synth/set/osc6/frequency/value"_s,        &osc6.input("frequency"_s).input("y"_s).input("value"_s)),
            make_tuple("/synth/set/osc6/frequency/portamento"_s,   &osc6.input("frequency"_s).input("y"_s).input("duration"_s)),
            make_tuple("/synth/set/osc6/gain/value"_s,             &osc6.input("gain"_s).input("y"_s).input("value"_s)),
            make_tuple("/synth/set/osc6/phase/value"_s,            &osc6.input("phase"_s).input("value"_s)),
            make_tuple("/synth/set/osc6/shape"_s,                  &osc6.input("shape"_s)),
            // osc1
            make_tuple("/synth/set/osc1/frequency/value"_s,        &osc1.input("frequency"_s).input("value"_s)),
            make_tuple("/synth/set/osc1/gain/value"_s,             &osc1.input("gain"_s).input("value"_s)),
            make_tuple("/synth/set/osc1/phase/value"_s,            &osc1.input("phase"_s).input("value"_s)),
            make_tuple("/synth/set/osc1/shape"_s,                  &osc1.input("shape"_s)),
            // osc2
            make_tuple("/synth/set/osc2/frequency/value"_s,        &osc2.input("frequency"_s).input("value"_s)),
            make_tuple("/synth/set/osc2/gain/value"_s,             &osc2.input("gain"_s).input("value"_s)),
            make_tuple("/synth/set/osc2/phase/value"_s,            &osc2.input("phase"_s).input("value"_s)),
            make_tuple("/synth/set/osc2/shape"_s,                  &osc2.input("shape"_s)),
            // osc3
            make_tuple("/synth/set/osc3/frequency/value"_s,        &osc3.input("frequency"_s).input("value"_s)),
            make_tuple("/synth/set/osc3/gain/value"_s,             &osc3.input("gain"_s).input("value"_s)),
            make_tuple("/synth/set/osc3/phase/value"_s,            &osc3.input("phase"_s).input("value"_s)),
            make_tuple("/synth/set/osc3/shape"_s,                  &osc3.input("shape"_s)),
            // osc4
            make_tuple("/synth/set/osc4/frequency/value"_s,        &osc4.input("frequency"_s).input("value"_s)),
            make_tuple("/synth/set/osc4/gain/value"_s,             &osc4.input("gain"_s).input("value"_s)),
            make_tuple("/synth/set/osc4/phase/value"_s,            &osc4.input("phase"_s).input("value"_s)),
            make_tuple("/synth/set/osc4/shape"_s,                  &osc4.input("shape"_s))
            );
        // clang-format on
    }
    template <char... Chars>
    constexpr auto& operator[](dap::constexpr_string<Chars...> key)
    {
        return *params()[key];
    }
};

#endif // DAP_EXAMPLES_CRTP_SYNTH_SYNTH_H
