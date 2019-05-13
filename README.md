# dap
digital algorithm processing library

# motivation
Learning and playing with meta programming. Experimenting with compile time graphs.

# supported platforms
Although the code is mostly platform independant, this project as is will only build successfully on darwin.

# example code
Following an example for creating an fm oscillator where all controls are smoothed.  Please take a look at src/examples/crtp_synth for a more complex example.

```cpp
using namespace dap;
using samplerate_t = float;

// a smoothed control
using control_t = decltype(
    processor<dsp::FixedSmoother<float, 512>>::
        with_inputs<float>::named("value"_s));

using shape_t = dsp::OscillatorFunctions::Shape;

template <typename Amp, typename Freq, typename Ph>
using osc_t =
    decltype(processor<dsp::Oscillator<float>>::
        with_inputs<Amp, Freq, Ph, samplerate_t, shape_t>::named("gain"_s,
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

fm_t fm;
samplerate_t samplerate = 48000.0f;
float c = 440.0f;  // carrier frequency
float m = 110.0f;  // modulating frequency
float I = 0.5f;    // modulation index
float gain = 0.5f; // output gain

fm.input("gain"_s).input("value"_s) = gain;
fm.input("frequency"_s).input("x"_s).input("value"_s) = c;
fm.input("phase"_s).input("value"_s) = 0.0f;
fm.input("samplerate"_s) = samplerate;
fm.input("shape"_s) = shape_t::Sine;

auto& modulator = fm.input("frequency"_s).input("y"_s);
modulator.input("gain"_s).input("value"_s) = m*I;
modulator.input("frequency"_s).input("value"_s) = m;
modulator.input("phase"_s).input("value"_s) = 0.0f;
modulator.input("samplerate"_s) = samplerate;
modulator.input("shape"_s) = shape_t::Sine;

while (true)
    fm();
```
