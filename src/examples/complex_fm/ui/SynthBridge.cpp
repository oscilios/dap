#include "SynthBridge.h"
#include "AudioProcess.h"
#include "ComplexFMSynth.h"
#include "audioio/AudioDeviceList.h"
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

    initAudioDevice(m_deviceIndex);
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
    m_noteActive  = true;
    updatePartialFrequencies(m_fundamental);
    m_synth->envelope().setGate(1.0f);
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
    IMPL_OP_SETTER(N, fmEnvAmount, fmEnvAmount, fmEnvAmountChanged, setFMEnvAmount(v))     \
    IMPL_OP_SETTER(N, busGain, busGain, busGainChanged, /* handled below */)

// busGain needs special handling — it sets the mixer bus gain, not an Operator method
// We'll override the macro-generated version below.

// First define all but busGain via a narrower macro
#undef IMPL_OP_SETTERS

#define IMPL_OP_SETTERS_NO_BUS(N)                                                          \
    IMPL_OP_SETTER(N, carrierLevel, carrierLevel, carrierLevelChanged, setCarrierLevel(v)) \
    IMPL_OP_SETTER(N, amRate, amRate, amRateChanged, setAmplitudeModRate(v))               \
    IMPL_OP_SETTER(N, amDepth, amDepth, amDepthChanged, setAmplitudeModDepth(v))           \
    IMPL_OP_SETTER(N, amEnvAmount, amEnvAmount, amEnvAmountChanged, setAMEnvAmount(v))     \
    IMPL_OP_SETTER(N, vibratoRate, vibratoRate, vibratoRateChanged, setVibratoRate(v))     \
    IMPL_OP_SETTER(N, vibratoDepth, vibratoDepth, vibratoDepthChanged, setVibratoDepth(v)) \
    IMPL_OP_SETTER(N, modFreq, modFreq, modFreqChanged, setModFreq(v))                     \
    IMPL_OP_SETTER(N, modIdx, modIdx, modIdxChanged, setModIdx(v))                         \
    IMPL_OP_SETTER(N, fmEnvAmount, fmEnvAmount, fmEnvAmountChanged, setFMEnvAmount(v))

IMPL_OP_SETTERS_NO_BUS(0)
IMPL_OP_SETTERS_NO_BUS(1)
IMPL_OP_SETTERS_NO_BUS(2)
IMPL_OP_SETTERS_NO_BUS(3)
IMPL_OP_SETTERS_NO_BUS(4)

#undef IMPL_OP_SETTER
#undef IMPL_OP_SETTERS_NO_BUS

// busGain: sets mixer bus level (not part of Operator accessor class)
// The bus gain is set through the graph directly. We need to access it via the mixer.
// Looking at ComplexFMSynth.cpp line 6-11, bus gain is set through:
// m_graph.input("signal"_s)...input<N>().input("gain"_s).input("value"_s)
// But we don't have direct access from SynthBridge — we'll skip the synth call for busGain
// since it requires graph-level access. The Operator class doesn't expose it.
// For now, store the value for UI state only.

#define IMPL_BUS_GAIN(N)                          \
    void SynthBridge::setOp##N##_busGain(float v) \
    {                                             \
        if (qFuzzyCompare(m_op[N].busGain, v))    \
            return;                               \
        m_op[N].busGain = v;                      \
        emit op##N##_busGainChanged();            \
    }

IMPL_BUS_GAIN(0)
IMPL_BUS_GAIN(1)
IMPL_BUS_GAIN(2)
IMPL_BUS_GAIN(3)
IMPL_BUS_GAIN(4)
#undef IMPL_BUS_GAIN

// --- Amplitude envelope ---

void SynthBridge::setAmpAttack(float v)
{
    if (qFuzzyCompare(m_ampAttack, v))
        return;
    m_ampAttack = v;
    if (m_synth)
        m_synth->envelope().setAttack(v);
    emit ampAttackChanged();
}

void SynthBridge::setAmpDecay(float v)
{
    if (qFuzzyCompare(m_ampDecay, v))
        return;
    m_ampDecay = v;
    if (m_synth)
        m_synth->envelope().setDecay(v);
    emit ampDecayChanged();
}

void SynthBridge::setAmpSustain(float v)
{
    if (qFuzzyCompare(m_ampSustain, v))
        return;
    m_ampSustain = v;
    if (m_synth)
        m_synth->envelope().setSustain(v);
    emit ampSustainChanged();
}

void SynthBridge::setAmpRelease(float v)
{
    if (qFuzzyCompare(m_ampRelease, v))
        return;
    m_ampRelease = v;
    if (m_synth)
        m_synth->envelope().setRelease(v);
    emit ampReleaseChanged();
}

// --- Filter ---

void SynthBridge::setFilterResonance(float v)
{
    if (qFuzzyCompare(m_filterResonance, v))
        return;
    m_filterResonance = v;
    if (m_synth)
        m_synth->filter().setResonance(v);
    emit filterResonanceChanged();
}

void SynthBridge::setFilterEnvBase(float v)
{
    if (qFuzzyCompare(m_filterEnvBase, v))
        return;
    m_filterEnvBase = v;
    if (m_synth)
        m_synth->filterEnvelope().setBase(v);
    emit filterEnvBaseChanged();
}

void SynthBridge::setFilterEnvAmount(float v)
{
    if (qFuzzyCompare(m_filterEnvAmount, v))
        return;
    m_filterEnvAmount = v;
    if (m_synth)
        m_synth->filterEnvelope().setAmount(v);
    emit filterEnvAmountChanged();
}

void SynthBridge::setFilterEnvAttack(float v)
{
    if (qFuzzyCompare(m_filterEnvAttack, v))
        return;
    m_filterEnvAttack = v;
    if (m_synth)
        m_synth->filterEnvelope().setAttack(v);
    emit filterEnvAttackChanged();
}

void SynthBridge::setFilterEnvDecay(float v)
{
    if (qFuzzyCompare(m_filterEnvDecay, v))
        return;
    m_filterEnvDecay = v;
    if (m_synth)
        m_synth->filterEnvelope().setDecay(v);
    emit filterEnvDecayChanged();
}

void SynthBridge::setFilterEnvSustain(float v)
{
    if (qFuzzyCompare(m_filterEnvSustain, v))
        return;
    m_filterEnvSustain = v;
    if (m_synth)
        m_synth->filterEnvelope().setSustain(v);
    emit filterEnvSustainChanged();
}

void SynthBridge::setFilterEnvRelease(float v)
{
    if (qFuzzyCompare(m_filterEnvRelease, v))
        return;
    m_filterEnvRelease = v;
    if (m_synth)
        m_synth->filterEnvelope().setRelease(v);
    emit filterEnvReleaseChanged();
}

// --- Attack envelope ---

void SynthBridge::setAtkEnvAttack(float v)
{
    if (qFuzzyCompare(m_atkEnvAttack, v))
        return;
    m_atkEnvAttack = v;
    if (m_synth)
        m_synth->attackEnvelope().setAttack(v);
    emit atkEnvAttackChanged();
}

void SynthBridge::setAtkEnvDecay(float v)
{
    if (qFuzzyCompare(m_atkEnvDecay, v))
        return;
    m_atkEnvDecay = v;
    if (m_synth)
        m_synth->attackEnvelope().setDecay(v);
    emit atkEnvDecayChanged();
}

void SynthBridge::setAtkEnvSustain(float v)
{
    if (qFuzzyCompare(m_atkEnvSustain, v))
        return;
    m_atkEnvSustain = v;
    if (m_synth)
        m_synth->attackEnvelope().setSustain(v);
    emit atkEnvSustainChanged();
}

void SynthBridge::setAtkEnvRelease(float v)
{
    if (qFuzzyCompare(m_atkEnvRelease, v))
        return;
    m_atkEnvRelease = v;
    if (m_synth)
        m_synth->attackEnvelope().setRelease(v);
    emit atkEnvReleaseChanged();
}

// --- Phaser ---

void SynthBridge::setPhaserRate(float v)
{
    if (qFuzzyCompare(m_phaserRate, v))
        return;
    m_phaserRate = v;
    if (m_synth)
        m_synth->phaser().setRate(v);
    emit phaserRateChanged();
}

void SynthBridge::setPhaserDepth(float v)
{
    if (qFuzzyCompare(m_phaserDepth, v))
        return;
    m_phaserDepth = v;
    if (m_synth)
        m_synth->phaser().setDepth(v);
    emit phaserDepthChanged();
}

void SynthBridge::setPhaserFeedback(float v)
{
    if (qFuzzyCompare(m_phaserFeedback, v))
        return;
    m_phaserFeedback = v;
    if (m_synth)
        m_synth->phaser().setFeedback(v);
    emit phaserFeedbackChanged();
}

void SynthBridge::setPhaserWet(float v)
{
    if (qFuzzyCompare(m_phaserWet, v))
        return;
    m_phaserWet = v;
    if (m_synth)
        m_synth->phaser().setWet(v);
    emit phaserWetChanged();
}

// --- Noise ---

void SynthBridge::setNoiseGain(float v)
{
    if (qFuzzyCompare(m_noiseGain, v))
        return;
    m_noiseGain = v;
    if (m_synth)
        m_synth->noise().setGain(v);
    emit noiseGainChanged();
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

// --- EQ ---

void SynthBridge::setEqFrequency(float v)
{
    if (qFuzzyCompare(m_eqFrequency, v))
        return;
    m_eqFrequency = v;
    if (m_synth)
        m_synth->eq().setFrequency(v);
    emit eqFrequencyChanged();
}

void SynthBridge::setEqQ(float v)
{
    if (qFuzzyCompare(m_eqQ, v))
        return;
    m_eqQ = v;
    if (m_synth)
        m_synth->eq().setQ(v);
    emit eqQChanged();
}

void SynthBridge::setEqGainDb(float v)
{
    if (qFuzzyCompare(m_eqGainDb, v))
        return;
    m_eqGainDb = v;
    if (m_synth)
        m_synth->eq().setGainDb(v);
    emit eqGainDbChanged();
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
