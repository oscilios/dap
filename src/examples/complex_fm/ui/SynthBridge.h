#pragma once

#include <QObject>
#include <QQmlEngine>
#include <memory>

namespace complex_fm
{
    class AudioProcess;
    class Synth;
}

class SynthBridge : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    // Per-operator properties (op0-op4)
#define SYNTH_OP_PROPS(N)                                                                 \
    Q_PROPERTY(float op##N##_carrierLevel READ op##N##_carrierLevel WRITE                 \
                   setOp##N##_carrierLevel NOTIFY op##N##_carrierLevelChanged)            \
    Q_PROPERTY(float op##N##_amRate READ op##N##_amRate WRITE setOp##N##_amRate NOTIFY    \
                   op##N##_amRateChanged)                                                 \
    Q_PROPERTY(float op##N##_amDepth READ op##N##_amDepth WRITE setOp##N##_amDepth NOTIFY \
                   op##N##_amDepthChanged)                                                \
    Q_PROPERTY(float op##N##_amEnvAmount READ op##N##_amEnvAmount WRITE                   \
                   setOp##N##_amEnvAmount NOTIFY op##N##_amEnvAmountChanged)              \
    Q_PROPERTY(float op##N##_vibratoRate READ op##N##_vibratoRate WRITE                   \
                   setOp##N##_vibratoRate NOTIFY op##N##_vibratoRateChanged)              \
    Q_PROPERTY(float op##N##_vibratoDepth READ op##N##_vibratoDepth WRITE                 \
                   setOp##N##_vibratoDepth NOTIFY op##N##_vibratoDepthChanged)            \
    Q_PROPERTY(float op##N##_modFreq READ op##N##_modFreq WRITE setOp##N##_modFreq NOTIFY \
                   op##N##_modFreqChanged)                                                \
    Q_PROPERTY(float op##N##_modIdx READ op##N##_modIdx WRITE setOp##N##_modIdx NOTIFY    \
                   op##N##_modIdxChanged)                                                 \
    Q_PROPERTY(float op##N##_fmEnvAmount READ op##N##_fmEnvAmount WRITE                   \
                   setOp##N##_fmEnvAmount NOTIFY op##N##_fmEnvAmountChanged)              \
    Q_PROPERTY(float op##N##_busGain READ op##N##_busGain WRITE setOp##N##_busGain NOTIFY \
                   op##N##_busGainChanged)

    SYNTH_OP_PROPS(0)
    SYNTH_OP_PROPS(1)
    SYNTH_OP_PROPS(2)
    SYNTH_OP_PROPS(3)
    SYNTH_OP_PROPS(4)
#undef SYNTH_OP_PROPS

    // Amplitude envelope
    Q_PROPERTY(float ampAttack READ ampAttack WRITE setAmpAttack NOTIFY ampAttackChanged)
    Q_PROPERTY(float ampDecay READ ampDecay WRITE setAmpDecay NOTIFY ampDecayChanged)
    Q_PROPERTY(float ampSustain READ ampSustain WRITE setAmpSustain NOTIFY ampSustainChanged)
    Q_PROPERTY(float ampRelease READ ampRelease WRITE setAmpRelease NOTIFY ampReleaseChanged)

    // Filter
    Q_PROPERTY(float filterResonance READ filterResonance WRITE setFilterResonance NOTIFY
                   filterResonanceChanged)
    Q_PROPERTY(
        float filterEnvBase READ filterEnvBase WRITE setFilterEnvBase NOTIFY filterEnvBaseChanged)
    Q_PROPERTY(float filterEnvAmount READ filterEnvAmount WRITE setFilterEnvAmount NOTIFY
                   filterEnvAmountChanged)
    Q_PROPERTY(float filterEnvAttack READ filterEnvAttack WRITE setFilterEnvAttack NOTIFY
                   filterEnvAttackChanged)
    Q_PROPERTY(float filterEnvDecay READ filterEnvDecay WRITE setFilterEnvDecay NOTIFY
                   filterEnvDecayChanged)
    Q_PROPERTY(float filterEnvSustain READ filterEnvSustain WRITE setFilterEnvSustain NOTIFY
                   filterEnvSustainChanged)
    Q_PROPERTY(float filterEnvRelease READ filterEnvRelease WRITE setFilterEnvRelease NOTIFY
                   filterEnvReleaseChanged)

    // Attack envelope
    Q_PROPERTY(
        float atkEnvAttack READ atkEnvAttack WRITE setAtkEnvAttack NOTIFY atkEnvAttackChanged)
    Q_PROPERTY(float atkEnvDecay READ atkEnvDecay WRITE setAtkEnvDecay NOTIFY atkEnvDecayChanged)
    Q_PROPERTY(
        float atkEnvSustain READ atkEnvSustain WRITE setAtkEnvSustain NOTIFY atkEnvSustainChanged)
    Q_PROPERTY(
        float atkEnvRelease READ atkEnvRelease WRITE setAtkEnvRelease NOTIFY atkEnvReleaseChanged)

    // Phaser
    Q_PROPERTY(float phaserRate READ phaserRate WRITE setPhaserRate NOTIFY phaserRateChanged)
    Q_PROPERTY(float phaserDepth READ phaserDepth WRITE setPhaserDepth NOTIFY phaserDepthChanged)
    Q_PROPERTY(float phaserFeedback READ phaserFeedback WRITE setPhaserFeedback NOTIFY
                   phaserFeedbackChanged)
    Q_PROPERTY(float phaserWet READ phaserWet WRITE setPhaserWet NOTIFY phaserWetChanged)

    // Noise
    Q_PROPERTY(float noiseGain READ noiseGain WRITE setNoiseGain NOTIFY noiseGainChanged)
    Q_PROPERTY(int noiseColor READ noiseColor WRITE setNoiseColor NOTIFY noiseColorChanged)

    // EQ
    Q_PROPERTY(float eqFrequency READ eqFrequency WRITE setEqFrequency NOTIFY eqFrequencyChanged)
    Q_PROPERTY(float eqQ READ eqQ WRITE setEqQ NOTIFY eqQChanged)
    Q_PROPERTY(float eqGainDb READ eqGainDb WRITE setEqGainDb NOTIFY eqGainDbChanged)
    Q_PROPERTY(int eqType READ eqType WRITE setEqType NOTIFY eqTypeChanged)

public:
    explicit SynthBridge(QObject* parent = nullptr);
    ~SynthBridge() override;

    void setDeviceIndex(int idx);

    Q_INVOKABLE bool startAudio();
    Q_INVOKABLE bool stopAudio();

    // Piano keyboard
    Q_INVOKABLE void noteOn(int midiNote);
    Q_INVOKABLE void noteOff();

    // Per-operator getters/setters
#define SYNTH_OP_ACCESSORS(N)              \
    float op##N##_carrierLevel() const     \
    {                                      \
        return m_op[N].carrierLevel;       \
    }                                      \
    float op##N##_amRate() const           \
    {                                      \
        return m_op[N].amRate;             \
    }                                      \
    float op##N##_amDepth() const          \
    {                                      \
        return m_op[N].amDepth;            \
    }                                      \
    float op##N##_amEnvAmount() const      \
    {                                      \
        return m_op[N].amEnvAmount;        \
    }                                      \
    float op##N##_vibratoRate() const      \
    {                                      \
        return m_op[N].vibratoRate;        \
    }                                      \
    float op##N##_vibratoDepth() const     \
    {                                      \
        return m_op[N].vibratoDepth;       \
    }                                      \
    float op##N##_modFreq() const          \
    {                                      \
        return m_op[N].modFreq;            \
    }                                      \
    float op##N##_modIdx() const           \
    {                                      \
        return m_op[N].modIdx;             \
    }                                      \
    float op##N##_fmEnvAmount() const      \
    {                                      \
        return m_op[N].fmEnvAmount;        \
    }                                      \
    float op##N##_busGain() const          \
    {                                      \
        return m_op[N].busGain;            \
    }                                      \
    void setOp##N##_carrierLevel(float v); \
    void setOp##N##_amRate(float v);       \
    void setOp##N##_amDepth(float v);      \
    void setOp##N##_amEnvAmount(float v);  \
    void setOp##N##_vibratoRate(float v);  \
    void setOp##N##_vibratoDepth(float v); \
    void setOp##N##_modFreq(float v);      \
    void setOp##N##_modIdx(float v);       \
    void setOp##N##_fmEnvAmount(float v);  \
    void setOp##N##_busGain(float v);

    SYNTH_OP_ACCESSORS(0)
    SYNTH_OP_ACCESSORS(1)
    SYNTH_OP_ACCESSORS(2)
    SYNTH_OP_ACCESSORS(3)
    SYNTH_OP_ACCESSORS(4)
#undef SYNTH_OP_ACCESSORS

    // Amplitude envelope
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
    void setAmpAttack(float v);
    void setAmpDecay(float v);
    void setAmpSustain(float v);
    void setAmpRelease(float v);

    // Filter
    float filterResonance() const
    {
        return m_filterResonance;
    }
    float filterEnvBase() const
    {
        return m_filterEnvBase;
    }
    float filterEnvAmount() const
    {
        return m_filterEnvAmount;
    }
    float filterEnvAttack() const
    {
        return m_filterEnvAttack;
    }
    float filterEnvDecay() const
    {
        return m_filterEnvDecay;
    }
    float filterEnvSustain() const
    {
        return m_filterEnvSustain;
    }
    float filterEnvRelease() const
    {
        return m_filterEnvRelease;
    }
    void setFilterResonance(float v);
    void setFilterEnvBase(float v);
    void setFilterEnvAmount(float v);
    void setFilterEnvAttack(float v);
    void setFilterEnvDecay(float v);
    void setFilterEnvSustain(float v);
    void setFilterEnvRelease(float v);

    // Attack envelope
    float atkEnvAttack() const
    {
        return m_atkEnvAttack;
    }
    float atkEnvDecay() const
    {
        return m_atkEnvDecay;
    }
    float atkEnvSustain() const
    {
        return m_atkEnvSustain;
    }
    float atkEnvRelease() const
    {
        return m_atkEnvRelease;
    }
    void setAtkEnvAttack(float v);
    void setAtkEnvDecay(float v);
    void setAtkEnvSustain(float v);
    void setAtkEnvRelease(float v);

    // Phaser
    float phaserRate() const
    {
        return m_phaserRate;
    }
    float phaserDepth() const
    {
        return m_phaserDepth;
    }
    float phaserFeedback() const
    {
        return m_phaserFeedback;
    }
    float phaserWet() const
    {
        return m_phaserWet;
    }
    void setPhaserRate(float v);
    void setPhaserDepth(float v);
    void setPhaserFeedback(float v);
    void setPhaserWet(float v);

    // Noise
    float noiseGain() const
    {
        return m_noiseGain;
    }
    int noiseColor() const
    {
        return m_noiseColor;
    }
    void setNoiseGain(float v);
    void setNoiseColor(int v);

    // EQ
    float eqFrequency() const
    {
        return m_eqFrequency;
    }
    float eqQ() const
    {
        return m_eqQ;
    }
    float eqGainDb() const
    {
        return m_eqGainDb;
    }
    int eqType() const
    {
        return m_eqType;
    }
    void setEqFrequency(float v);
    void setEqQ(float v);
    void setEqGainDb(float v);
    void setEqType(int v);

signals:
    // Per-operator signals
#define SYNTH_OP_SIGNALS(N)             \
    void op##N##_carrierLevelChanged(); \
    void op##N##_amRateChanged();       \
    void op##N##_amDepthChanged();      \
    void op##N##_amEnvAmountChanged();  \
    void op##N##_vibratoRateChanged();  \
    void op##N##_vibratoDepthChanged(); \
    void op##N##_modFreqChanged();      \
    void op##N##_modIdxChanged();       \
    void op##N##_fmEnvAmountChanged();  \
    void op##N##_busGainChanged();

    SYNTH_OP_SIGNALS(0)
    SYNTH_OP_SIGNALS(1)
    SYNTH_OP_SIGNALS(2)
    SYNTH_OP_SIGNALS(3)
    SYNTH_OP_SIGNALS(4)
#undef SYNTH_OP_SIGNALS

    void ampAttackChanged();
    void ampDecayChanged();
    void ampSustainChanged();
    void ampReleaseChanged();
    void filterResonanceChanged();
    void filterEnvBaseChanged();
    void filterEnvAmountChanged();
    void filterEnvAttackChanged();
    void filterEnvDecayChanged();
    void filterEnvSustainChanged();
    void filterEnvReleaseChanged();
    void atkEnvAttackChanged();
    void atkEnvDecayChanged();
    void atkEnvSustainChanged();
    void atkEnvReleaseChanged();
    void phaserRateChanged();
    void phaserDepthChanged();
    void phaserFeedbackChanged();
    void phaserWetChanged();
    void noiseGainChanged();
    void noiseColorChanged();
    void eqFrequencyChanged();
    void eqQChanged();
    void eqGainDbChanged();
    void eqTypeChanged();

private:
    void initAudioDevice(int deviceIdx);
    void applyOperator(int idx);
    void updatePartialFrequencies(float fundamental);

    std::unique_ptr<complex_fm::AudioProcess> m_process;
    complex_fm::Synth* m_synth = nullptr;
    int m_deviceIndex          = 0;

    struct OpParams
    {
        float carrierLevel = 0.7f;
        float amRate       = 0.8f;
        float amDepth      = 0.15f;
        float amEnvAmount  = 0.0f;
        float vibratoRate  = 5.8f;
        float vibratoDepth = 0.2f;
        float modFreq      = 196.0f;
        float modIdx       = 3.0f;
        float fmEnvAmount  = 0.0f;
        float busGain      = 0.7f;
    };
    OpParams m_op[5];

    float m_ampAttack  = 1.2f;
    float m_ampDecay   = 2.0f;
    float m_ampSustain = 0.0f;
    float m_ampRelease = 0.15f;

    float m_filterResonance  = 0.6f;
    float m_filterEnvBase    = 400.0f;
    float m_filterEnvAmount  = 1800.0f;
    float m_filterEnvAttack  = 1.0f;
    float m_filterEnvDecay   = 2.5f;
    float m_filterEnvSustain = 0.0f;
    float m_filterEnvRelease = 0.12f;

    float m_atkEnvAttack  = 0.01f;
    float m_atkEnvDecay   = 0.15f;
    float m_atkEnvSustain = 0.0f;
    float m_atkEnvRelease = 0.1f;

    float m_phaserRate     = 0.6f;
    float m_phaserDepth    = 0.4f;
    float m_phaserFeedback = 0.25f;
    float m_phaserWet      = 0.3f;

    float m_noiseGain = 0.09f;
    int m_noiseColor  = 3; // OneOverF3

    float m_eqFrequency = 200.0f;
    float m_eqQ         = 0.707f;
    float m_eqGainDb    = -3.0f;
    int m_eqType        = 5; // LowShelf

    float m_fundamental = 0.0f;
    bool m_noteActive   = false;

    static constexpr float kPartialOffsets[5]   = {0.0f, 1.5f, 2.5f, 3.5f, 4.5f};
    static constexpr int kPartialMultipliers[5] = {1, 2, 3, 4, 5};
};
