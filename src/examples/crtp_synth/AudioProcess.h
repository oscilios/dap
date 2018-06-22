#ifndef DAP_EXAMPLES_CRTP_SYNTH_AUDIO_PROCESS_H
#define DAP_EXAMPLES_CRTP_SYNTH_AUDIO_PROCESS_H

#include "Synth.h"
#include "audioio/IAudioBus.h"
#include "audioio/IAudioProcess.h"
#include "base/Any.h"
#include <iostream>

namespace crtp_synth
{
    class AudioProcess;
}

class crtp_synth::AudioProcess final : public dap::audioio::IAudioProcess
{
    const std::unique_ptr<dap::audioio::IAudioBus> m_outputBus;
    const size_t m_outputChannelCount;
    float** m_outputs;
    Synth m_synth;

public:
    AudioProcess(int32_t outputId, size_t outputChannelCount, size_t bufferSize, float sampleRate);
    AudioProcess(const AudioProcess&) = delete;
    AudioProcess(AudioProcess&&)      = delete;
    ~AudioProcess() override;
    AudioProcess& operator=(const AudioProcess&) = delete;
    AudioProcess& operator=(AudioProcess&&) = delete;
    bool process() override;
    void setInputs(float const* const* inputs) override;
    void setOutputs(float** outputs) override;
    bool start() override;
    bool stop() override;

    template <char... Chars>
    constexpr auto& operator[](dap::constexpr_string<Chars...> s)
    {
        return m_synth[s];
    }
};
#endif // DAP_EXAMPLES_CRTP_SYNTH_AUDIO_PROCESS_H
