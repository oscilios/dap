#include "AudioDeviceList.h"

#ifdef __APPLE__
#include "coreaudio/AudioUtilities.h"
#else
// TODO
#endif

std::vector<std::unique_ptr<dap::audioio::IAudioDevice>>
dap::audioio::AudioDeviceList::create(Scope scope)
{
#ifdef __APPLE__
    if (scope == Scope::Input)
        return dap::coreaudio::createInputAudioDeviceList();
    return dap::coreaudio::createOutputAudioDeviceList();
#else
    static_assert(false, "AudioDeviceList::create not implemented for this platform");
    return {};
#endif
}
