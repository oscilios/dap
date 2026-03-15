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

    // --- Feedback delay: off by default ---
    setDelayTime(0.0f);
    setFeedback(0.0f);
}
