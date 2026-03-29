#pragma once
#include "../Arpeggiator.h"
#include "../Sequencer.h"
#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QtQmlIntegration>
#include <memory>

namespace sub37
{
    class AudioProcess;
    class Synth;
}

class SynthBridge : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    // Oscillators
    Q_PROPERTY(int osc1Shape READ osc1Shape WRITE setOsc1Shape NOTIFY osc1ShapeChanged)
    Q_PROPERTY(float osc1Level READ osc1Level WRITE setOsc1Level NOTIFY osc1LevelChanged)
    Q_PROPERTY(int osc2Shape READ osc2Shape WRITE setOsc2Shape NOTIFY osc2ShapeChanged)
    Q_PROPERTY(float osc2Level READ osc2Level WRITE setOsc2Level NOTIFY osc2LevelChanged)
    Q_PROPERTY(
        float osc2BeatFreq READ osc2BeatFreq WRITE setOsc2BeatFreq NOTIFY osc2BeatFreqChanged)
    Q_PROPERTY(int osc2Octave READ osc2Octave WRITE setOsc2Octave NOTIFY osc2OctaveChanged)
    Q_PROPERTY(float subLevel READ subLevel WRITE setSubLevel NOTIFY subLevelChanged)

    // PWM
    Q_PROPERTY(float pwmLevel READ pwmLevel WRITE setPwmLevel NOTIFY pwmLevelChanged)
    Q_PROPERTY(
        float pwmDutyCycle READ pwmDutyCycle WRITE setPwmDutyCycle NOTIFY pwmDutyCycleChanged)
    Q_PROPERTY(
        float pwmDcLfoRate READ pwmDcLfoRate WRITE setPwmDcLfoRate NOTIFY pwmDcLfoRateChanged)
    Q_PROPERTY(
        float pwmDcLfoDepth READ pwmDcLfoDepth WRITE setPwmDcLfoDepth NOTIFY pwmDcLfoDepthChanged)
    Q_PROPERTY(
        int pwmDcLfoShape READ pwmDcLfoShape WRITE setPwmDcLfoShape NOTIFY pwmDcLfoShapeChanged)

    // Noise
    Q_PROPERTY(float noiseGain READ noiseGain WRITE setNoiseGain NOTIFY noiseGainChanged)
    Q_PROPERTY(int noiseColor READ noiseColor WRITE setNoiseColor NOTIFY noiseColorChanged)

    // Amp Envelope
    Q_PROPERTY(float ampAttack READ ampAttack WRITE setAmpAttack NOTIFY ampAttackChanged)
    Q_PROPERTY(float ampDecay READ ampDecay WRITE setAmpDecay NOTIFY ampDecayChanged)
    Q_PROPERTY(float ampSustain READ ampSustain WRITE setAmpSustain NOTIFY ampSustainChanged)
    Q_PROPERTY(float ampRelease READ ampRelease WRITE setAmpRelease NOTIFY ampReleaseChanged)

    // Filter
    Q_PROPERTY(
        float filterCutoff READ filterCutoff WRITE setFilterCutoff NOTIFY filterCutoffChanged)
    Q_PROPERTY(float filterResonance READ filterResonance WRITE setFilterResonance NOTIFY
                   filterResonanceChanged)
    Q_PROPERTY(float filterEnvAmount READ filterEnvAmount WRITE setFilterEnvAmount NOTIFY
                   filterEnvAmountChanged)

    // Filter Envelope
    Q_PROPERTY(
        float filterAttack READ filterAttack WRITE setFilterAttack NOTIFY filterAttackChanged)
    Q_PROPERTY(float filterDecay READ filterDecay WRITE setFilterDecay NOTIFY filterDecayChanged)
    Q_PROPERTY(
        float filterSustain READ filterSustain WRITE setFilterSustain NOTIFY filterSustainChanged)
    Q_PROPERTY(
        float filterRelease READ filterRelease WRITE setFilterRelease NOTIFY filterReleaseChanged)

    // Pitch Envelope
    Q_PROPERTY(float pitchEnvAttack READ pitchEnvAttack WRITE setPitchEnvAttack NOTIFY
                   pitchEnvAttackChanged)
    Q_PROPERTY(
        float pitchEnvDecay READ pitchEnvDecay WRITE setPitchEnvDecay NOTIFY pitchEnvDecayChanged)
    Q_PROPERTY(float pitchEnvSustain READ pitchEnvSustain WRITE setPitchEnvSustain NOTIFY
                   pitchEnvSustainChanged)
    Q_PROPERTY(float pitchEnvRelease READ pitchEnvRelease WRITE setPitchEnvRelease NOTIFY
                   pitchEnvReleaseChanged)
    Q_PROPERTY(float pitchEnvAmount READ pitchEnvAmount WRITE setPitchEnvAmount NOTIFY
                   pitchEnvAmountChanged)

    // Mod 1 (Pitch LFO)
    Q_PROPERTY(
        float pitchLfoRate READ pitchLfoRate WRITE setPitchLfoRate NOTIFY pitchLfoRateChanged)
    Q_PROPERTY(
        float pitchLfoDepth READ pitchLfoDepth WRITE setPitchLfoDepth NOTIFY pitchLfoDepthChanged)
    Q_PROPERTY(
        int pitchLfoShape READ pitchLfoShape WRITE setPitchLfoShape NOTIFY pitchLfoShapeChanged)

    // Mod 1b (Pitch LFO 2 — Osc 2 only)
    Q_PROPERTY(
        float pitchLfo2Rate READ pitchLfo2Rate WRITE setPitchLfo2Rate NOTIFY pitchLfo2RateChanged)
    Q_PROPERTY(float pitchLfo2Depth READ pitchLfo2Depth WRITE setPitchLfo2Depth NOTIFY
                   pitchLfo2DepthChanged)
    Q_PROPERTY(
        int pitchLfo2Shape READ pitchLfo2Shape WRITE setPitchLfo2Shape NOTIFY pitchLfo2ShapeChanged)

    // Mod 2 (Filter LFO)
    Q_PROPERTY(
        float filterLfoRate READ filterLfoRate WRITE setFilterLfoRate NOTIFY filterLfoRateChanged)
    Q_PROPERTY(float filterLfoDepth READ filterLfoDepth WRITE setFilterLfoDepth NOTIFY
                   filterLfoDepthChanged)
    Q_PROPERTY(
        int filterLfoShape READ filterLfoShape WRITE setFilterLfoShape NOTIFY filterLfoShapeChanged)

    // Feedback delay
    Q_PROPERTY(float delayTime READ delayTime WRITE setDelayTime NOTIFY delayTimeChanged)
    Q_PROPERTY(float feedback READ feedback WRITE setFeedback NOTIFY feedbackChanged)

    // Glide
    Q_PROPERTY(float glideTime READ glideTime WRITE setGlideTime NOTIFY glideTimeChanged)

    // Hold
    Q_PROPERTY(bool hold READ hold WRITE setHold NOTIFY holdChanged)

    // Arpeggiator
    Q_PROPERTY(bool arpEnabled READ arpEnabled WRITE setArpEnabled NOTIFY arpEnabledChanged)
    Q_PROPERTY(float arpBpm READ arpBpm WRITE setArpBpm NOTIFY arpBpmChanged)
    Q_PROPERTY(int arpPattern READ arpPattern WRITE setArpPattern NOTIFY arpPatternChanged)
    Q_PROPERTY(
        int arpOctaveRange READ arpOctaveRange WRITE setArpOctaveRange NOTIFY arpOctaveRangeChanged)
    Q_PROPERTY(float arpGate READ arpGate WRITE setArpGate NOTIFY arpGateChanged)
    Q_PROPERTY(
        int arpSubdivision READ arpSubdivision WRITE setArpSubdivision NOTIFY arpSubdivisionChanged)
    Q_PROPERTY(int arpCurrentNote READ arpCurrentNote NOTIFY arpCurrentNoteChanged)

    // Sequencer
    Q_PROPERTY(bool seqPlaying READ seqPlaying WRITE setSeqPlaying NOTIFY seqPlayingChanged)
    Q_PROPERTY(float seqBpm READ seqBpm WRITE setSeqBpm NOTIFY seqBpmChanged)
    Q_PROPERTY(int seqStepCount READ seqStepCount WRITE setSeqStepCount NOTIFY seqStepCountChanged)
    Q_PROPERTY(float seqGate READ seqGate WRITE setSeqGate NOTIFY seqGateChanged)
    Q_PROPERTY(
        int seqSubdivision READ seqSubdivision WRITE setSeqSubdivision NOTIFY seqSubdivisionChanged)
    Q_PROPERTY(int seqCurrentStep READ seqCurrentStep NOTIFY seqCurrentStepChanged)
    Q_PROPERTY(int seqSelectedStep READ seqSelectedStep WRITE setSeqSelectedStep NOTIFY
                   seqSelectedStepChanged)
    Q_PROPERTY(QVariantList seqNotes READ seqNotes NOTIFY seqNotesChanged)

public:
    explicit SynthBridge(QObject* parent = nullptr);
    ~SynthBridge() override;

    // Getters
    int osc1Shape() const
    {
        return m_osc1Shape;
    }
    float osc1Level() const
    {
        return m_osc1Level;
    }
    int osc2Shape() const
    {
        return m_osc2Shape;
    }
    float osc2Level() const
    {
        return m_osc2Level;
    }
    float osc2BeatFreq() const
    {
        return m_osc2BeatFreq;
    }
    int osc2Octave() const
    {
        return m_osc2Octave;
    }
    float subLevel() const
    {
        return m_subLevel;
    }

    float pwmLevel() const
    {
        return m_pwmLevel;
    }
    float pwmDutyCycle() const
    {
        return m_pwmDutyCycle;
    }
    float pwmDcLfoRate() const
    {
        return m_pwmDcLfoRate;
    }
    float pwmDcLfoDepth() const
    {
        return m_pwmDcLfoDepth;
    }
    int pwmDcLfoShape() const
    {
        return m_pwmDcLfoShape;
    }

    float noiseGain() const
    {
        return m_noiseGain;
    }
    int noiseColor() const
    {
        return m_noiseColor;
    }

    float ampAttack() const
    {
        return m_ampAttack;
    }
    float ampDecay() const
    {
        return m_ampDecay;
    }
    float ampSustain() const
    {
        return m_ampSustain;
    }
    float ampRelease() const
    {
        return m_ampRelease;
    }

    float filterCutoff() const
    {
        return m_filterCutoff;
    }
    float filterResonance() const
    {
        return m_filterResonance;
    }
    float filterEnvAmount() const
    {
        return m_filterEnvAmount;
    }

    float filterAttack() const
    {
        return m_filterAttack;
    }
    float filterDecay() const
    {
        return m_filterDecay;
    }
    float filterSustain() const
    {
        return m_filterSustain;
    }
    float filterRelease() const
    {
        return m_filterRelease;
    }

    float pitchEnvAttack() const
    {
        return m_pitchEnvAttack;
    }
    float pitchEnvDecay() const
    {
        return m_pitchEnvDecay;
    }
    float pitchEnvSustain() const
    {
        return m_pitchEnvSustain;
    }
    float pitchEnvRelease() const
    {
        return m_pitchEnvRelease;
    }
    float pitchEnvAmount() const
    {
        return m_pitchEnvAmount;
    }

    float pitchLfoRate() const
    {
        return m_pitchLfoRate;
    }
    float pitchLfoDepth() const
    {
        return m_pitchLfoDepth;
    }
    int pitchLfoShape() const
    {
        return m_pitchLfoShape;
    }

    float pitchLfo2Rate() const
    {
        return m_pitchLfo2Rate;
    }
    float pitchLfo2Depth() const
    {
        return m_pitchLfo2Depth;
    }
    int pitchLfo2Shape() const
    {
        return m_pitchLfo2Shape;
    }

    float filterLfoRate() const
    {
        return m_filterLfoRate;
    }
    float filterLfoDepth() const
    {
        return m_filterLfoDepth;
    }
    int filterLfoShape() const
    {
        return m_filterLfoShape;
    }

    float delayTime() const
    {
        return m_delayTime;
    }
    float feedback() const
    {
        return m_feedback;
    }

    float glideTime() const
    {
        return m_glideTime;
    }

    bool arpEnabled() const
    {
        return m_arpEnabled;
    }
    float arpBpm() const
    {
        return m_arpBpm;
    }
    int arpPattern() const
    {
        return m_arpPattern;
    }
    int arpOctaveRange() const
    {
        return m_arpOctaveRange;
    }
    float arpGate() const
    {
        return m_arpGate;
    }
    int arpSubdivision() const
    {
        return m_arpSubdivision;
    }
    bool hold() const
    {
        return m_hold;
    }
    int arpCurrentNote() const
    {
        return m_arpCurrentNote;
    }

    bool seqPlaying() const
    {
        return m_seqPlaying;
    }
    float seqBpm() const
    {
        return m_seqBpm;
    }
    int seqStepCount() const
    {
        return m_sequencer.stepCount();
    }
    float seqGate() const
    {
        return m_seqGate;
    }
    int seqSubdivision() const
    {
        return m_seqSubdivision;
    }
    int seqCurrentStep() const
    {
        return m_seqCurrentStep;
    }
    int seqSelectedStep() const
    {
        return m_seqSelectedStep;
    }
    QVariantList seqNotes() const;

    Q_INVOKABLE void seqSetNote(int step, int note);
    Q_INVOKABLE void seqClearStep(int step);
    Q_INVOKABLE void seqClear();

    void setDeviceIndex(int idx);

    Q_INVOKABLE bool startAudio();
    Q_INVOKABLE bool stopAudio();
    Q_INVOKABLE void noteOn(int midiNote);
    Q_INVOKABLE void noteOff(int midiNote);

public slots:
    void setOsc1Shape(int v);
    void setOsc1Level(float v);
    void setOsc2Shape(int v);
    void setOsc2Level(float v);
    void setOsc2BeatFreq(float v);
    void setOsc2Octave(int v);
    void setSubLevel(float v);

    void setPwmLevel(float v);
    void setPwmDutyCycle(float v);
    void setPwmDcLfoRate(float v);
    void setPwmDcLfoDepth(float v);
    void setPwmDcLfoShape(int v);

    void setNoiseGain(float v);
    void setNoiseColor(int v);

    void setAmpAttack(float v);
    void setAmpDecay(float v);
    void setAmpSustain(float v);
    void setAmpRelease(float v);

    void setFilterCutoff(float v);
    void setFilterResonance(float v);
    void setFilterEnvAmount(float v);

    void setFilterAttack(float v);
    void setFilterDecay(float v);
    void setFilterSustain(float v);
    void setFilterRelease(float v);

    void setPitchEnvAttack(float v);
    void setPitchEnvDecay(float v);
    void setPitchEnvSustain(float v);
    void setPitchEnvRelease(float v);
    void setPitchEnvAmount(float v);

    void setPitchLfoRate(float v);
    void setPitchLfoDepth(float v);
    void setPitchLfoShape(int v);

    void setPitchLfo2Rate(float v);
    void setPitchLfo2Depth(float v);
    void setPitchLfo2Shape(int v);

    void setFilterLfoRate(float v);
    void setFilterLfoDepth(float v);
    void setFilterLfoShape(int v);

    void setDelayTime(float v);
    void setFeedback(float v);

    void setGlideTime(float v);

    void setArpEnabled(bool v);
    void setArpBpm(float v);
    void setArpPattern(int v);
    void setArpOctaveRange(int v);
    void setArpGate(float v);
    void setArpSubdivision(int v);
    void setHold(bool v);

    void setSeqPlaying(bool v);
    void setSeqBpm(float v);
    void setSeqStepCount(int v);
    void setSeqGate(float v);
    void setSeqSubdivision(int v);
    void setSeqSelectedStep(int v);

signals:
    void osc1ShapeChanged();
    void osc1LevelChanged();
    void osc2ShapeChanged();
    void osc2LevelChanged();
    void osc2BeatFreqChanged();
    void osc2OctaveChanged();
    void subLevelChanged();

    void pwmLevelChanged();
    void pwmDutyCycleChanged();
    void pwmDcLfoRateChanged();
    void pwmDcLfoDepthChanged();
    void pwmDcLfoShapeChanged();

    void noiseGainChanged();
    void noiseColorChanged();

    void ampAttackChanged();
    void ampDecayChanged();
    void ampSustainChanged();
    void ampReleaseChanged();

    void filterCutoffChanged();
    void filterResonanceChanged();
    void filterEnvAmountChanged();

    void filterAttackChanged();
    void filterDecayChanged();
    void filterSustainChanged();
    void filterReleaseChanged();

    void pitchEnvAttackChanged();
    void pitchEnvDecayChanged();
    void pitchEnvSustainChanged();
    void pitchEnvReleaseChanged();
    void pitchEnvAmountChanged();

    void pitchLfoRateChanged();
    void pitchLfoDepthChanged();
    void pitchLfoShapeChanged();

    void pitchLfo2RateChanged();
    void pitchLfo2DepthChanged();
    void pitchLfo2ShapeChanged();

    void filterLfoRateChanged();
    void filterLfoDepthChanged();
    void filterLfoShapeChanged();

    void delayTimeChanged();
    void feedbackChanged();

    void glideTimeChanged();

    void arpEnabledChanged();
    void arpBpmChanged();
    void arpPatternChanged();
    void arpOctaveRangeChanged();
    void arpGateChanged();
    void arpSubdivisionChanged();
    void holdChanged();
    void arpCurrentNoteChanged();

    void seqPlayingChanged();
    void seqBpmChanged();
    void seqStepCountChanged();
    void seqGateChanged();
    void seqSubdivisionChanged();
    void seqCurrentStepChanged();
    void seqSelectedStepChanged();
    void seqNotesChanged();

private:
    void initAudioDevice(int deviceIdx);
    void updateOsc2Freq();
    void arpStep();
    void updateArpTimer();
    void seqStep();
    void updateSeqTimer();

    std::unique_ptr<sub37::AudioProcess> m_process;
    sub37::Synth* m_synth = nullptr;
    int m_deviceIndex     = 0;
    float m_sampleRate    = 44100.0f;
    bool m_noteActive     = false;
    float m_fundamental   = 110.0f;

    // Cached parameter values (defaults match Sub37Synth constructor)
    int m_osc1Shape      = 2; // Saw
    float m_osc1Level    = 0.7f;
    int m_osc2Shape      = 2; // Saw
    float m_osc2Level    = 0.5f;
    float m_osc2BeatFreq = 2.0f;
    int m_osc2Octave     = 0;
    float m_subLevel     = 0.6f;

    float m_pwmLevel      = 0.0f;
    float m_pwmDutyCycle  = 0.5f;
    float m_pwmDcLfoRate  = 1.0f;
    float m_pwmDcLfoDepth = 0.0f;
    int m_pwmDcLfoShape   = 4; // Triangle

    float m_noiseGain = 0.0f;
    int m_noiseColor  = 0; // White

    float m_ampAttack  = 0.005f;
    float m_ampDecay   = 0.3f;
    float m_ampSustain = 0.7f;
    float m_ampRelease = 0.15f;

    float m_filterCutoff    = 800.0f;
    float m_filterResonance = 0.4f;
    float m_filterEnvAmount = 3000.0f;

    float m_filterAttack  = 0.001f;
    float m_filterDecay   = 0.4f;
    float m_filterSustain = 0.0f;
    float m_filterRelease = 0.2f;

    float m_pitchEnvAttack  = 0.01f;
    float m_pitchEnvDecay   = 0.3f;
    float m_pitchEnvSustain = 0.0f;
    float m_pitchEnvRelease = 0.1f;
    float m_pitchEnvAmount  = 0.0f;

    float m_pitchLfoRate  = 5.0f;
    float m_pitchLfoDepth = 0.0f;
    int m_pitchLfoShape   = 4; // Triangle

    float m_pitchLfo2Rate  = 5.0f;
    float m_pitchLfo2Depth = 0.0f;
    int m_pitchLfo2Shape   = 4; // Triangle

    float m_filterLfoRate  = 0.5f;
    float m_filterLfoDepth = 0.0f;
    int m_filterLfoShape   = 4; // Triangle

    float m_delayTime = 0.0f;
    float m_feedback  = 0.0f;

    float m_glideTime = 0.0f;

    // Arpeggiator
    sub37::Arpeggiator m_arpeggiator;
    QTimer m_arpTimer;
    bool m_hold          = false;
    bool m_arpEnabled    = false;
    float m_arpBpm       = 120.0f;
    int m_arpPattern     = 0; // Up
    int m_arpOctaveRange = 1;
    float m_arpGate      = 0.5f;
    int m_arpSubdivision = 2; // Eighth notes
    int m_arpCurrentNote = -1;

    // Sequencer
    sub37::Sequencer m_sequencer;
    QTimer m_seqTimer;
    bool m_seqPlaying     = false;
    float m_seqBpm        = 120.0f;
    float m_seqGate       = 0.5f;
    int m_seqSubdivision  = 2; // Eighth notes
    int m_seqCurrentStep  = -1;
    int m_seqSelectedStep = -1;
};
