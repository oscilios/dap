#include "Sub37Synth.h"

sub37::Synth::Synth(size_t bufferSize, float samplerate)
: m_output(1, bufferSize)
{
    // --- Osc 1: Saw wave ---
    osc1().setGain(1.0f);
    osc1().setBusLevel(0.7f);
    osc1().setShape(Graph::shape::Saw);
    osc1().setFreq(110.0f);
    osc1().setGlideDuration(0);
    osc1().setSampleRate(samplerate);
    osc1().setPhase(0.0f);
    osc1().setPitchLfoRate(5.0f);
    osc1().setPitchLfoDepth(0.0f);
    osc1().setPitchLfoShape(Graph::shape::Triangle);
    osc1().setPitchLfoSampleRate(samplerate);
    osc1().setPitchLfoPhase(0.0f);

    // --- Osc 2: Saw wave, slight detune ---
    osc2().setGain(1.0f);
    osc2().setBusLevel(0.5f);
    osc2().setShape(Graph::shape::Saw);
    osc2().setFreq(110.0f);
    osc2().setGlideDuration(0);
    osc2().setSampleRate(samplerate);
    osc2().setPhase(0.0f);
    osc2().setPitchLfoRate(5.0f);
    osc2().setPitchLfoDepth(0.0f);
    osc2().setPitchLfoShape(Graph::shape::Triangle);
    osc2().setPitchLfoSampleRate(samplerate);
    osc2().setPitchLfoPhase(0.0f);
    m_osc2BeatFreq = 2.0f; // 2 Hz detune

    // --- Sub oscillator: Square, one octave below ---
    subOsc().setGain(1.0f);
    subOsc().setBusLevel(0.6f);
    subOsc().setShape(Graph::shape::Square);
    subOsc().setFreq(55.0f);
    subOsc().setGlideDuration(0);
    subOsc().setSampleRate(samplerate);
    subOsc().setPhase(0.0f);
    subOsc().setPitchLfoRate(5.0f);
    subOsc().setPitchLfoDepth(0.0f);
    subOsc().setPitchLfoShape(Graph::shape::Triangle);
    subOsc().setPitchLfoSampleRate(samplerate);
    subOsc().setPitchLfoPhase(0.0f);

    // --- PWM: off by default ---
    pwm().setGain(1.0f);
    pwm().setBusLevel(0.0f);
    pwm().setFreq(110.0f);
    pwm().setGlideDuration(0);
    pwm().setSampleRate(samplerate);
    pwm().setPhase(0.0f);
    pwm().setDutyCycle(0.5f);
    pwm().setDcLfoRate(1.0f);
    pwm().setDcLfoDepth(0.0f);
    pwm().setDcLfoShape(Graph::shape::Triangle);
    pwm().setDcLfoSampleRate(samplerate);
    pwm().setDcLfoPhase(0.0f);
    pwm().setPitchLfoRate(5.0f);
    pwm().setPitchLfoDepth(0.0f);
    pwm().setPitchLfoShape(Graph::shape::Triangle);
    pwm().setPitchLfoSampleRate(samplerate);
    pwm().setPitchLfoPhase(0.0f);

    // --- Noise: off by default ---
    setBusGain<3>(1.0f);
    noise().setGain(0.0f);
    noise().setColor(Graph::noise_gen_t::Color::White);

    // --- Amp envelope: punchy bass ---
    envelope().setSampleRate(samplerate);
    envelope().setAttack(0.005f);
    envelope().setDecay(0.3f);
    envelope().setSustain(0.7f);
    envelope().setRelease(0.15f);

    // --- Filter: Moog ladder, classic bass ---
    filter().setResonance(0.4f);
    filter().setSampleRate(samplerate);
    filter().setLfoRate(0.5f);
    filter().setLfoDepth(0.0f);
    filter().setLfoShape(Graph::shape::Triangle);
    filter().setLfoSampleRate(samplerate);
    filter().setLfoPhase(0.0f);

    // --- Filter envelope: snappy for bass ---
    filterEnvelope().setSampleRate(samplerate);
    filterEnvelope().setBase(800.0f);
    filterEnvelope().setAmount(3000.0f);
    filterEnvelope().setAttack(0.001f);
    filterEnvelope().setDecay(0.4f);
    filterEnvelope().setSustain(0.0f);
    filterEnvelope().setRelease(0.2f);

    // --- Pitch envelope: off by default ---
    pitchEnvelope().setSampleRate(samplerate);
    pitchEnvelope().setAttack(0.01f);
    pitchEnvelope().setDecay(0.3f);
    pitchEnvelope().setSustain(0.0f);
    pitchEnvelope().setRelease(0.1f);
    pitchEnvelope().setAmount(0.0f);

    // Initialize sub + pwm pitch envelopes (amount=0, no effect)
    {
        auto& mixer = filterNode().input("signal"_s).input("y"_s);
        // Sub (bus 2) pitch envelope
        auto& subPitchEnv =
            mixer.template input<2>().input("signal"_s).input("frequency"_s).input("y"_s);
        subPitchEnv.input("x"_s).input("samplerate"_s) = samplerate;
        subPitchEnv.input("y"_s).input("value"_s)      = 0.0f;
        // PWM (bus 4) pitch envelope
        auto& pwmPitchEnv =
            mixer.template input<4>().input("signal"_s).input("frequency"_s).input("y"_s);
        pwmPitchEnv.input("x"_s).input("samplerate"_s) = samplerate;
        pwmPitchEnv.input("y"_s).input("value"_s)      = 0.0f;
    }

    // --- Distortion: off by default (mix=0 → dry passthrough) ---
    distortion().setDrive(1.0f);
    distortion().setMix(0.0f);
    distortion().setParam(0.8f);
    distortion().setType(dap::dsp::Distortion::Type::SoftClip);

    // --- Flanger: off by default (wet=0 → dry passthrough) ---
    flanger().setDelay(200.0f); // ~4.5 ms at 44.1kHz
    flanger().setDepth(100.0f); // ~2.3 ms sweep
    flanger().setRate(0.5f);    // 0.5 Hz LFO
    flanger().setFeedback(0.7f);
    flanger().setWet(0.0f);
    flanger().setSampleRate(samplerate);

    // --- Feedback delay: off by default ---
    setDelayTime(0.0f);
    setFeedback(0.0f);
}
