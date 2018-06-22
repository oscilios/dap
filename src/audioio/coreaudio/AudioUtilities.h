#ifndef DAP_AUDIO_IO_COREAUDIO_AUDIO_UTILITIES_H
#define DAP_AUDIO_IO_COREAUDIO_AUDIO_UTILITIES_H

#include "AudioDevice.h"
#include <cassert>
#include <iostream>
#include <vector>

#define FourCC2Str(code)                                                             \
    (char[5])                                                                        \
    {                                                                                \
        ((code) >> 24) & 0xFF, ((code) >> 16) & 0xFF, ((code) >> 8) & 0xFF, (code) & 0xFF, 0 \
    }

#ifndef NDEBUG
#define DAP_ASSERT_NOERR(err)                                                              \
    if (err != noErr)                                                                      \
        printf("%s, line %d: Error code %i\n", __FILE__, __LINE__, static_cast<int>(err)); \
    assert(err == noErr);
#else
#define DAP_ASSERT_NOERR(err) \
    do                        \
    {                         \
        if (0 != (err))       \
        {                     \
        }                     \
    } while (false)
#endif

namespace dap
{
    namespace coreaudio
    {
        void MakeBufferSilent(AudioBufferList* ioData);
        std::ostream& operator<<(std::ostream& out, const AudioStreamBasicDescription& asbd);
        std::vector<std::unique_ptr<dap::audioio::IAudioDevice>> createInputAudioDeviceList();
        std::vector<std::unique_ptr<dap::audioio::IAudioDevice>> createOutputAudioDeviceList();
        template <typename T>
        inline bool setProperty(AudioObjectID objectId,
                                AudioObjectPropertyAddress const* const address,
                                const T* const value)
        {
            UInt32 propertySize = sizeof(T);
            return AudioObjectSetPropertyData(objectId, address, 0, nullptr, propertySize, value) ==
                   noErr;
        }
        template <typename T>
        inline bool getProperty(AudioObjectID objectId,
                                AudioObjectPropertyAddress const* const address,
                                T* result)
        {
            UInt32 propertySize = sizeof(T);
            return AudioObjectGetPropertyData(objectId, address, 0, nullptr, &propertySize, result) ==
                   noErr;
        }
        template <typename T>
        inline bool getPropertyList(AudioObjectID objectId,
                                    AudioObjectPropertyAddress const* const address,
                                    std::vector<T>* result)
        {
            UInt32 propertySize = result->size() * sizeof(T);
            return AudioObjectGetPropertyData(
                       objectId, address, 0, nullptr, &propertySize, result->data()) == noErr;
        }
        inline UInt32 getPropertySize(AudioObjectID objectId,
                                      AudioObjectPropertyAddress const* const address)
        {
            UInt32 result = 0;
            AudioObjectGetPropertyDataSize(objectId, address, 0, nullptr, &result);
            return result;
        }
    }
}
#endif // DAP_AUDIO_IO_COREAUDIO_AUDIO_UTILITIES_H
