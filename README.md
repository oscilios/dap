# dap
digital algorithm processing library

I started this repository for developping a dsp library. Things turned around and ended up working on a library for compile time graphs. You should check src/examples/crtp_synth out.
This is work in progress and experimental
The code is mostly cross-platform, however I am currently working on OSX, so don't expect this code to compile in other platforms. Mainly because of coreaudio and very few platform dependant classes.

Following an example for creating an fm oscillator where all controls are smoothed:

```cpp
using namespace dap;
using samplerate_t = float;

// a smoothed control
using control_t = decltype(
    make_node::processor<dsp::FixedSmoother<float, 512>>::
        with_inputs<float>::named("value"_s));

using osc_shape_t = dsp::OscillatorFunctions::Shape;

template <typename Amp, typename Freq, typename Ph>
using osc_t =
    decltype(make_node::processor<dsp::Oscillator<float>>::
        with_inputs<Amp, Freq, Ph, samplerate_t, osc_shape_t>::named("gain"_s,
                                                                     "frequency"_s,
                                                                     "phase"_s,
                                                                     "samplerate"_s,
                                                                     "shape"_s));

// oscillator where gain, frequency and phase are smoothed controls
using control_osc_t = osc_t<control_t, control_t, control_t>;
// modulated control: the result of adding a control with an oscillator
using mod_control_t = decltype(control_osc_t{} + control_t{});
// an fm oscillator
using fm_t = osc_t<control_t, mod_control_t, control_t>;

fm_t fm_osc;
osc.input("gain"_s).input("value"_s) = 1.0f;
osc.input("frequency"_s).input("x"_s).input("value"_s) = 440.0f; // carrier frequency
osc.input("phase"_s).input("value"_s) = 0.0f;
osc.input("samplerate"_s) = 48000.0f;
osc.input("shape"_s) = osc_shape_t::Sine;


// modulation
auto& modulator = osc.input("frequency"_s).input("y"_s);
modulator.input("gain"_s).input("value"_s) = 50.0f;
modulator.input("frequency"_s).input("value"_s) = 110.0f;
modulator.input("phase"_s).input("value"_s) = 0.0f;
modulator.input("samplerate"_s) = 48000.0f;
modulator.input("shape"_s) = osc_shape_t::Sine;

while (true)
    fm_osc();
```
