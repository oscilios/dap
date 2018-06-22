#ifndef DAP_AUDIO_IO_COREAUDIO_AUDIO_INPUT_H
#define DAP_AUDIO_IO_COREAUDIO_AUDIO_INPUT_H

#include "AudioDevice.h"
#include "audioio/IAudioBus.h"
#include <AudioToolbox/AudioToolbox.h>
#include <atomic>

namespace dap
{
    namespace coreaudio
    {
        class AudioInput;
        class AudioDeviceListener;
    }
    namespace audioio
    {
        class IAudioProcess;
    }
}

class dap::coreaudio::AudioInput : public dap::audioio::IAudioBus
{
    std::atomic<bool> m_mtx{false};
    bool m_stopRequested;
    size_t m_samplesProcessed;
    AudioBufferList* m_buffer;
    std::vector<float*> m_inputs;
    coreaudio::AudioDevice m_device;
    std::unique_ptr<AudioDeviceListener> m_listener;

    AudioUnit m_auUnit;
    audioio::IAudioProcess* m_audioProcess;

    OSStatus setupAUHAL();
    OSStatus setDeviceAsCurrent();
    OSStatus enableIO();
    OSStatus setupCallback();
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
    AudioInput(AudioDeviceID input, uint32_t bufferSize, float sampleRate);
    AudioInput(const AudioInput&) = delete;
    AudioInput(AudioInput&&)      = delete;
    ~AudioInput() override;
    AudioInput& operator=(const AudioInput&) = delete;
    AudioInput& operator=(AudioInput&&) = delete;
    bool start() override;
    bool stop() override;
    void setAudioProcess(audioio::IAudioProcess* p) override;
    void removeAudioProcess() override;
};

#endif // DAP_AUDIO_IO_COREAUDIO_AUDIO_INPUT_H
