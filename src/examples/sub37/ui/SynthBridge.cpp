#include "SynthBridge.h"
#include "AudioProcess.h"
#include "Sub37Synth.h"
#include "audioio/AudioDeviceList.h"
#include "dsp/NoiseGenerator.h"
#include "dsp/OscillatorFunctions.h"
#include <QTimer>
#include <cmath>

SynthBridge::SynthBridge(QObject* parent)
: QObject(parent)
{
}

void SynthBridge::setDeviceIndex(int idx)
{
    m_deviceIndex = idx;
    if (m_process)
        m_process->stop();
    m_process.reset();
    m_synth = nullptr;
    initAudioDevice(idx);
}

void SynthBridge::initAudioDevice(int deviceIdx)
{
    const auto devices = dap::audioio::AudioDeviceList::create(dap::audioio::Scope::Output);
    if (devices.empty())
        return;

    auto idx =
        static_cast<std::size_t>(std::clamp(deviceIdx, 0, static_cast<int>(devices.size()) - 1));
    auto* dev    = devices[idx].get();
    m_sampleRate = dev->getSampleRate();
    m_process    = std::make_unique<sub37::AudioProcess>(
        dev->getId(), dev->getChannelCount(), dev->getBufferSize(), m_sampleRate);
    m_synth = &m_process->getSynth();

    // Apply cached parameter values to the new synth instance
    using Shape = dap::dsp::OscillatorFunctions::Shape;

    m_synth->osc1().setShape(static_cast<Shape>(m_osc1Shape));
    m_synth->osc1().setBusLevel(m_osc1Level);
    m_synth->osc2().setShape(static_cast<Shape>(m_osc2Shape));
    m_synth->osc2().setBusLevel(m_osc2Level);
    m_synth->setOsc2BeatFreq(m_osc2BeatFreq);
    m_synth->setBusGain<2>(m_subLevel);

    m_synth->noise().setGain(m_noiseGain);
    {
        using Color = dap::dsp::NoiseGenerator<dap::dsp::UniformDistribution>::Color;
        m_synth->noise().setColor(static_cast<Color>(m_noiseColor));
    }

    m_synth->envelope().setAttack(m_ampAttack);
    m_synth->envelope().setDecay(m_ampDecay);
    m_synth->envelope().setSustain(m_ampSustain);
    m_synth->envelope().setRelease(m_ampRelease);
    m_synth->envelope().setSampleRate(m_sampleRate);

    m_synth->filterEnvelope().setBase(m_filterCutoff);
    m_synth->filter().setResonance(m_filterResonance);
    m_synth->filterEnvelope().setAmount(m_filterEnvAmount);
    m_synth->filterEnvelope().setAttack(m_filterAttack);
    m_synth->filterEnvelope().setDecay(m_filterDecay);
    m_synth->filterEnvelope().setSustain(m_filterSustain);
    m_synth->filterEnvelope().setRelease(m_filterRelease);

    m_synth->setPitchLfo(m_pitchLfoRate, m_pitchLfoDepth, static_cast<Shape>(m_pitchLfoShape));
    m_synth->setPitchLfo2(m_pitchLfo2Rate, m_pitchLfo2Depth, static_cast<Shape>(m_pitchLfo2Shape));
    m_synth->filter().setLfoRate(m_filterLfoRate);
    m_synth->filter().setLfoDepth(m_filterLfoDepth);
    m_synth->filter().setLfoShape(static_cast<Shape>(m_filterLfoShape));

    m_synth->setDelayTime(m_delayTime * m_sampleRate);
    m_synth->setFeedback(m_feedback);

    auto glideSamples = static_cast<size_t>(m_glideTime * m_sampleRate);
    m_synth->setGlide(glideSamples);
}

SynthBridge::~SynthBridge()
{
    if (m_process)
        m_process->stop();
}

bool SynthBridge::startAudio()
{
    return m_process && m_process->start();
}

bool SynthBridge::stopAudio()
{
    return m_process && m_process->stop();
}

void SynthBridge::updateOsc2Freq()
{
    if (!m_synth)
        return;
    float octaveMul = std::pow(2.0f, static_cast<float>(m_osc2Octave));
    m_synth->osc2().setFreq(m_fundamental * octaveMul + m_osc2BeatFreq);
}

void SynthBridge::noteOn(int midiNote)
{
    if (!m_synth)
        return;
    m_fundamental = 440.0f * std::pow(2.0f, (midiNote - 69) / 12.0f);
    m_synth->setNote(m_fundamental);
    updateOsc2Freq();

    if (m_noteActive)
    {
        m_synth->envelope().setGate(0.0f);
        QTimer::singleShot(5,
                           this,
                           [this]()
                           {
                               if (m_synth && m_noteActive)
                                   m_synth->envelope().setGate(1.0f);
                           });
    }
    else
    {
        m_synth->envelope().setGate(1.0f);
    }
    m_noteActive = true;
}

void SynthBridge::noteOff()
{
    if (!m_synth)
        return;
    m_noteActive = false;
    m_synth->envelope().setGate(0.0f);
}

// --- Setters using macros ---

#define IMPL_FLOAT_SETTER(NAME, MEMBER, SYNTH_CALL, SIGNAL) \
    void SynthBridge::set##NAME(float v)                    \
    {                                                       \
        if (qFuzzyCompare(MEMBER, v))                       \
            return;                                         \
        MEMBER = v;                                         \
        if (m_synth)                                        \
            m_synth->SYNTH_CALL;                            \
        emit SIGNAL();                                      \
    }

IMPL_FLOAT_SETTER(Osc1Level, m_osc1Level, osc1().setBusLevel(v), osc1LevelChanged)
IMPL_FLOAT_SETTER(Osc2Level, m_osc2Level, osc2().setBusLevel(v), osc2LevelChanged)
IMPL_FLOAT_SETTER(SubLevel, m_subLevel, setBusGain<2>(v), subLevelChanged)

IMPL_FLOAT_SETTER(NoiseGain, m_noiseGain, noise().setGain(v), noiseGainChanged)

IMPL_FLOAT_SETTER(AmpAttack, m_ampAttack, envelope().setAttack(v), ampAttackChanged)
IMPL_FLOAT_SETTER(AmpDecay, m_ampDecay, envelope().setDecay(v), ampDecayChanged)
IMPL_FLOAT_SETTER(AmpSustain, m_ampSustain, envelope().setSustain(v), ampSustainChanged)
IMPL_FLOAT_SETTER(AmpRelease, m_ampRelease, envelope().setRelease(v), ampReleaseChanged)

IMPL_FLOAT_SETTER(FilterCutoff, m_filterCutoff, filterEnvelope().setBase(v), filterCutoffChanged)
IMPL_FLOAT_SETTER(FilterResonance,
                  m_filterResonance,
                  filter().setResonance(v),
                  filterResonanceChanged)
IMPL_FLOAT_SETTER(FilterEnvAmount,
                  m_filterEnvAmount,
                  filterEnvelope().setAmount(v),
                  filterEnvAmountChanged)

IMPL_FLOAT_SETTER(FilterAttack, m_filterAttack, filterEnvelope().setAttack(v), filterAttackChanged)
IMPL_FLOAT_SETTER(FilterDecay, m_filterDecay, filterEnvelope().setDecay(v), filterDecayChanged)
IMPL_FLOAT_SETTER(FilterSustain,
                  m_filterSustain,
                  filterEnvelope().setSustain(v),
                  filterSustainChanged)
IMPL_FLOAT_SETTER(FilterRelease,
                  m_filterRelease,
                  filterEnvelope().setRelease(v),
                  filterReleaseChanged)

IMPL_FLOAT_SETTER(FilterLfoRate, m_filterLfoRate, filter().setLfoRate(v), filterLfoRateChanged)
IMPL_FLOAT_SETTER(FilterLfoDepth, m_filterLfoDepth, filter().setLfoDepth(v), filterLfoDepthChanged)

#undef IMPL_FLOAT_SETTER

// Pitch LFO setters — must update all oscillator copies
void SynthBridge::setPitchLfoRate(float v)
{
    if (qFuzzyCompare(m_pitchLfoRate, v))
        return;
    m_pitchLfoRate = v;
    if (m_synth)
        m_synth->setPitchLfo(m_pitchLfoRate, m_pitchLfoDepth);
    emit pitchLfoRateChanged();
}

void SynthBridge::setPitchLfoDepth(float v)
{
    if (qFuzzyCompare(m_pitchLfoDepth, v))
        return;
    m_pitchLfoDepth = v;
    if (m_synth)
        m_synth->setPitchLfo(m_pitchLfoRate, m_pitchLfoDepth);
    emit pitchLfoDepthChanged();
}

void SynthBridge::setPitchLfoShape(int v)
{
    if (m_pitchLfoShape == v)
        return;
    m_pitchLfoShape = v;
    if (m_synth)
    {
        using Shape = dap::dsp::OscillatorFunctions::Shape;
        m_synth->setPitchLfo(m_pitchLfoRate, m_pitchLfoDepth, static_cast<Shape>(v));
    }
    emit pitchLfoShapeChanged();
}

// Pitch LFO 2 setters — osc2 only
void SynthBridge::setPitchLfo2Rate(float v)
{
    if (qFuzzyCompare(m_pitchLfo2Rate, v))
        return;
    m_pitchLfo2Rate = v;
    if (m_synth)
        m_synth->setPitchLfo2(m_pitchLfo2Rate, m_pitchLfo2Depth);
    emit pitchLfo2RateChanged();
}

void SynthBridge::setPitchLfo2Depth(float v)
{
    if (qFuzzyCompare(m_pitchLfo2Depth, v))
        return;
    m_pitchLfo2Depth = v;
    if (m_synth)
        m_synth->setPitchLfo2(m_pitchLfo2Rate, m_pitchLfo2Depth);
    emit pitchLfo2DepthChanged();
}

void SynthBridge::setPitchLfo2Shape(int v)
{
    if (m_pitchLfo2Shape == v)
        return;
    m_pitchLfo2Shape = v;
    if (m_synth)
    {
        using Shape = dap::dsp::OscillatorFunctions::Shape;
        m_synth->setPitchLfo2(m_pitchLfo2Rate, m_pitchLfo2Depth, static_cast<Shape>(v));
    }
    emit pitchLfo2ShapeChanged();
}

// Osc2 beat frequency
void SynthBridge::setOsc2BeatFreq(float v)
{
    if (qFuzzyCompare(m_osc2BeatFreq, v))
        return;
    m_osc2BeatFreq = v;
    if (m_synth)
    {
        m_synth->setOsc2BeatFreq(v);
        updateOsc2Freq();
    }
    emit osc2BeatFreqChanged();
}

// Osc2 octave offset
void SynthBridge::setOsc2Octave(int v)
{
    if (m_osc2Octave == v)
        return;
    m_osc2Octave = v;
    updateOsc2Freq();
    emit osc2OctaveChanged();
}

// Shape setters (int, not float)
void SynthBridge::setOsc1Shape(int v)
{
    if (m_osc1Shape == v)
        return;
    m_osc1Shape = v;
    if (m_synth)
    {
        using Shape = dap::dsp::OscillatorFunctions::Shape;
        m_synth->osc1().setShape(static_cast<Shape>(v));
    }
    emit osc1ShapeChanged();
}

void SynthBridge::setOsc2Shape(int v)
{
    if (m_osc2Shape == v)
        return;
    m_osc2Shape = v;
    if (m_synth)
    {
        using Shape = dap::dsp::OscillatorFunctions::Shape;
        m_synth->osc2().setShape(static_cast<Shape>(v));
    }
    emit osc2ShapeChanged();
}

void SynthBridge::setNoiseColor(int v)
{
    if (m_noiseColor == v)
        return;
    m_noiseColor = v;
    if (m_synth)
    {
        using Color = dap::dsp::NoiseGenerator<dap::dsp::UniformDistribution>::Color;
        m_synth->noise().setColor(static_cast<Color>(v));
    }
    emit noiseColorChanged();
}

void SynthBridge::setFilterLfoShape(int v)
{
    if (m_filterLfoShape == v)
        return;
    m_filterLfoShape = v;
    if (m_synth)
    {
        using Shape = dap::dsp::OscillatorFunctions::Shape;
        m_synth->filter().setLfoShape(static_cast<Shape>(v));
    }
    emit filterLfoShapeChanged();
}

// Feedback delay (seconds → samples)
void SynthBridge::setDelayTime(float v)
{
    if (qFuzzyCompare(m_delayTime, v))
        return;
    m_delayTime = v;
    if (m_synth)
        m_synth->setDelayTime(v * m_sampleRate);
    emit delayTimeChanged();
}

void SynthBridge::setFeedback(float v)
{
    if (qFuzzyCompare(m_feedback, v))
        return;
    m_feedback = v;
    if (m_synth)
        m_synth->setFeedback(v);
    emit feedbackChanged();
}

// Glide time (seconds → samples)
void SynthBridge::setGlideTime(float v)
{
    if (qFuzzyCompare(m_glideTime, v))
        return;
    m_glideTime = v;
    if (m_synth)
    {
        auto samples = static_cast<size_t>(v * m_sampleRate);
        m_synth->setGlide(samples);
    }
    emit glideTimeChanged();
}
