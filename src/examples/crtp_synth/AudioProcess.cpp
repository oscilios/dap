#include "AudioProcess.h"
#include "audioio/AudioBusFactory.h"
#include "base/SystemCommon.h"

using crtp_synth::AudioProcess;
using dap::audioio::AudioBusFactory;

AudioProcess::AudioProcess(int32_t outputId,
                           size_t outputChannelCount,
                           size_t bufferSize,
                           float sampleRate)
: m_outputBus(AudioBusFactory::create(dap::audioio::Scope::Output, outputId, bufferSize, sampleRate))
, m_outputChannelCount(outputChannelCount)
, m_synth(bufferSize, sampleRate)
{
}

AudioProcess::~AudioProcess() = default;
bool AudioProcess::process()
{
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

    m_synth.process();
    const auto& buf = m_synth.output();
    for (size_t ch = 0; ch < m_outputChannelCount; ++ch)
    {
        std::memcpy(m_outputs[ch], buf.channel(0).data(), sizeof(float) * buf.channelSize());
    }
    return true;
}
void AudioProcess::setInputs(float const* const* const inputs)
{
}
void AudioProcess::setOutputs(float** outputs)
{
    m_outputs = outputs;
}
bool AudioProcess::start()
{
    m_outputBus->setAudioProcess(this);
    return m_outputBus->start();
}
bool AudioProcess::stop()
{
    const auto result = m_outputBus->stop();
    m_outputBus->removeAudioProcess();
    return result;
}
