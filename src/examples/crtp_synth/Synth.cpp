#include "Synth.h"
#include "crtp/utility/InputNamesPrinter.h"

using crtp_synth::Synth;
using dap::operator""_s;

namespace
{
    struct SamplerateVisitor
    {
        const crtp_synth::scalar_t m_samplerate;

        template <typename Processor,
                  typename Inputs,
                  typename InputNames,
                  DAP_REQUIRES(tupleContainsType("samplerate"_s, InputNames{}))>
        void visit(dap::crtp::ProcessorNode<Processor, Inputs, InputNames>& node)
        {
            node.input("samplerate"_s) = m_samplerate;
        }
        template <typename... Ts>
        void visit(Ts&...)
        {
            // nothing to do
        }
        explicit SamplerateVisitor(crtp_synth::scalar_t samplerate)
        : m_samplerate(samplerate)
        {
        }
    };
}

Synth::Synth(size_t bufferSize, scalar_t samplerate)
: m_output(1, bufferSize, 0.0f)
{
    auto& This = *this;

    // buses gain
    This["/synth/set/bus0/gain/value"_s] = 1.0f;
    This["/synth/set/bus1/gain/value"_s] = 1.0f;
    This["/synth/set/bus2/gain/value"_s] = 1.0f;

    // effects
    This["/synth/set/filter/frequency/value"_s] = 500.0f;
    This["/synth/set/filter/resonance/value"_s] = 3.5f;
    This["/synth/set/phaser/frequency/value"_s] = 0.10f;
    This["/synth/set/phaser/depth/value"_s]     = 0.5f;
    This["/synth/set/phaser/feedback/value"_s]  = 1.0f;
    This["/synth/set/phaser/wet/value"_s]       = 0.9f;

    // noise
    This["/synth/set/noise/gain/value"_s] = 1.0f;
    This["/synth/set/noise/color"_s]      = noise_gen_t::Color::Pink;
    // osc5
    This["/synth/set/osc5/frequency/value"_s]      = 440.0f;
    This["/synth/set/osc5/frequency/portamento"_s] = bufferSize;
    This["/synth/set/osc5/gain/value"_s]           = 0.5f;
    This["/synth/set/osc5/phase/value"_s]          = 0.0f;
    // osc6
    This["/synth/set/osc6/frequency/value"_s]      = 329.63f;
    This["/synth/set/osc6/frequency/portamento"_s] = bufferSize;
    This["/synth/set/osc6/gain/value"_s]           = 0.5f;
    This["/synth/set/osc6/phase/value"_s]          = 0.0f;
    // osc1
    This["/synth/set/osc1/frequency/value"_s] = 4.0f;
    This["/synth/set/osc1/gain/value"_s]      = 1.0f;
    This["/synth/set/osc1/phase/value"_s]     = 0.0f;
    // osc2
    This["/synth/set/osc2/frequency/value"_s] = 220.0f; // 196.0f;
    This["/synth/set/osc2/gain/value"_s]      = 500.0f;
    This["/synth/set/osc2/phase/value"_s]     = 0.0f;
    // osc3
    This["/synth/set/osc3/frequency/value"_s] = 55.0f;
    This["/synth/set/osc3/gain/value"_s]      = 1.0f;
    This["/synth/set/osc3/phase/value"_s]     = M_PI / 2.0f;
    // osc4
    This["/synth/set/osc4/frequency/value"_s] = 220.0f; // 261.63f;
    This["/synth/set/osc4/gain/value"_s]      = 150.0f;
    This["/synth/set/osc4/phase/value"_s]     = M_PI / 4.0f;

    This["/synth/set/osc5/shape"_s] = osc_shape_t::Sine;
    This["/synth/set/osc6/shape"_s] = osc_shape_t::Sine;
    This["/synth/set/osc1/shape"_s] = osc_shape_t::Sine;
    This["/synth/set/osc2/shape"_s] = osc_shape_t::Sine;
    This["/synth/set/osc3/shape"_s] = osc_shape_t::Sine;
    This["/synth/set/osc4/shape"_s] = osc_shape_t::Sine;

    setSamplerate(samplerate);
    // dap::crtp::NodeVisitor<dap::crtp::InputNamesPrinter>()(m_graph); // TODO: seems broken for
    // mixer processor
}
void Synth::setSamplerate(scalar_t samplerate)
{
    auto visit = dap::crtp::make_node_visitor<SamplerateVisitor>(samplerate);
    visit(m_graph);
}
