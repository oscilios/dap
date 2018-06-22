#ifndef DAP_AUDIO_IO_COREAUDIO_AUDIO_OUTPUT_H
#define DAP_AUDIO_IO_COREAUDIO_AUDIO_OUTPUT_H

#include "AudioDevice.h"
#include "audioio/IAudioBus.h"
#include <AudioToolbox/AudioToolbox.h>
#include <atomic>

namespace dap
{
    namespace coreaudio
    {
        class AudioOutput;
        class AudioDeviceListener;
    }
    namespace audioio
    {
        class IAudioProcess;
    }
}

class dap::coreaudio::AudioOutput : public dap::audioio::IAudioBus
{
    std::atomic<bool> m_mtx{false};
    bool m_stopRequested;
    size_t m_samplesProcessed;
    std::vector<float*> m_outputs;
    coreaudio::AudioDevice m_device;
    std::unique_ptr<AudioDeviceListener> m_listener;

    AUGraph m_auGraph;
    AUNode m_auNode;
    AudioUnit m_auUnit;
    audioio::IAudioProcess* m_audioProcess;

    OSStatus setupGraph();
    OSStatus makeGraph();
    OSStatus setDeviceAsCurrent();
    OSStatus setupBuffers();
    OSStatus init();
    void cleanup();
    bool isRunning() const;

    static OSStatus renderCallback(void* inRefCon,
                                   AudioUnitRenderActionFlags* ioActionFlags,
                                   const AudioTimeStamp* inTimeStamp,
                                   UInt32 inBusNumber,
                                   UInt32 inNumberFrames,
                                   AudioBufferList* ioData);

public:
    AudioOutput(AudioDeviceID output, uint32_t bufferSize, float sampleRate);
    AudioOutput(const AudioOutput&) = delete;
    AudioOutput(AudioOutput&&)      = delete;
    ~AudioOutput() override;
    AudioOutput operator=(const AudioOutput&) = delete;
    AudioOutput operator=(AudioOutput&&) = delete;
    bool start() override;
    bool stop() override;
    void setAudioProcess(audioio::IAudioProcess* p) override;
    void removeAudioProcess() override;
};

#endif // DAP_AUDIO_IO_COREAUDIO_AUDIO_OUTPUT_H
