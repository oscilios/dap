#ifndef DAP_AUDIO_IO_CORAUDIO_AUDIO_DEVICE_LISTENER_H
#define DAP_AUDIO_IO_CORAUDIO_AUDIO_DEVICE_LISTENER_H

#include <CoreAudio/CoreAudio.h>
#include <string>

namespace dap
{
    class CoreAudioDriverImpl;
    namespace coreaudio
    {
        class AudioDeviceListener;
    }
}

class dap::coreaudio::AudioDeviceListener
{
    // Listens to the following property changes on an audio device
    // -kAudioObjectPropertyControlLis
    // -kAudioDevicePropertyDeviceIsRunning
    // -kAudioDevicePropertyStreams
    // -kAudioDevicePropertyDeviceHasChanged
    // -kAudioDeviceProcessorOverload
    // -kAudioDevicePropertyDeviceIsAlive
    // -kAudioDevicePropertyDataSource // to detect if headphones are plugged in

    AudioDeviceID m_id;
    const std::string m_name;

    bool addListener(AudioObjectID objectId, AudioObjectPropertyAddress const* const address);
    bool removeListener(AudioObjectID objectId, AudioObjectPropertyAddress const* const address);
    void addDeviceListeners();
    void removeDeviceListeners();

    static OSStatus
    onAudioObjectPropertyNotification(AudioObjectID inObjectID,
                                      UInt32 inNumberAddresses,
                                      const AudioObjectPropertyAddress inAddresses[],
                                      void* inClientData);

public:
    AudioDeviceListener(AudioDeviceID deviceId, std::string name);
    AudioDeviceListener(const AudioDeviceListener&) = delete;
    AudioDeviceListener(AudioDeviceListener&&)      = delete;
    ~AudioDeviceListener();
    AudioDeviceListener& operator=(const AudioDeviceListener&) = delete;
    AudioDeviceListener& operator=(AudioDeviceListener&&) = delete;
};
#endif // DAP_AUDIO_IO_CORAUDIO_AUDIO_DEVICE_LISTENER_H
