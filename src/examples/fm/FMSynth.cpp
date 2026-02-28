#include "FMSynth.h"
#include <cmath>

fm::Synth::Synth(size_t bufferSize, float samplerate)
: m_output(1, bufferSize, 0.0f)
{
    m_graph.input("samplerate"_s)     = samplerate;
    modulator().input("samplerate"_s) = samplerate;

    m_graph.input("shape"_s)     = Graph::shape::Sine;
    modulator().input("shape"_s) = Graph::shape::Sine;

    m_graph.input("phase"_s).input("value"_s)     = 0.0f;
    modulator().input("phase"_s).input("value"_s) = 0.0f;
}
