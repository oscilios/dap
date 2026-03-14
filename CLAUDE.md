# DAP — Digital Algorithm Processing Library

## Overview

A C++ metaprogramming library for building audio DSP graphs at compile time. Nodes are composed using `decltype` expressions and evaluated sample-by-sample via `operator()`.

## Build

```
cmake -S . -B build --toolchain ToolChain.cmake
cmake --build build
```

Platform: macOS (darwin) only. ARM64 supported.

## Project Structure

```
src/
  base/          — utilities: KeyValueTuple, TypeTraits, constexpr_string
  fastmath/      — AudioBuffer, Variable, SIMD helpers
  crtp/
    nodes/       — core graph node system
      Node.h           — NodeExpression base, input() accessors, BinaryNode
      Processor.h      — ProcessorNode wraps DSP classes as graph nodes
      private/
        BinaryNodeOpsImpl.hpp — operator+, *, - between nodes
  dsp/           — DSP processors (stateless, wrapped by ProcessorNode)
    Oscillator.h       — sine/saw/square oscillator
    Smoother.h         — FixedSmoother<T, N> (N-sample smoothing), Smoother<T> (variable)
    EnvelopeGenerator.h — ADSR envelope
    LadderFilter.h     — 4-pole ladder lowpass filter
    Phaser.h           — 6-stage allpass phaser with LFO
    NoiseGenerator.h   — white/pink/brown/1/f^3 noise
    Mixer.h            — Mixer::Bus + normalized Mixer
    CombFilter.h       — feedback comb / flanger
  audioio/       — platform audio I/O (CoreAudio on macOS)
  osc/           — OSC protocol support
  threadsafe/    — lock-free utilities
  examples/
    fm/          — minimal FM oscillator example
    crtp_synth/  — full synth with OSC control, 6 oscillators, filter, phaser
    complex_fm/  — bowed string FM synth (5 operators + noise + envelopes)
```

## Core Concepts

### Compile-time graph (CRTP nodes)

Graphs are built using type-level composition. Each node type is defined via `decltype`:

```cpp
using control_t = decltype(
    processor<dsp::FixedSmoother<float, 512>>::with_inputs<float>::named("value"_s));

using osc_t = decltype(
    processor<dsp::Oscillator<float>>::with_inputs<Amp, Freq, Ph, float, Shape>::named(
        "gain"_s, "frequency"_s, "phase"_s, "samplerate"_s, "shape"_s));
```

Nodes combine with arithmetic operators to form trees:

```cpp
using fm_t = decltype(control_t{} + osc_t{});     // BinaryNode<Add, ...>
using env_t = decltype(envelope_t{} * mixer_t{});  // BinaryNode<Multiply, ...>
```

**Important**: The graph is a tree, not a DAG. Each `decltype(...)` creates independent node instances. Nodes cannot be shared between multiple consumers.

### Accessing node inputs

Named inputs use `constexpr_string` literals:

```cpp
node.input("frequency"_s).input("value"_s) = 440.0f;
```

Binary node children are accessed via `input("x"_s)` (left) and `input("y"_s)` (right).
Mixer buses use `input<N>()` (template index).

### Processing

Nodes are evaluated one sample at a time:

```cpp
for (auto& x : buffer.channel(0)) {
    x = graph();  // calls operator()
}
```

## Examples

### complex_fm (bowed string synth)

See `src/examples/complex_fm/graph.md` for full graph diagrams.

Key files:
- `ComplexFMSynth.h` — graph type definitions and accessor classes
- `ComplexFMSynth.cpp` — constructor initialization
- `main.cpp` — parameter setup and melody playback
- `AudioProcess.h/cpp` — audio device callback wrapper

Graph structure: `phaser(filter(ampEnvelope * mixer(5 FM operators + noise)))`

Features:
- 5 FM operators with odd harmonic emphasis (1st, 3rd, 5th boosted)
- Amplitude envelope (slow attack for bowed character)
- LadderFilter with envelope-controlled cutoff (`base + filterEnv * amount`)
- Attack envelope controlling both FM and AM burst at note onset (`env_gain_t`)
- Vibrato swell at note onset (parameter automation in playNote)
- Pink/1/f^3 noise as 6th mixer bus
- Phaser for body resonance

### crtp_synth (OSC-controlled synth)

Key files:
- `Types.h` — all graph type aliases
- `Synth.h` — graph with `KeyValueTuple` for OSC parameter mapping
- `Synth.cpp` — initialization

Graph: `phaser(filter(mixer(2 AM/FM oscillators + noise)))`

## Common Patterns

### Envelope-controlled parameter (env_gain_t)

```cpp
using env_gain_t = decltype(control_t{} + envelope_t{} * control_t{});
// output = base + envelope * amount
```

Used for filter cutoff, FM modulation depth, AM modulation depth.

### AudioProcess wrapper

Each example uses an `AudioProcess` class that:
1. Opens an audio device via `audioio`
2. Creates a `Synth` with the device's buffer size and sample rate
3. Calls `synth.process()` in the audio callback
4. Copies `synth.output()` to all output channels

## Code Style

After editing any C++ file, run clang-format:

```
clang-format -i <file>
```

The repo has a `.clang-format` config at the root. Always format before committing.

## Known Issues

- `Phaser.h`: `m_output` must be zero-initialized (`T m_output{0}`) to prevent NaN feedback propagation. This was patched locally.
