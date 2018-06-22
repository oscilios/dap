#ifndef DAP_AUDIO_IO_TEST_PASS_THROUGH_AUDIO_PROCESS_H
#define DAP_AUDIO_IO_TEST_PASS_THROUGH_AUDIO_PROCESS_H

#include "audioio/IAudioProcess.h"
#include "audioio/IAudioBus.h"
#include "fastmath/AudioBuffer.h"

namespace dap
{
    namespace audioio
    {
        class PassThroughAudioProcess;
    }
}

class dap::audioio::PassThroughAudioProcess : public dap::audioio::IAudioProcess
{
    using Buffer      = fastmath::AudioBuffer<float>;
    using BufferQueue = std::vector<Buffer>;
    const std::unique_ptr<IAudioBus> m_input;
    const std::unique_ptr<IAudioBus> m_output;
    const size_t m_inputChannelCount;
    const size_t m_outputChannelCount;
    const size_t m_bufferSize;
    std::atomic<int64_t> m_rIdx{0};
    std::atomic<int64_t> m_wIdx{0};
    const int m_queueSize;
    const int m_bitmask;
    float** m_outputs;
    BufferQueue m_inputQueue;
    const Buffer m_silence;
    void writeSilence();

public:
    PassThroughAudioProcess(int32_t inputId,
                            int32_t outputId,
                            size_t inputChannelCount,
                            size_t outputChannelCount,
                            size_t bufferSize,
                            float sampleRate);
    PassThroughAudioProcess(const PassThroughAudioProcess&) = delete;
    PassThroughAudioProcess(PassThroughAudioProcess&&) = delete;
    ~PassThroughAudioProcess() override;
    PassThroughAudioProcess& operator=(const PassThroughAudioProcess&) = delete;
    PassThroughAudioProcess& operator=(PassThroughAudioProcess&&) = delete;
    bool process() override;
    void setInputs(float const* const* inputs) override;
    void setOutputs(float** outputs) override;
    bool start() override;
    bool stop() override;
};
#endif // DAP_AUDIO_IO_TEST_PASS_THROUGH_AUDIO_PROCESS_H
