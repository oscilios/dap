#include "SynthBridge.h"
#include "AudioProcess.h"
#include "ComplexFMSynth.h"
#include "audioio/AudioDeviceList.h"
#include <QTimer>
#include <cmath>

SynthBridge::SynthBridge(QObject* parent)
: QObject(parent)
{
    // Default operator params
    m_op[0] = {0.7f, 0.8f, 0.15f, 0.15f, 5.8f, 0.2f, 196.0f, 3.0f, 1800.0f, 0.7f};
    m_op[1] = {0.2f, 1.0f, 0.18f, 0.12f, 6.1f, 0.18f, 394.0f, 2.5f, 1200.0f, 0.2f};
    m_op[2] = {0.3f, 1.3f, 0.2f, 0.1f, 6.4f, 0.14f, 590.5f, 2.0f, 900.0f, 0.4f};
    m_op[3] = {0.08f, 1.6f, 0.15f, 0.08f, 6.6f, 0.1f, 787.5f, 1.5f, 600.0f, 0.1f};
    m_op[4] = {0.12f, 1.9f, 0.12f, 0.06f, 6.8f, 0.08f, 984.5f, 1.2f, 450.0f, 0.15f};
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
    auto* dev = devices[idx].get();
    m_process = std::make_unique<complex_fm::AudioProcess>(
        dev->getId(), dev->getChannelCount(), dev->getBufferSize(), dev->getSampleRate());
    m_synth = &m_process->getSynth();

    // Apply parameter values to the new synth instance
    m_synth->envelope().setAttack(m_ampAttack);
    m_synth->envelope().setDecay(m_ampDecay);
    m_synth->envelope().setSustain(m_ampSustain);
    m_synth->envelope().setRelease(m_ampRelease);
    m_synth->envelope().setSampleRate(dev->getSampleRate());

    m_synth->filter().setResonance(m_filterResonance);
    m_synth->filterEnvelope().setBase(m_filterEnvBase);
    m_synth->filterEnvelope().setAmount(m_filterEnvAmount);
    m_synth->filterEnvelope().setAttack(m_filterEnvAttack);
    m_synth->filterEnvelope().setDecay(m_filterEnvDecay);
    m_synth->filterEnvelope().setSustain(m_filterEnvSustain);
    m_synth->filterEnvelope().setRelease(m_filterEnvRelease);

    m_synth->attackEnvelope().setAttack(m_atkEnvAttack);
    m_synth->attackEnvelope().setDecay(m_atkEnvDecay);
    m_synth->attackEnvelope().setSustain(m_atkEnvSustain);
    m_synth->attackEnvelope().setRelease(m_atkEnvRelease);

    m_synth->phaser().setRate(m_phaserRate);
    m_synth->phaser().setDepth(m_phaserDepth);
    m_synth->phaser().setFeedback(m_phaserFeedback);
    m_synth->phaser().setWet(m_phaserWet);

    m_synth->noise().setGain(m_noiseGain);
    {
        using Color = dap::dsp::NoiseGenerator<dap::dsp::UniformDistribution>::Color;
        m_synth->noise().setColor(static_cast<Color>(m_noiseColor));
    }

    m_synth->eq().setFrequency(m_eqFrequency);
    m_synth->eq().setQ(m_eqQ);
    m_synth->eq().setGainDb(m_eqGainDb);
    {
        using Type = dap::dsp::BiquadFilter<float>::Type;
        m_synth->eq().setType(static_cast<Type>(m_eqType));
    }

    for (int i = 0; i < 5; ++i)
        applyOperator(i);
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

void SynthBridge::applyOperator(int idx)
{
    if (!m_synth)
        return;
    auto& p    = m_op[idx];
    auto apply = [&](auto op)
    {
        op.setCarrierLevel(p.carrierLevel);
        op.setAmplitudeModRate(p.amRate);
        op.setAmplitudeModDepth(p.amDepth);
        op.setAMEnvAmount(p.amEnvAmount);
        op.setVibratoRate(p.vibratoRate);
        op.setVibratoDepth(p.vibratoDepth);
        op.setModFreq(p.modFreq);
        op.setModIdx(p.modIdx);
        op.setFMEnvAmount(p.fmEnvAmount);
    };
    switch (idx)
    {
        case 0:
            apply(m_synth->signal<0>());
            break;
        case 1:
            apply(m_synth->signal<1>());
            break;
        case 2:
            apply(m_synth->signal<2>());
            break;
        case 3:
            apply(m_synth->signal<3>());
            break;
        case 4:
            apply(m_synth->signal<4>());
            break;
    }
}

void SynthBridge::updatePartialFrequencies(float fundamental)
{
    if (!m_synth)
        return;
    auto setFreqs = [&](auto op, int idx)
    {
        float freq = fundamental * kPartialMultipliers[idx] + kPartialOffsets[idx];
        op.setFreq(freq);
        m_op[idx].modFreq = freq;
        op.setModFreq(freq);
    };
    setFreqs(m_synth->signal<0>(), 0);
    setFreqs(m_synth->signal<1>(), 1);
    setFreqs(m_synth->signal<2>(), 2);
    setFreqs(m_synth->signal<3>(), 3);
    setFreqs(m_synth->signal<4>(), 4);
}

void SynthBridge::noteOn(int midiNote)
{
    if (!m_synth)
        return;
    m_fundamental = 440.0f * std::pow(2.0f, (midiNote - 69) / 12.0f);
    updatePartialFrequencies(m_fundamental);

    if (m_noteActive)
    {
        // Retrigger: gate off, then gate on after the audio thread has processed
        // at least one buffer so the envelope sees the low→high transition
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

// --- Per-operator setter implementations ---

#define IMPL_OP_SETTER(N, FIELD, SETTER, SIGNAL, SYNTH_CALL) \
    void SynthBridge::setOp##N##_##FIELD(float v)            \
    {                                                        \
        if (qFuzzyCompare(m_op[N].FIELD, v))                 \
            return;                                          \
        m_op[N].FIELD = v;                                   \
        if (m_synth)                                         \
            m_synth->signal<N>().SYNTH_CALL;                 \
        emit op##N##_##SIGNAL();                             \
    }

#define IMPL_OP_SETTERS(N)                                                                 \
    IMPL_OP_SETTER(N, carrierLevel, carrierLevel, carrierLevelChanged, setCarrierLevel(v)) \
    IMPL_OP_SETTER(N, amRate, amRate, amRateChanged, setAmplitudeModRate(v))               \
    IMPL_OP_SETTER(N, amDepth, amDepth, amDepthChanged, setAmplitudeModDepth(v))           \
    IMPL_OP_SETTER(N, amEnvAmount, amEnvAmount, amEnvAmountChanged, setAMEnvAmount(v))     \
    IMPL_OP_SETTER(N, vibratoRate, vibratoRate, vibratoRateChanged, setVibratoRate(v))     \
    IMPL_OP_SETTER(N, vibratoDepth, vibratoDepth, vibratoDepthChanged, setVibratoDepth(v)) \
    IMPL_OP_SETTER(N, modFreq, modFreq, modFreqChanged, setModFreq(v))                     \
    IMPL_OP_SETTER(N, modIdx, modIdx, modIdxChanged, setModIdx(v))                         \
    IMPL_OP_SETTER(N, fmEnvAmount, fmEnvAmount, fmEnvAmountChanged, setFMEnvAmount(v))

IMPL_OP_SETTERS(0)
IMPL_OP_SETTERS(1)
IMPL_OP_SETTERS(2)
IMPL_OP_SETTERS(3)
IMPL_OP_SETTERS(4)

#undef IMPL_OP_SETTER
#undef IMPL_OP_SETTERS

#define IMPL_BUS_GAIN(N)                          \
    void SynthBridge::setOp##N##_busGain(float v) \
    {                                             \
        if (qFuzzyCompare(m_op[N].busGain, v))    \
            return;                               \
        m_op[N].busGain = v;                      \
        if (m_synth)                              \
            m_synth->setBusGain<N>(v);            \
        emit op##N##_busGainChanged();            \
    }

IMPL_BUS_GAIN(0)
IMPL_BUS_GAIN(1)
IMPL_BUS_GAIN(2)
IMPL_BUS_GAIN(3)
IMPL_BUS_GAIN(4)
#undef IMPL_BUS_GAIN

// --- Scalar property setters ---

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

IMPL_FLOAT_SETTER(AmpAttack, m_ampAttack, envelope().setAttack(v), ampAttackChanged)
IMPL_FLOAT_SETTER(AmpDecay, m_ampDecay, envelope().setDecay(v), ampDecayChanged)
IMPL_FLOAT_SETTER(AmpSustain, m_ampSustain, envelope().setSustain(v), ampSustainChanged)
IMPL_FLOAT_SETTER(AmpRelease, m_ampRelease, envelope().setRelease(v), ampReleaseChanged)

IMPL_FLOAT_SETTER(FilterResonance,
                  m_filterResonance,
                  filter().setResonance(v),
                  filterResonanceChanged)
IMPL_FLOAT_SETTER(FilterEnvBase, m_filterEnvBase, filterEnvelope().setBase(v), filterEnvBaseChanged)
IMPL_FLOAT_SETTER(FilterEnvAmount,
                  m_filterEnvAmount,
                  filterEnvelope().setAmount(v),
                  filterEnvAmountChanged)
IMPL_FLOAT_SETTER(FilterEnvAttack,
                  m_filterEnvAttack,
                  filterEnvelope().setAttack(v),
                  filterEnvAttackChanged)
IMPL_FLOAT_SETTER(FilterEnvDecay,
                  m_filterEnvDecay,
                  filterEnvelope().setDecay(v),
                  filterEnvDecayChanged)
IMPL_FLOAT_SETTER(FilterEnvSustain,
                  m_filterEnvSustain,
                  filterEnvelope().setSustain(v),
                  filterEnvSustainChanged)
IMPL_FLOAT_SETTER(FilterEnvRelease,
                  m_filterEnvRelease,
                  filterEnvelope().setRelease(v),
                  filterEnvReleaseChanged)

IMPL_FLOAT_SETTER(AtkEnvAttack, m_atkEnvAttack, attackEnvelope().setAttack(v), atkEnvAttackChanged)
IMPL_FLOAT_SETTER(AtkEnvDecay, m_atkEnvDecay, attackEnvelope().setDecay(v), atkEnvDecayChanged)
IMPL_FLOAT_SETTER(AtkEnvSustain,
                  m_atkEnvSustain,
                  attackEnvelope().setSustain(v),
                  atkEnvSustainChanged)
IMPL_FLOAT_SETTER(AtkEnvRelease,
                  m_atkEnvRelease,
                  attackEnvelope().setRelease(v),
                  atkEnvReleaseChanged)

IMPL_FLOAT_SETTER(PhaserRate, m_phaserRate, phaser().setRate(v), phaserRateChanged)
IMPL_FLOAT_SETTER(PhaserDepth, m_phaserDepth, phaser().setDepth(v), phaserDepthChanged)
IMPL_FLOAT_SETTER(PhaserFeedback, m_phaserFeedback, phaser().setFeedback(v), phaserFeedbackChanged)
IMPL_FLOAT_SETTER(PhaserWet, m_phaserWet, phaser().setWet(v), phaserWetChanged)

IMPL_FLOAT_SETTER(NoiseGain, m_noiseGain, noise().setGain(v), noiseGainChanged)

IMPL_FLOAT_SETTER(EqFrequency, m_eqFrequency, eq().setFrequency(v), eqFrequencyChanged)
IMPL_FLOAT_SETTER(EqQ, m_eqQ, eq().setQ(v), eqQChanged)
IMPL_FLOAT_SETTER(EqGainDb, m_eqGainDb, eq().setGainDb(v), eqGainDbChanged)

#undef IMPL_FLOAT_SETTER

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

void SynthBridge::setEqType(int v)
{
    if (m_eqType == v)
        return;
    m_eqType = v;
    if (m_synth)
    {
        using Type = dap::dsp::BiquadFilter<float>::Type;
        m_synth->eq().setType(static_cast<Type>(v));
    }
    emit eqTypeChanged();
}
