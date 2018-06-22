#include "PassThroughAudioProcess.h"
#include "audioio/AudioBusFactory.h"

using dap::audioio::PassThroughAudioProcess;
using dap::audioio::AudioBusFactory;
PassThroughAudioProcess::PassThroughAudioProcess(int32_t inputId,
                                                 int32_t outputId,
                                                 size_t inputChannelCount,
                                                 size_t outputChannelCount,
                                                 size_t bufferSize,
                                                 float sampleRate)
: m_input(AudioBusFactory::create(Scope::Input, inputId, bufferSize, sampleRate))
, m_output(AudioBusFactory::create(Scope::Output, outputId, bufferSize, sampleRate))
, m_inputChannelCount(inputChannelCount)
, m_outputChannelCount(outputChannelCount)
, m_bufferSize(bufferSize)
, m_queueSize(16)
, m_bitmask(m_queueSize - 1)
, m_inputQueue(m_queueSize, Buffer(m_inputChannelCount, bufferSize))
, m_silence(1, m_bufferSize, 0.0f)
{
}
PassThroughAudioProcess::~PassThroughAudioProcess() = default;
void PassThroughAudioProcess::writeSilence()
{
    const size_t channelCount = std::min(m_inputChannelCount, m_outputChannelCount);
    for (size_t ch = 0; ch < channelCount; ++ch)
    {
        std::memcpy(m_outputs[ch], m_silence.data(), sizeof(float) * m_bufferSize);
    }
}
bool PassThroughAudioProcess::process()
{
    if (m_wIdx <= m_rIdx)
    {
        writeSilence();
        return false;
    }
    const Buffer& inputBuffer = m_inputQueue[m_rIdx & m_bitmask];
    ++m_rIdx;
    const size_t channelCount = std::min(m_inputChannelCount, m_outputChannelCount);
    for (size_t ch = 0; ch < channelCount; ++ch)
    {
        std::memcpy(m_outputs[ch], inputBuffer.channel(ch).data(), sizeof(float) * m_bufferSize);
    }
    return true;
}
void PassThroughAudioProcess::setInputs(float const* const* const inputs)
{
    assert(m_wIdx >= m_rIdx);
    Buffer& inputBuffer = m_inputQueue[m_wIdx & m_bitmask];
    ++m_wIdx;
    for (size_t ch = 0; ch < m_inputChannelCount; ++ch)
    {
        inputBuffer.channel(ch).copy(inputs[ch], m_bufferSize);
    }
}
void PassThroughAudioProcess::setOutputs(float** outputs)
{
    m_outputs = outputs;
}
bool PassThroughAudioProcess::start()
{
    m_input->setAudioProcess(this);
    m_output->setAudioProcess(this);
    return m_input->start() && m_output->start();
}
bool PassThroughAudioProcess::stop()
{
    const auto result = m_input->stop() && m_output->stop();
    m_input->removeAudioProcess();
    m_output->removeAudioProcess();
    return result;
}
