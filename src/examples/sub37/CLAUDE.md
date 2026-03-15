# Sub37 — Moog Sub 37 Synthesizer Example

## Overview

A subtractive synthesizer modeled on the Moog Sub 37, using the DAP compile-time graph system. Unlike the complex_fm example (which uses FM synthesis), this is a classic subtractive architecture: multiple oscillators → mixer → Moog ladder filter → output.

## Signal Flow

```
filter(ampEnv * mixer(osc1, osc2, sub, noise), filterCutoff, resonance, sr)
```

Where:
- `filterCutoff = base + filterEnv * amount + filterLFO`
- `oscFreq = glide(noteFreq) + pitchLFO`

## Graph Type Hierarchy

```
type = filter_t<osc_output_t>
  = LadderFilter<signal, frequency, resonance, samplerate>

osc_output_t = envelope_t * osc_mixer_t
osc_mixer_t  = mixer_t<osc_t, osc_t, osc_t, noise_t>
                        osc1   osc2   sub    noise

osc_t = Oscillator<control_t, osc_freq_t, control_t, sr, shape>
osc_freq_t = glide_t + lfo_t     (portamento + pitch LFO)

filter_cutoff_t = control_t + envelope_t * control_t + lfo_t
                   base       filterEnv    amount      filterLFO
```

## Graph Navigation (from m_graph)

| Target | Path |
|--------|------|
| Amp envelope | `.input("signal"_s).input("x"_s)` |
| Mixer | `.input("signal"_s).input("y"_s)` |
| Bus N | `.input("signal"_s).input("y"_s).input<N>()` |
| Bus N signal | `...input<N>().input("signal"_s)` |
| Bus N gain | `...input<N>().input("gain"_s)` |
| Osc freq glide | `osc.input("frequency"_s).input("x"_s)` |
| Osc pitch LFO | `osc.input("frequency"_s).input("y"_s)` |
| Filter cutoff base | `.input("frequency"_s).input("x"_s).input("x"_s)` |
| Filter envelope | `.input("frequency"_s).input("x"_s).input("y"_s).input("x"_s)` |
| Filter env amount | `.input("frequency"_s).input("x"_s).input("y"_s).input("y"_s)` |
| Filter LFO | `.input("frequency"_s).input("y"_s)` |
| Filter resonance | `.input("resonance"_s)` |

## Architecture Notes

- **Tree structure**: Each oscillator has its own copy of the pitch LFO (independent instances). Use `setPitchLfo()` to set the same rate/depth on all copies.
- **Osc2 beat frequency**: Stored as `m_osc2BeatFreq`, applied additively in `setNote()`.
- **Sub oscillator**: Always tracks osc1 at half frequency (one octave below).
- **Gate**: `AmpEnvelope::setGate()` triggers both amp and filter envelopes simultaneously.
- **No hard sync**: The DAP Oscillator doesn't support hard sync. Osc2 runs independently.
- **No arpeggiator**: Omitted for simplicity.

## Mixer Bus Layout

| Bus | Source | Default Waveform |
|-----|--------|-----------------|
| 0 | Osc 1 | Saw |
| 1 | Osc 2 | Saw |
| 2 | Sub | Square |
| 3 | Noise | White |

## Build

```
cmake -S . -B build --toolchain ToolChain.cmake
cmake --build build --target sub37_synth
```

## Run

```
./build/src/examples/sub37/sub37_synth
./build/src/examples/sub37/sub37_synth --list-devices
./build/src/examples/sub37/sub37_synth --device 1
```
