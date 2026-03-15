#pragma once

#include "Sub37Synth.h"
#include "audioio/IAudioBus.h"
#include "audioio/IAudioProcess.h"
#include "base/Any.h"

namespace sub37
{
    class AudioProcess;
}

class sub37::AudioProcess final : public dap::audioio::IAudioProcess
{
    const std::unique_ptr<dap::audioio::IAudioBus> m_outputBus;
    const size_t m_outputChannelCount;
    float** m_outputs;
    sub37::Synth m_synth;

public:
    AudioProcess(int32_t outputId, size_t outputChannelCount, size_t bufferSize, float sampleRate);
    AudioProcess(const AudioProcess&) = delete;
    AudioProcess(AudioProcess&&)      = delete;
    ~AudioProcess() override;
    AudioProcess& operator=(const AudioProcess&) = delete;
    AudioProcess& operator=(AudioProcess&&)      = delete;
    bool process() override;
    void setInputs(float const* const* inputs) override;
    void setOutputs(float** outputs) override;
    bool start() override;
    bool stop() override;

    Synth& getSynth()
    {
        return m_synth;
    }
};
