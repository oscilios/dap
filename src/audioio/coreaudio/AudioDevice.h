#ifndef DAP_AUDIO_IO_COREAUDIO_AUDIODEVICE_H
#define DAP_AUDIO_IO_COREAUDIO_AUDIODEVICE_H

#include "audioio/IAudioDevice.h"
#include <CoreAudio/CoreAudio.h>

namespace dap
{
    namespace coreaudio
    {
        class AudioDevice;
    }
}

class dap::coreaudio::AudioDevice : public dap::audioio::IAudioDevice
{
    AudioDeviceID m_ID{};
    AudioObjectPropertyScope m_scope{};
    uint32_t m_channelCount{};
    uint32_t m_bufferSize{};
    std::vector<uint32_t> m_bufferSizeList{};
    Float64 m_sampleRate{};
    std::vector<float> m_sampleRateList{};
    AudioStreamBasicDescription m_format{};
    bool m_supportsFloat{};
    bool m_supportsInt{};
    uint32_t m_safetyOffset{};
    uint32_t m_latency{};
    std::string m_name{};
    std::string m_manufacturer{};

    void init(AudioDeviceID id, AudioObjectPropertyScope scope);

public:
    AudioDevice() = default;
    AudioDevice(AudioDeviceID devid, AudioObjectPropertyScope scope);
    bool setBufferSize(uint32_t size) override;
    bool setSampleRate(float sampleRate) override;
    bool setExclusiveAccess() override;

    int32_t getId() const override
    {
        return static_cast<int32_t>(m_ID);
    }
    bool isValid() const override
    {
        return m_ID != kAudioDeviceUnknown;
    }
    uint32_t getBufferSize() const override
    {
        return m_bufferSize;
    }
    uint32_t getChannelCount() const override
    {
        return m_channelCount;
    }
    float getSampleRate() const override
    {
        return static_cast<float>(m_sampleRate);
    }
    std::vector<uint32_t> getSupportedBufferSizes() const override
    {
        return m_bufferSizeList;
    }
    std::vector<float> getSupportedSampleRates() const override
    {
        return m_sampleRateList;
    }
    const std::string& getName() const override
    {
        return m_name;
    }
    const std::string& getManufacturer() const override
    {
        return m_manufacturer;
    }
    bool getFormatFloatSupported() const override
    {
        return m_supportsFloat;
    }
    bool getFormatIntSupported() const override
    {
        return m_supportsInt;
    }
    uint32_t getSafetyOffset() const override
    {
        return m_safetyOffset;
    }
    uint32_t getLatency() const override
    {
        return m_latency;
    }
};

#endif // DAP_AUDIO_IO_COREAUDIO_AUDIODEVICE_H
