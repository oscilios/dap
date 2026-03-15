# Complex FM Synth — Audio Graph

The graph is a compile-time tree of DSP nodes, evaluated sample-by-sample via `operator()`.
All smoothed parameters use `FixedSmoother<float, 512>` for glitch-free changes.

## Top-level signal flow

```mermaid
graph BT
    op0["fm_osc_t<br/>Operator 0<br/>1st harmonic (odd)"]
    op1["fm_osc_t<br/>Operator 1<br/>2nd harmonic (even)"]
    op2["fm_osc_t<br/>Operator 2<br/>3rd harmonic (odd)"]
    op3["fm_osc_t<br/>Operator 3<br/>4th harmonic (even)"]
    op4["fm_osc_t<br/>Operator 4<br/>5th harmonic (odd)"]
    noise["NoiseGenerator<br/>(gain, color)"]

    bus0["Bus 0"]
    bus1["Bus 1"]
    bus2["Bus 2"]
    bus3["Bus 3"]
    bus4["Bus 4"]
    bus5["Bus 5"]

    mixer["Mixer<br/>(normalized sum)"]
    amp_env["Amplitude Envelope<br/>(ADSR)"]
    mul(("*"))
    filter_freq["Filter Cutoff<br/>base + filterEnv * amount"]
    filter["LadderFilter<br/>(resonance)"]
    phaser["Phaser<br/>(rate, depth, feedback, wet)"]
    eq["BiquadFilter<br/>(LowShelf, freq, Q, gainDb)"]
    output(["Output"])

    op0 -->|signal| bus0
    op1 -->|signal| bus1
    op2 -->|signal| bus2
    op3 -->|signal| bus3
    op4 -->|signal| bus4
    noise -->|signal| bus5

    bus0 --> mixer
    bus1 --> mixer
    bus2 --> mixer
    bus3 --> mixer
    bus4 --> mixer
    bus5 --> mixer

    mixer --> mul
    amp_env --> mul
    mul -->|signal| filter
    filter_freq -->|frequency| filter
    filter -->|signal| phaser
    phaser -->|signal| eq
    eq --> output
```

## Per-operator structure (fm_osc_t)

Each of the 5 operators is an FM oscillator with amplitude modulation,
vibrato, and envelope-controlled FM/AM burst at note onset.

```mermaid
graph BT
    carrier["Carrier Oscillator<br/>(samplerate, shape, phase)"]

    carrier_level["Carrier Level<br/>(control_t)"]
    am_osc["AM Oscillator<br/>(rate, shape, phase)"]
    am_gain["AM Gain<br/>base + atkEnv * amount"]
    amp_add(("+"))

    portamento["Portamento<br/>(value, duration)"]
    vibrato["Vibrato LFO<br/>(rate, depth, shape)"]
    fm_osc["FM Modulator Osc<br/>(freq, shape, phase)"]
    fm_gain["FM Gain<br/>base + atkEnv * amount"]
    vib_add(("+"))
    freq_add(("+"))

    phase["Phase<br/>(control_t)"]

    carrier_level --> amp_add
    am_gain -->|gain| am_osc
    am_osc --> amp_add
    amp_add -->|"gain (amp_mod_t)"| carrier

    portamento --> vib_add
    vibrato --> vib_add
    vib_add --> freq_add
    fm_gain -->|gain| fm_osc
    fm_osc --> freq_add
    freq_add -->|"frequency (fm_vib_mod_t)"| carrier

    phase -->|phase| carrier
```

## Envelope-controlled gain (env_gain_t)

Used for both FM modulator gain and AM oscillator gain.
Provides a burst of extra modulation at note onset that decays via the attack envelope.

```mermaid
graph BT
    base["Base<br/>(control_t)<br/>steady-state value"]
    env["Attack Envelope<br/>(ADSR)<br/>fast attack, short decay"]
    amount["Amount<br/>(control_t)<br/>burst magnitude"]
    mul(("*"))
    add(("+"))
    output(["env_gain_t output"])

    env --> mul
    amount --> mul
    base --> add
    mul --> add
    add --> output
```

## Filter cutoff envelope (filter_env_freq_t)

Same structure as env_gain_t but controlling the LadderFilter cutoff frequency.

```mermaid
graph BT
    base["Base Frequency<br/>(control_t)<br/>400 Hz"]
    env["Filter Envelope<br/>(ADSR)"]
    amount["Amount<br/>(control_t)<br/>1800 Hz"]
    mul(("*"))
    add(("+"))
    output(["Filter cutoff frequency"])

    env --> mul
    amount --> mul
    base --> add
    mul --> add
    add --> output
```

## Output EQ (biquad_t)

Final-stage BiquadFilter configured as a low shelf to tame low-end rumble.
Supports all standard biquad types (LPF, HPF, BPF, Notch, Peak, LowShelf, HighShelf)
via the `Type` enum. Uses `IIRFilter<float, 3>` (Direct Form II) internally.

Default settings: LowShelf at 200 Hz, Q = 0.707, -3 dB.

## Envelope gating

All envelopes are gated together via `Envelope::setGate()`:
- **Amplitude envelope** — slow attack (1.2s) for bowed character
- **Filter envelope** — controls LadderFilter cutoff sweep
- **Attack envelopes** — 5 FM + 5 AM, fast attack (0.01s) / short decay (0.15s) for bow onset transient
