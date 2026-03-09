#include "ComplexFMSynth.h"

complex_fm::Synth::Synth(size_t bufferSize, float samplerate)
: m_output(1, bufferSize)
{
    m_graph.input("signal"_s).input("signal"_s).input("y"_s).input<0>().input("gain"_s).input("value"_s) = 0.8f; // op1 gain
    m_graph.input("signal"_s).input("signal"_s).input("y"_s).input<1>().input("gain"_s).input("value"_s) = 0.6f; // op2 gain
    m_graph.input("signal"_s).input("signal"_s).input("y"_s).input<2>().input("gain"_s).input("value"_s) = 0.4f; // op3 gain

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
    signal<0>().getGainOp().input("gain"_s).input("value"_s)      = 0.2f;

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
    signal<1>().getGainOp().input("gain"_s).input("value"_s)      = 0.2f;

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
    signal<2>().getGainOp().input("gain"_s).input("value"_s)      = 0.2f;

    // Vibrato LFO (slow modulation for expression)
    signal<2>().getVibratoOp().input("samplerate"_s)                 = samplerate;
    signal<2>().getVibratoOp().input("shape"_s)                      = Graph::shape::Sine;
    signal<2>().getVibratoOp().input("phase"_s).input("value"_s)     = 0.0f;
    signal<2>().getVibratoOp().input("frequency"_s).input("value"_s) = 6.0f;
    signal<2>().getVibratoOp().input("gain"_s).input("value"_s)      = 4.0f;

    // Ladder filter: tame harsh FM harmonics
    filter().setSampleRate(samplerate);
    filter().setCutoff(2200.0f);
    filter().setResonance(0.6f);

    // Phaser: organic body movement for string character
    phaser().setSampleRate(samplerate);
    phaser().setRate(0.6f);
    phaser().setDepth(0.4f);
    phaser().setFeedback(0.25f);
    phaser().setWet(0.3f);
}
