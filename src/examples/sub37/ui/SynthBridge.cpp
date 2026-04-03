#include "SynthBridge.h"
#include "AudioProcess.h"
#include "Sub37Synth.h"
#include "audioio/AudioDeviceList.h"
#include "dsp/NoiseGenerator.h"
#include "dsp/OscillatorFunctions.h"
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QTimer>
#include <cmath>

SynthBridge::SynthBridge(QObject* parent)
: QObject(parent)
{
    connect(&m_arpTimer, &QTimer::timeout, this, &SynthBridge::arpStep);
    connect(&m_seqTimer, &QTimer::timeout, this, &SynthBridge::seqStep);
    updateArpTimer();
    updateSeqTimer();
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

    m_synth->pwm().setBusLevel(m_pwmLevel);
    m_synth->pwm().setDutyCycle(m_pwmDutyCycle);
    m_synth->pwm().setDcLfoRate(m_pwmDcLfoRate);
    m_synth->pwm().setDcLfoDepth(m_pwmDcLfoDepth);
    {
        using Shape = dap::dsp::OscillatorFunctions::Shape;
        m_synth->pwm().setDcLfoShape(static_cast<Shape>(m_pwmDcLfoShape));
    }

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

    m_synth->pitchEnvelope().setAttack(m_pitchEnvAttack);
    m_synth->pitchEnvelope().setDecay(m_pitchEnvDecay);
    m_synth->pitchEnvelope().setSustain(m_pitchEnvSustain);
    m_synth->pitchEnvelope().setRelease(m_pitchEnvRelease);
    m_synth->pitchEnvelope().setAmount(m_pitchEnvAmount);

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

    // Assign to selected sequencer step if editing
    if (m_seqSelectedStep >= 0)
    {
        m_sequencer.setNote(m_seqSelectedStep, midiNote);
        emit seqNotesChanged();
        m_seqSelectedStep = (m_seqSelectedStep + 1) % m_sequencer.stepCount();
        emit seqSelectedStepChanged();
    }

    // If sequencer is playing, don't interfere with its playback
    if (m_seqPlaying)
        return;

    if (m_arpEnabled)
    {
        m_arpeggiator.noteOn(midiNote);
        if (!m_arpTimer.isActive() && m_arpeggiator.hasNotes())
        {
            arpStep();
            m_arpTimer.start();
        }
        return;
    }

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

void SynthBridge::noteOff(int midiNote)
{
    if (!m_synth)
        return;

    if (m_hold)
        return;

    if (m_seqPlaying)
        return;

    if (m_arpEnabled)
    {
        m_arpeggiator.noteOff(midiNote);
        if (!m_arpeggiator.hasNotes())
        {
            m_arpTimer.stop();
            m_synth->envelope().setGate(0.0f);
            m_arpCurrentNote = -1;
            emit arpCurrentNoteChanged();
        }
        return;
    }

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

IMPL_FLOAT_SETTER(PwmLevel, m_pwmLevel, pwm().setBusLevel(v), pwmLevelChanged)
IMPL_FLOAT_SETTER(PwmDutyCycle, m_pwmDutyCycle, pwm().setDutyCycle(v), pwmDutyCycleChanged)
IMPL_FLOAT_SETTER(PwmDcLfoRate, m_pwmDcLfoRate, pwm().setDcLfoRate(v), pwmDcLfoRateChanged)
IMPL_FLOAT_SETTER(PwmDcLfoDepth, m_pwmDcLfoDepth, pwm().setDcLfoDepth(v), pwmDcLfoDepthChanged)

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

IMPL_FLOAT_SETTER(PitchEnvAttack,
                  m_pitchEnvAttack,
                  pitchEnvelope().setAttack(v),
                  pitchEnvAttackChanged)
IMPL_FLOAT_SETTER(PitchEnvDecay, m_pitchEnvDecay, pitchEnvelope().setDecay(v), pitchEnvDecayChanged)
IMPL_FLOAT_SETTER(PitchEnvSustain,
                  m_pitchEnvSustain,
                  pitchEnvelope().setSustain(v),
                  pitchEnvSustainChanged)
IMPL_FLOAT_SETTER(PitchEnvRelease,
                  m_pitchEnvRelease,
                  pitchEnvelope().setRelease(v),
                  pitchEnvReleaseChanged)
IMPL_FLOAT_SETTER(PitchEnvAmount,
                  m_pitchEnvAmount,
                  pitchEnvelope().setAmount(v),
                  pitchEnvAmountChanged)

IMPL_FLOAT_SETTER(FilterLfoRate, m_filterLfoRate, filter().setLfoRate(v), filterLfoRateChanged)
IMPL_FLOAT_SETTER(FilterLfoDepth, m_filterLfoDepth, filter().setLfoDepth(v), filterLfoDepthChanged)

#undef IMPL_FLOAT_SETTER

void SynthBridge::setPwmDcLfoShape(int v)
{
    if (m_pwmDcLfoShape == v)
        return;
    m_pwmDcLfoShape = v;
    if (m_synth)
    {
        using Shape = dap::dsp::OscillatorFunctions::Shape;
        m_synth->pwm().setDcLfoShape(static_cast<Shape>(v));
    }
    emit pwmDcLfoShapeChanged();
}

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

// --- Arpeggiator ---

void SynthBridge::updateArpTimer()
{
    float stepMs = 60000.0f / m_arpBpm;
    if (m_arpSubdivision > 1)
        stepMs /= static_cast<float>(m_arpSubdivision);
    m_arpTimer.setInterval(static_cast<int>(stepMs));
}

void SynthBridge::arpStep()
{
    if (!m_synth)
        return;

    int note = m_arpeggiator.step();
    if (note < 0)
    {
        m_synth->envelope().setGate(0.0f);
        m_arpTimer.stop();
        m_arpCurrentNote = -1;
        emit arpCurrentNoteChanged();
        return;
    }

    m_arpCurrentNote = note;
    emit arpCurrentNoteChanged();

    float freq    = 440.0f * std::pow(2.0f, (note - 69) / 12.0f);
    m_fundamental = freq;
    m_synth->setNote(freq);
    updateOsc2Freq();

    // Re-trigger envelope
    m_synth->envelope().setGate(0.0f);
    QTimer::singleShot(2,
                       this,
                       [this]()
                       {
                           if (m_synth && m_arpeggiator.hasNotes())
                               m_synth->envelope().setGate(1.0f);
                       });

    // Schedule gate off based on gate length
    float stepMs = 60000.0f / m_arpBpm;
    if (m_arpSubdivision > 1)
        stepMs /= static_cast<float>(m_arpSubdivision);
    auto gateMs = static_cast<int>(stepMs * m_arpGate);

    QTimer::singleShot(gateMs,
                       this,
                       [this, note]()
                       {
                           if (m_synth && m_arpCurrentNote == note)
                               m_synth->envelope().setGate(0.0f);
                       });
}

void SynthBridge::setArpEnabled(bool v)
{
    if (m_arpEnabled == v)
        return;
    m_arpEnabled = v;
    if (!v)
    {
        m_arpTimer.stop();
        m_arpeggiator.clear();
        m_arpCurrentNote = -1;
        emit arpCurrentNoteChanged();
        if (m_synth)
            m_synth->envelope().setGate(0.0f);
    }
    emit arpEnabledChanged();
}

void SynthBridge::setHold(bool v)
{
    if (m_hold == v)
        return;
    m_hold = v;
    if (!v && m_synth)
    {
        if (m_arpEnabled && !m_arpeggiator.hasNotes())
        {
            m_arpTimer.stop();
            m_synth->envelope().setGate(0.0f);
            m_arpCurrentNote = -1;
            emit arpCurrentNoteChanged();
        }
        else if (!m_arpEnabled)
        {
            // Release sustained note when hold is turned off
            m_noteActive = false;
            m_synth->envelope().setGate(0.0f);
        }
    }
    emit holdChanged();
}

void SynthBridge::setArpBpm(float v)
{
    if (qFuzzyCompare(m_arpBpm, v))
        return;
    m_arpBpm = v;
    updateArpTimer();
    emit arpBpmChanged();
}

void SynthBridge::setArpPattern(int v)
{
    if (m_arpPattern == v)
        return;
    m_arpPattern = v;
    m_arpeggiator.setPattern(static_cast<sub37::Arpeggiator::Pattern>(v));
    emit arpPatternChanged();
}

void SynthBridge::setArpOctaveRange(int v)
{
    if (m_arpOctaveRange == v)
        return;
    m_arpOctaveRange = v;
    m_arpeggiator.setOctaveRange(v);
    emit arpOctaveRangeChanged();
}

void SynthBridge::setArpGate(float v)
{
    if (qFuzzyCompare(m_arpGate, v))
        return;
    m_arpGate = v;
    emit arpGateChanged();
}

void SynthBridge::setArpSubdivision(int v)
{
    if (m_arpSubdivision == v)
        return;
    m_arpSubdivision = v;
    updateArpTimer();
    emit arpSubdivisionChanged();
}

// --- Sequencer ---

QVariantList SynthBridge::seqNotes() const
{
    QVariantList list;
    for (int i = 0; i < sub37::Sequencer::MaxSteps; ++i)
        list.append(m_sequencer.getNote(i));
    return list;
}

void SynthBridge::seqSetNote(int step, int note)
{
    m_sequencer.setNote(step, note);
    emit seqNotesChanged();
}

void SynthBridge::seqClearStep(int step)
{
    m_sequencer.clearStep(step);
    emit seqNotesChanged();
}

void SynthBridge::seqClear()
{
    m_sequencer.clear();
    emit seqNotesChanged();
}

void SynthBridge::updateSeqTimer()
{
    float stepMs = 60000.0f / m_seqBpm;
    if (m_seqSubdivision > 1)
        stepMs /= static_cast<float>(m_seqSubdivision);
    m_seqTimer.setInterval(static_cast<int>(stepMs));
}

void SynthBridge::seqStep()
{
    if (!m_synth)
        return;

    int playingStep = m_sequencer.currentStep();
    int note        = m_sequencer.step();

    m_seqCurrentStep = playingStep;
    emit seqCurrentStepChanged();

    if (note >= 0)
    {
        float freq    = 440.0f * std::pow(2.0f, (note - 69) / 12.0f);
        m_fundamental = freq;
        m_synth->setNote(freq);
        updateOsc2Freq();

        // Re-trigger envelope
        m_synth->envelope().setGate(0.0f);
        QTimer::singleShot(2,
                           this,
                           [this]()
                           {
                               if (m_synth && m_seqPlaying)
                                   m_synth->envelope().setGate(1.0f);
                           });

        // Schedule gate off
        float stepMs = 60000.0f / m_seqBpm;
        if (m_seqSubdivision > 1)
            stepMs /= static_cast<float>(m_seqSubdivision);
        auto gateMs = static_cast<int>(stepMs * m_seqGate);

        QTimer::singleShot(gateMs,
                           this,
                           [this, playingStep]()
                           {
                               if (m_synth && m_seqCurrentStep != playingStep)
                                   m_synth->envelope().setGate(0.0f);
                           });
    }
    else
    {
        // Rest — gate off
        m_synth->envelope().setGate(0.0f);
    }
}

void SynthBridge::setSeqPlaying(bool v)
{
    if (m_seqPlaying == v)
        return;
    m_seqPlaying = v;
    if (v)
    {
        m_sequencer.reset();
        m_seqCurrentStep = -1;
        seqStep();
        m_seqTimer.start();
    }
    else
    {
        m_seqTimer.stop();
        m_seqCurrentStep = -1;
        emit seqCurrentStepChanged();
        if (m_synth)
            m_synth->envelope().setGate(0.0f);
    }
    emit seqPlayingChanged();
}

void SynthBridge::setSeqBpm(float v)
{
    if (qFuzzyCompare(m_seqBpm, v))
        return;
    m_seqBpm = v;
    updateSeqTimer();
    emit seqBpmChanged();
}

void SynthBridge::setSeqStepCount(int v)
{
    if (m_sequencer.stepCount() == v)
        return;
    m_sequencer.setStepCount(v);
    emit seqStepCountChanged();
    emit seqNotesChanged();
}

void SynthBridge::setSeqGate(float v)
{
    if (qFuzzyCompare(m_seqGate, v))
        return;
    m_seqGate = v;
    emit seqGateChanged();
}

void SynthBridge::setSeqSubdivision(int v)
{
    if (m_seqSubdivision == v)
        return;
    m_seqSubdivision = v;
    updateSeqTimer();
    emit seqSubdivisionChanged();
}

void SynthBridge::setSeqSelectedStep(int v)
{
    if (m_seqSelectedStep == v)
        return;
    m_seqSelectedStep = v;
    emit seqSelectedStepChanged();
}

// --- Presets ---

QString SynthBridge::presetDir() const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/presets";
}

QJsonObject SynthBridge::toJson() const
{
    QJsonObject o;
    o["osc1Shape"]    = m_osc1Shape;
    o["osc1Level"]    = m_osc1Level;
    o["osc2Shape"]    = m_osc2Shape;
    o["osc2Level"]    = m_osc2Level;
    o["osc2BeatFreq"] = m_osc2BeatFreq;
    o["osc2Octave"]   = m_osc2Octave;
    o["subLevel"]     = m_subLevel;

    o["pwmLevel"]      = m_pwmLevel;
    o["pwmDutyCycle"]  = m_pwmDutyCycle;
    o["pwmDcLfoRate"]  = m_pwmDcLfoRate;
    o["pwmDcLfoDepth"] = m_pwmDcLfoDepth;
    o["pwmDcLfoShape"] = m_pwmDcLfoShape;

    o["noiseGain"]  = m_noiseGain;
    o["noiseColor"] = m_noiseColor;

    o["ampAttack"]  = m_ampAttack;
    o["ampDecay"]   = m_ampDecay;
    o["ampSustain"] = m_ampSustain;
    o["ampRelease"] = m_ampRelease;

    o["filterCutoff"]    = m_filterCutoff;
    o["filterResonance"] = m_filterResonance;
    o["filterEnvAmount"] = m_filterEnvAmount;
    o["filterAttack"]    = m_filterAttack;
    o["filterDecay"]     = m_filterDecay;
    o["filterSustain"]   = m_filterSustain;
    o["filterRelease"]   = m_filterRelease;

    o["pitchEnvAttack"]  = m_pitchEnvAttack;
    o["pitchEnvDecay"]   = m_pitchEnvDecay;
    o["pitchEnvSustain"] = m_pitchEnvSustain;
    o["pitchEnvRelease"] = m_pitchEnvRelease;
    o["pitchEnvAmount"]  = m_pitchEnvAmount;

    o["pitchLfoRate"]  = m_pitchLfoRate;
    o["pitchLfoDepth"] = m_pitchLfoDepth;
    o["pitchLfoShape"] = m_pitchLfoShape;

    o["pitchLfo2Rate"]  = m_pitchLfo2Rate;
    o["pitchLfo2Depth"] = m_pitchLfo2Depth;
    o["pitchLfo2Shape"] = m_pitchLfo2Shape;

    o["filterLfoRate"]  = m_filterLfoRate;
    o["filterLfoDepth"] = m_filterLfoDepth;
    o["filterLfoShape"] = m_filterLfoShape;

    o["delayTime"] = m_delayTime;
    o["feedback"]  = m_feedback;

    o["glideTime"] = m_glideTime;

    // Arpeggiator
    o["arpEnabled"]     = m_arpEnabled;
    o["arpBpm"]         = m_arpBpm;
    o["arpPattern"]     = m_arpPattern;
    o["arpOctaveRange"] = m_arpOctaveRange;
    o["arpGate"]        = m_arpGate;
    o["arpSubdivision"] = m_arpSubdivision;

    // Sequencer
    o["seqBpm"]         = m_seqBpm;
    o["seqStepCount"]   = m_sequencer.stepCount();
    o["seqGate"]        = m_seqGate;
    o["seqSubdivision"] = m_seqSubdivision;

    QJsonArray notes;
    for (int i = 0; i < sub37::Sequencer::MaxSteps; ++i)
        notes.append(m_sequencer.getNote(i));
    o["seqNotes"] = notes;

    return o;
}

void SynthBridge::fromJson(const QJsonObject& o)
{
    // Write members directly and push to synth engine, bypassing the
    // qFuzzyCompare guards in the setters.  The float->double->float
    // round-trip through JSON makes qFuzzyCompare treat matching values
    // as equal, causing it to skip the update.

    using Shape = dap::dsp::OscillatorFunctions::Shape;
    using Color = dap::dsp::NoiseGenerator<dap::dsp::UniformDistribution>::Color;

#define LOAD_FLOAT(KEY, MEMBER, SYNTH_CALL, SIGNAL)      \
    if (o.contains(#KEY))                                \
    {                                                    \
        MEMBER = static_cast<float>(o[#KEY].toDouble()); \
        if (m_synth)                                     \
            m_synth->SYNTH_CALL;                         \
        emit SIGNAL();                                   \
    }

#define LOAD_INT(KEY, MEMBER, SYNTH_CALL, SIGNAL) \
    if (o.contains(#KEY))                         \
    {                                             \
        MEMBER = o[#KEY].toInt();                 \
        if (m_synth)                              \
            m_synth->SYNTH_CALL;                  \
        emit SIGNAL();                            \
    }

    // clang-format off
    LOAD_INT(osc1Shape, m_osc1Shape, osc1().setShape(static_cast<Shape>(m_osc1Shape)), osc1ShapeChanged)
    LOAD_FLOAT(osc1Level, m_osc1Level, osc1().setBusLevel(m_osc1Level), osc1LevelChanged)
    LOAD_INT(osc2Shape, m_osc2Shape, osc2().setShape(static_cast<Shape>(m_osc2Shape)), osc2ShapeChanged)
    LOAD_FLOAT(osc2Level, m_osc2Level, osc2().setBusLevel(m_osc2Level), osc2LevelChanged)
    LOAD_FLOAT(subLevel, m_subLevel, setBusGain<2>(m_subLevel), subLevelChanged)
    // clang-format on

    if (o.contains("osc2Octave"))
    {
        m_osc2Octave = o["osc2Octave"].toInt();
        emit osc2OctaveChanged();
    }

    if (o.contains("osc2BeatFreq"))
    {
        m_osc2BeatFreq = static_cast<float>(o["osc2BeatFreq"].toDouble());
        if (m_synth)
            m_synth->setOsc2BeatFreq(m_osc2BeatFreq);
        emit osc2BeatFreqChanged();
    }

    // clang-format off
    LOAD_FLOAT(pwmLevel, m_pwmLevel, pwm().setBusLevel(m_pwmLevel), pwmLevelChanged)
    LOAD_FLOAT(pwmDutyCycle, m_pwmDutyCycle, pwm().setDutyCycle(m_pwmDutyCycle), pwmDutyCycleChanged)
    LOAD_FLOAT(pwmDcLfoRate, m_pwmDcLfoRate, pwm().setDcLfoRate(m_pwmDcLfoRate), pwmDcLfoRateChanged)
    LOAD_FLOAT(pwmDcLfoDepth, m_pwmDcLfoDepth, pwm().setDcLfoDepth(m_pwmDcLfoDepth), pwmDcLfoDepthChanged)
    LOAD_INT(pwmDcLfoShape, m_pwmDcLfoShape, pwm().setDcLfoShape(static_cast<Shape>(m_pwmDcLfoShape)), pwmDcLfoShapeChanged)

    LOAD_FLOAT(noiseGain, m_noiseGain, noise().setGain(m_noiseGain), noiseGainChanged)
    LOAD_INT(noiseColor, m_noiseColor, noise().setColor(static_cast<Color>(m_noiseColor)), noiseColorChanged)

    LOAD_FLOAT(ampAttack, m_ampAttack, envelope().setAttack(m_ampAttack), ampAttackChanged)
    LOAD_FLOAT(ampDecay, m_ampDecay, envelope().setDecay(m_ampDecay), ampDecayChanged)
    LOAD_FLOAT(ampSustain, m_ampSustain, envelope().setSustain(m_ampSustain), ampSustainChanged)
    LOAD_FLOAT(ampRelease, m_ampRelease, envelope().setRelease(m_ampRelease), ampReleaseChanged)

    LOAD_FLOAT(filterCutoff, m_filterCutoff, filterEnvelope().setBase(m_filterCutoff), filterCutoffChanged)
    LOAD_FLOAT(filterResonance, m_filterResonance, filter().setResonance(m_filterResonance), filterResonanceChanged)
    LOAD_FLOAT(filterEnvAmount, m_filterEnvAmount, filterEnvelope().setAmount(m_filterEnvAmount), filterEnvAmountChanged)
    LOAD_FLOAT(filterAttack, m_filterAttack, filterEnvelope().setAttack(m_filterAttack), filterAttackChanged)
    LOAD_FLOAT(filterDecay, m_filterDecay, filterEnvelope().setDecay(m_filterDecay), filterDecayChanged)
    LOAD_FLOAT(filterSustain, m_filterSustain, filterEnvelope().setSustain(m_filterSustain), filterSustainChanged)
    LOAD_FLOAT(filterRelease, m_filterRelease, filterEnvelope().setRelease(m_filterRelease), filterReleaseChanged)

    LOAD_FLOAT(pitchEnvAttack, m_pitchEnvAttack, pitchEnvelope().setAttack(m_pitchEnvAttack), pitchEnvAttackChanged)
    LOAD_FLOAT(pitchEnvDecay, m_pitchEnvDecay, pitchEnvelope().setDecay(m_pitchEnvDecay), pitchEnvDecayChanged)
    LOAD_FLOAT(pitchEnvSustain, m_pitchEnvSustain, pitchEnvelope().setSustain(m_pitchEnvSustain), pitchEnvSustainChanged)
    LOAD_FLOAT(pitchEnvRelease, m_pitchEnvRelease, pitchEnvelope().setRelease(m_pitchEnvRelease), pitchEnvReleaseChanged)
    LOAD_FLOAT(pitchEnvAmount, m_pitchEnvAmount, pitchEnvelope().setAmount(m_pitchEnvAmount), pitchEnvAmountChanged)
    // clang-format on

    // Pitch LFOs need to call the combined setters
    if (o.contains("pitchLfoRate"))
        m_pitchLfoRate = static_cast<float>(o["pitchLfoRate"].toDouble());
    if (o.contains("pitchLfoDepth"))
        m_pitchLfoDepth = static_cast<float>(o["pitchLfoDepth"].toDouble());
    if (o.contains("pitchLfoShape"))
        m_pitchLfoShape = o["pitchLfoShape"].toInt();
    if (m_synth)
        m_synth->setPitchLfo(m_pitchLfoRate, m_pitchLfoDepth, static_cast<Shape>(m_pitchLfoShape));
    emit pitchLfoRateChanged();
    emit pitchLfoDepthChanged();
    emit pitchLfoShapeChanged();

    if (o.contains("pitchLfo2Rate"))
        m_pitchLfo2Rate = static_cast<float>(o["pitchLfo2Rate"].toDouble());
    if (o.contains("pitchLfo2Depth"))
        m_pitchLfo2Depth = static_cast<float>(o["pitchLfo2Depth"].toDouble());
    if (o.contains("pitchLfo2Shape"))
        m_pitchLfo2Shape = o["pitchLfo2Shape"].toInt();
    if (m_synth)
        m_synth->setPitchLfo2(
            m_pitchLfo2Rate, m_pitchLfo2Depth, static_cast<Shape>(m_pitchLfo2Shape));
    emit pitchLfo2RateChanged();
    emit pitchLfo2DepthChanged();
    emit pitchLfo2ShapeChanged();

    // clang-format off
    LOAD_FLOAT(filterLfoRate, m_filterLfoRate, filter().setLfoRate(m_filterLfoRate), filterLfoRateChanged)
    LOAD_FLOAT(filterLfoDepth, m_filterLfoDepth, filter().setLfoDepth(m_filterLfoDepth), filterLfoDepthChanged)
    LOAD_INT(filterLfoShape, m_filterLfoShape, filter().setLfoShape(static_cast<Shape>(m_filterLfoShape)), filterLfoShapeChanged)
    // clang-format on

    if (o.contains("delayTime"))
    {
        m_delayTime = static_cast<float>(o["delayTime"].toDouble());
        if (m_synth)
            m_synth->setDelayTime(m_delayTime * m_sampleRate);
        emit delayTimeChanged();
    }

    if (o.contains("feedback"))
    {
        m_feedback = static_cast<float>(o["feedback"].toDouble());
        if (m_synth)
            m_synth->setFeedback(m_feedback);
        emit feedbackChanged();
    }

    if (o.contains("glideTime"))
    {
        m_glideTime = static_cast<float>(o["glideTime"].toDouble());
        if (m_synth)
            m_synth->setGlide(static_cast<size_t>(m_glideTime * m_sampleRate));
        emit glideTimeChanged();
    }

    // Arpeggiator
    if (o.contains("arpBpm"))
        setArpBpm(o["arpBpm"].toDouble());
    if (o.contains("arpPattern"))
        setArpPattern(o["arpPattern"].toInt());
    if (o.contains("arpOctaveRange"))
        setArpOctaveRange(o["arpOctaveRange"].toInt());
    if (o.contains("arpGate"))
        setArpGate(o["arpGate"].toDouble());
    if (o.contains("arpSubdivision"))
        setArpSubdivision(o["arpSubdivision"].toInt());
    if (o.contains("arpEnabled"))
        setArpEnabled(o["arpEnabled"].toBool());

    // Sequencer
    if (o.contains("seqBpm"))
        setSeqBpm(o["seqBpm"].toDouble());
    if (o.contains("seqGate"))
        setSeqGate(o["seqGate"].toDouble());
    if (o.contains("seqSubdivision"))
        setSeqSubdivision(o["seqSubdivision"].toInt());
    if (o.contains("seqStepCount"))
        setSeqStepCount(o["seqStepCount"].toInt());

    if (o.contains("seqNotes"))
    {
        auto arr = o["seqNotes"].toArray();
        m_sequencer.clear();
        for (int i = 0; i < arr.size() && i < sub37::Sequencer::MaxSteps; ++i)
        {
            int note = arr[i].toInt(-1);
            if (note >= 0)
                m_sequencer.setNote(i, note);
        }
        emit seqNotesChanged();
    }

#undef LOAD_FLOAT
#undef LOAD_INT
}

QStringList SynthBridge::presetList() const
{
    QDir dir(presetDir());
    if (!dir.exists())
        return {};
    auto entries = dir.entryList({"*.json"}, QDir::Files, QDir::Name);
    QStringList names;
    for (auto& e : entries)
        names.append(e.chopped(5)); // strip ".json"
    return names;
}

void SynthBridge::savePreset(const QString& name)
{
    QDir dir(presetDir());
    if (!dir.exists())
        dir.mkpath(".");

    QFile file(dir.filePath(name + ".json"));
    if (!file.open(QIODevice::WriteOnly))
        return;

    file.write(QJsonDocument(toJson()).toJson(QJsonDocument::Indented));
    m_currentPreset = name;
    emit currentPresetChanged();
    emit presetListChanged();
}

void SynthBridge::loadPreset(const QString& name)
{
    QFile file(presetDir() + "/" + name + ".json");
    if (!file.open(QIODevice::ReadOnly))
        return;

    auto doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject())
        return;

    fromJson(doc.object());
    m_currentPreset = name;
    emit currentPresetChanged();
}

void SynthBridge::deletePreset(const QString& name)
{
    QFile::remove(presetDir() + "/" + name + ".json");
    if (m_currentPreset == name)
    {
        m_currentPreset.clear();
        emit currentPresetChanged();
    }
    emit presetListChanged();
}
