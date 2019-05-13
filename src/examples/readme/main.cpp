#include "crtp/nodes/Processor.h"
#include "dsp/Oscillator.h"
#include "dsp/Smoother.h"

namespace dsp = dap::dsp;
using dap::crtp::processor;
using dap::operator""_s;

using scalar_t     = float;
using samplerate_t = scalar_t;

struct fm
{
private:
    // a smoothed control
    using control_t = decltype(
        processor<dsp::FixedSmoother<scalar_t, 512>>::with_inputs<scalar_t>::named("value"_s));

    // alias for oscillator shapes
    using shape_t = dsp::OscillatorFunctions::Shape;

    // define an oscillator and its input names
    template <typename Amp, typename Freq, typename Ph>
    using osc_t = decltype(
        processor<dsp::Oscillator<scalar_t>>::with_inputs<Amp, Freq, Ph, samplerate_t, shape_t>::
            named("gain"_s, "frequency"_s, "phase"_s, "samplerate"_s, "shape"_s));

    // oscillator where gain, frequency and phase are smoothed controls
    using control_osc_t = osc_t<control_t, control_t, control_t>;
    // modulated control: the result of adding a control with an oscillator
    using mod_control_t = decltype(control_t{} + control_osc_t{});
    // an fm oscillator
    using fm_t = osc_t<control_t, mod_control_t, control_t>;

public:
    using type  = osc_t<control_t, mod_control_t, control_t>;
    using shape = shape_t;
};

int main()
{
    fm::type fm;
    samplerate_t samplerate = 48000.0f;
    scalar_t c              = 440.0f; // carrier frequency
    scalar_t m              = 110.0f; // modulating frequency
    scalar_t I              = 0.5f;   // modulation index
    scalar_t gain           = 0.5f;   // output gain

    fm.input("gain"_s).input("value"_s)                   = gain;
    fm.input("frequency"_s).input("x"_s).input("value"_s) = c;
    fm.input("phase"_s).input("value"_s)                  = 0.0f;
    fm.input("samplerate"_s)                              = samplerate;
    fm.input("shape"_s)                                   = fm::shape::Sine;

    auto& modulator                                 = fm.input("frequency"_s).input("y"_s);
    modulator.input("gain"_s).input("value"_s)      = m * I;
    modulator.input("frequency"_s).input("value"_s) = m;
    modulator.input("phase"_s).input("value"_s)     = 0.0f;
    modulator.input("samplerate"_s)                 = samplerate;
    modulator.input("shape"_s)                      = fm::shape::Sine;

    while (true)
        fm();

    return 0;
}
