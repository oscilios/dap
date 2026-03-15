#pragma once
#include <QObject>
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

    // Glide
    Q_PROPERTY(float glideTime READ glideTime WRITE setGlideTime NOTIFY glideTimeChanged)

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

    float glideTime() const
    {
        return m_glideTime;
    }

    void setDeviceIndex(int idx);

    Q_INVOKABLE bool startAudio();
    Q_INVOKABLE bool stopAudio();
    Q_INVOKABLE void noteOn(int midiNote);
    Q_INVOKABLE void noteOff();

public slots:
    void setOsc1Shape(int v);
    void setOsc1Level(float v);
    void setOsc2Shape(int v);
    void setOsc2Level(float v);
    void setOsc2BeatFreq(float v);
    void setOsc2Octave(int v);
    void setSubLevel(float v);

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

    void setPitchLfoRate(float v);
    void setPitchLfoDepth(float v);
    void setPitchLfoShape(int v);

    void setPitchLfo2Rate(float v);
    void setPitchLfo2Depth(float v);
    void setPitchLfo2Shape(int v);

    void setFilterLfoRate(float v);
    void setFilterLfoDepth(float v);
    void setFilterLfoShape(int v);

    void setGlideTime(float v);

signals:
    void osc1ShapeChanged();
    void osc1LevelChanged();
    void osc2ShapeChanged();
    void osc2LevelChanged();
    void osc2BeatFreqChanged();
    void osc2OctaveChanged();
    void subLevelChanged();

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

    void pitchLfoRateChanged();
    void pitchLfoDepthChanged();
    void pitchLfoShapeChanged();

    void pitchLfo2RateChanged();
    void pitchLfo2DepthChanged();
    void pitchLfo2ShapeChanged();

    void filterLfoRateChanged();
    void filterLfoDepthChanged();
    void filterLfoShapeChanged();

    void glideTimeChanged();

private:
    void initAudioDevice(int deviceIdx);
    void updateOsc2Freq();

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

    float m_pitchLfoRate  = 5.0f;
    float m_pitchLfoDepth = 0.0f;
    int m_pitchLfoShape   = 4; // Triangle

    float m_pitchLfo2Rate  = 5.0f;
    float m_pitchLfo2Depth = 0.0f;
    int m_pitchLfo2Shape   = 4; // Triangle

    float m_filterLfoRate  = 0.5f;
    float m_filterLfoDepth = 0.0f;
    int m_filterLfoShape   = 4; // Triangle

    float m_glideTime = 0.0f;
};
