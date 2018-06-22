#include "AudioBusFactory.h"

#ifdef __APPLE__
#include "coreaudio/AudioInput.h"
#include "coreaudio/AudioOutput.h"
#else
// TODO
#endif

std::unique_ptr<dap::audioio::IAudioBus>
dap::audioio::AudioBusFactory::create(Scope scope,
                                      int32_t deviceId,
                                      size_t bufferSize,
                                      float sampleRate)
{
#ifdef __APPLE__
    if (scope == dap::audioio::Scope::Input)
        return std::make_unique<dap::coreaudio::AudioInput>(deviceId, bufferSize, sampleRate);
    return std::make_unique<dap::coreaudio::AudioOutput>(deviceId, bufferSize, sampleRate);
#else
    static_assert(false, "AudioBusFactory not implemented for this platform");
    return nullptr;
#endif
}
