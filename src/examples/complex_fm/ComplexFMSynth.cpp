#include "ComplexFMSynth.h"

complex_fm::Synth::Synth(size_t bufferSize, float samplerate)
: m_output(1, bufferSize)
{
    m_graph.input("signal"_s).input("signal"_s).input("signal"_s).input("y"_s).input<0>().input("gain"_s).input("value"_s) = 0.7f;  // 1st harmonic (odd)
    m_graph.input("signal"_s).input("signal"_s).input("signal"_s).input("y"_s).input<1>().input("gain"_s).input("value"_s) = 0.2f;  // 2nd harmonic (even, reduced)
    m_graph.input("signal"_s).input("signal"_s).input("signal"_s).input("y"_s).input<2>().input("gain"_s).input("value"_s) = 0.4f;  // 3rd harmonic (odd, boosted)
    m_graph.input("signal"_s).input("signal"_s).input("signal"_s).input("y"_s).input<3>().input("gain"_s).input("value"_s) = 0.1f;  // 4th harmonic (even, reduced)
    m_graph.input("signal"_s).input("signal"_s).input("signal"_s).input("y"_s).input<4>().input("gain"_s).input("value"_s) = 0.15f; // 5th harmonic (odd)
    m_graph.input("signal"_s).input("signal"_s).input("signal"_s).input("y"_s).input<5>().input("gain"_s).input("value"_s) = 0.5f; // noise bus gain

    signal<0>().getFreqPortamentoDuration() = 512;
    signal<1>().getFreqPortamentoDuration() = 512;
    signal<2>().getFreqPortamentoDuration() = 512;

    signal<0>().getFreq() = 220.0f;
    signal<0>().setCarrierLevel(0.8f);
    signal<0>().setModFreq(signal<0>().getFreq() * 2.0f);
    signal<0>().setModIdx(5.0f);

    // Carrier
    signal<0>().setSampleRate(samplerate);
    signal<0>().setShape(Graph::shape::Sine);
    signal<0>().setPhase(0.0f);

    // Modulator
    signal<0>().getFreqOp().input("samplerate"_s)             = samplerate;
    signal<0>().getFreqOp().input("shape"_s)                  = Graph::shape::Sine;
    signal<0>().getFreqOp().input("phase"_s).input("value"_s) = 0.0f;

    signal<0>().getGainOp().input("samplerate"_s)                 = samplerate;
    signal<0>().getGainOp().input("shape"_s)                      = Graph::shape::Sine;
    signal<0>().getGainOp().input("phase"_s).input("value"_s)     = 0.0f;
    signal<0>().getGainOp().input("frequency"_s).input("value"_s) = 0.3f;
    signal<0>().getGainOp().input("gain"_s).input("x"_s).input("value"_s)      = 0.2f;

    // Vibrato LFO (slow modulation for expression)
    signal<0>().getVibratoOp().input("samplerate"_s)                 = samplerate;
    signal<0>().getVibratoOp().input("shape"_s)                      = Graph::shape::Sine;
    signal<0>().getVibratoOp().input("phase"_s).input("value"_s)     = 0.0f;
    signal<0>().getVibratoOp().input("frequency"_s).input("value"_s) = 6.0f;
    signal<0>().getVibratoOp().input("gain"_s).input("value"_s)      = 4.0f;

    // Operator 2
    signal<1>().getFreq() = 220.0f;
    signal<1>().setCarrierLevel(0.8f);
    signal<1>().setModFreq(signal<1>().getFreq() * 2.0f);
    signal<1>().setModIdx(5.0f);

    // Carrier
    signal<1>().setSampleRate(samplerate);
    signal<1>().setShape(Graph::shape::Sine);
    signal<1>().setPhase(0.0f);

    // Modulator
    signal<1>().getFreqOp().input("samplerate"_s)             = samplerate;
    signal<1>().getFreqOp().input("shape"_s)                  = Graph::shape::Sine;
    signal<1>().getFreqOp().input("phase"_s).input("value"_s) = 0.0f;

    signal<1>().getGainOp().input("samplerate"_s)                 = samplerate;
    signal<1>().getGainOp().input("shape"_s)                      = Graph::shape::Sine;
    signal<1>().getGainOp().input("phase"_s).input("value"_s)     = 0.0f;
    signal<1>().getGainOp().input("frequency"_s).input("value"_s) = 0.3f;
    signal<1>().getGainOp().input("gain"_s).input("x"_s).input("value"_s)      = 0.2f;

    // Vibrato LFO (slow modulation for expression)
    signal<1>().getVibratoOp().input("samplerate"_s)                 = samplerate;
    signal<1>().getVibratoOp().input("shape"_s)                      = Graph::shape::Sine;
    signal<1>().getVibratoOp().input("phase"_s).input("value"_s)     = 0.0f;
    signal<1>().getVibratoOp().input("frequency"_s).input("value"_s) = 6.0f;
    signal<1>().getVibratoOp().input("gain"_s).input("value"_s)      = 4.0f;

    // Operator 3
    signal<2>().getFreq() = 220.0f;
    signal<2>().setCarrierLevel(0.8f);
    signal<2>().setModFreq(signal<2>().getFreq() * 2.0f);
    signal<2>().setModIdx(5.0f);

    // Carrier
    signal<2>().setSampleRate(samplerate);
    signal<2>().setShape(Graph::shape::Sine);
    signal<2>().setPhase(0.0f);

    // Modulator
    signal<2>().getFreqOp().input("samplerate"_s)             = samplerate;
    signal<2>().getFreqOp().input("shape"_s)                  = Graph::shape::Sine;
    signal<2>().getFreqOp().input("phase"_s).input("value"_s) = 0.0f;

    signal<2>().getGainOp().input("samplerate"_s)                 = samplerate;
    signal<2>().getGainOp().input("shape"_s)                      = Graph::shape::Sine;
    signal<2>().getGainOp().input("phase"_s).input("value"_s)     = 0.0f;
    signal<2>().getGainOp().input("frequency"_s).input("value"_s) = 0.3f;
    signal<2>().getGainOp().input("gain"_s).input("x"_s).input("value"_s)      = 0.2f;

    // Vibrato LFO (slow modulation for expression)
    signal<2>().getVibratoOp().input("samplerate"_s)                 = samplerate;
    signal<2>().getVibratoOp().input("shape"_s)                      = Graph::shape::Sine;
    signal<2>().getVibratoOp().input("phase"_s).input("value"_s)     = 0.0f;
    signal<2>().getVibratoOp().input("frequency"_s).input("value"_s) = 6.0f;
    signal<2>().getVibratoOp().input("gain"_s).input("value"_s)      = 4.0f;

    // Operator 4 (4th harmonic)
    signal<3>().getFreq() = 220.0f;
    signal<3>().setCarrierLevel(0.2f);
    signal<3>().setModFreq(signal<3>().getFreq() * 2.0f);
    signal<3>().setModIdx(5.0f);
    signal<3>().setSampleRate(samplerate);
    signal<3>().setShape(Graph::shape::Sine);
    signal<3>().setPhase(0.0f);
    signal<3>().getFreqOp().input("samplerate"_s)             = samplerate;
    signal<3>().getFreqOp().input("shape"_s)                  = Graph::shape::Sine;
    signal<3>().getFreqOp().input("phase"_s).input("value"_s) = 0.0f;
    signal<3>().getGainOp().input("samplerate"_s)                 = samplerate;
    signal<3>().getGainOp().input("shape"_s)                      = Graph::shape::Sine;
    signal<3>().getGainOp().input("phase"_s).input("value"_s)     = 0.0f;
    signal<3>().getGainOp().input("frequency"_s).input("value"_s) = 0.3f;
    signal<3>().getGainOp().input("gain"_s).input("x"_s).input("value"_s)      = 0.2f;
    signal<3>().getVibratoOp().input("samplerate"_s)                 = samplerate;
    signal<3>().getVibratoOp().input("shape"_s)                      = Graph::shape::Sine;
    signal<3>().getVibratoOp().input("phase"_s).input("value"_s)     = 0.0f;
    signal<3>().getVibratoOp().input("frequency"_s).input("value"_s) = 6.0f;
    signal<3>().getVibratoOp().input("gain"_s).input("value"_s)      = 4.0f;
    signal<3>().getFreqPortamentoDuration()                           = 512;

    // Operator 5 (5th harmonic)
    signal<4>().getFreq() = 220.0f;
    signal<4>().setCarrierLevel(0.15f);
    signal<4>().setModFreq(signal<4>().getFreq() * 2.0f);
    signal<4>().setModIdx(5.0f);
    signal<4>().setSampleRate(samplerate);
    signal<4>().setShape(Graph::shape::Sine);
    signal<4>().setPhase(0.0f);
    signal<4>().getFreqOp().input("samplerate"_s)             = samplerate;
    signal<4>().getFreqOp().input("shape"_s)                  = Graph::shape::Sine;
    signal<4>().getFreqOp().input("phase"_s).input("value"_s) = 0.0f;
    signal<4>().getGainOp().input("samplerate"_s)                 = samplerate;
    signal<4>().getGainOp().input("shape"_s)                      = Graph::shape::Sine;
    signal<4>().getGainOp().input("phase"_s).input("value"_s)     = 0.0f;
    signal<4>().getGainOp().input("frequency"_s).input("value"_s) = 0.3f;
    signal<4>().getGainOp().input("gain"_s).input("x"_s).input("value"_s)      = 0.2f;
    signal<4>().getVibratoOp().input("samplerate"_s)                 = samplerate;
    signal<4>().getVibratoOp().input("shape"_s)                      = Graph::shape::Sine;
    signal<4>().getVibratoOp().input("phase"_s).input("value"_s)     = 0.0f;
    signal<4>().getVibratoOp().input("frequency"_s).input("value"_s) = 6.0f;
    signal<4>().getVibratoOp().input("gain"_s).input("value"_s)      = 4.0f;
    signal<4>().getFreqPortamentoDuration()                           = 512;

    // Ladder filter with envelope-controlled cutoff
    filter().setSampleRate(samplerate);
    filter().setResonance(0.6f);

    // Filter envelope: cutoff = base + filterEnv * amount
    filterEnvelope().setSampleRate(samplerate);
    filterEnvelope().setBase(400.0f);
    filterEnvelope().setAmount(2200.0f);
    filterEnvelope().setAttack(0.12f);
    filterEnvelope().setDecay(2.0f);
    filterEnvelope().setSustain(0.0f);
    filterEnvelope().setRelease(0.5f);

    // Attack envelope: shared ADSR for FM and AM burst at note onset
    attackEnvelope().setSampleRate(samplerate);
    attackEnvelope().setAttack(0.01f);
    attackEnvelope().setDecay(0.6f);
    attackEnvelope().setSustain(0.0f);
    attackEnvelope().setRelease(0.1f);

    // Per-operator FM and AM envelope amounts (0 by default, set in main)
    signal<0>().setFMEnvAmount(0.0f);
    signal<1>().setFMEnvAmount(0.0f);
    signal<2>().setFMEnvAmount(0.0f);
    signal<3>().setFMEnvAmount(0.0f);
    signal<4>().setFMEnvAmount(0.0f);
    signal<0>().setAMEnvAmount(0.0f);
    signal<1>().setAMEnvAmount(0.0f);
    signal<2>().setAMEnvAmount(0.0f);
    signal<3>().setAMEnvAmount(0.0f);
    signal<4>().setAMEnvAmount(0.0f);

    // Phaser: organic body movement for string character
    phaser().setSampleRate(samplerate);
    phaser().setRate(0.6f);
    phaser().setDepth(0.4f);
    phaser().setFeedback(0.25f);
    phaser().setWet(0.3f);

    // Noise: bow attack transient (gain starts at 0, triggered per note)
    noise().setGain(0.0f);
    noise().setColor(Graph::noise_gen_t::Color::OneOverF3);

    // Low shelf EQ: tame low-end rumble
    eq().setSampleRate(samplerate);
    eq().setType(Graph::biquad_filter_t::Type::LowShelf);
    eq().setFrequency(200.0f);
    eq().setQ(0.707f);
    eq().setGainDb(-3.0f);
}
