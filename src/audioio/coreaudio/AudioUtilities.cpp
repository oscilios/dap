#include "AudioUtilities.h"
#include <CoreServices/CoreServices.h>
#include <ostream>

using dap::coreaudio::AudioDevice;
using dap::audioio::IAudioDevice;

static std::vector<std::unique_ptr<IAudioDevice>> createDeviceInfoList(AudioObjectPropertyScope scope)
{
    AudioObjectPropertyAddress address = {kAudioHardwarePropertyDevices,
                                          kAudioObjectPropertyScopeGlobal,
                                          kAudioObjectPropertyElementMain};

    auto propertySize = dap::coreaudio::getPropertySize(kAudioObjectSystemObject, &address);
    const int deviceCount = propertySize / sizeof(AudioDeviceID);
    std::vector<AudioDeviceID> ids(deviceCount);
    dap::coreaudio::getPropertyList(kAudioObjectSystemObject, &address, &ids);

    std::vector<std::unique_ptr<IAudioDevice>> devices;
    for (const auto id : ids)
    {
        std::unique_ptr<IAudioDevice> dev = std::make_unique<AudioDevice>(id, scope);
        if (dev->getChannelCount() > 0)
        {
            devices.emplace_back(std::move(dev));
        }
    }
    return devices;
}

namespace dap
{

    namespace coreaudio
    {

        void MakeBufferSilent(AudioBufferList* ioData)
        {
            for (UInt32 i = 0; i < ioData->mNumberBuffers; i++)
                memset(ioData->mBuffers[i].mData, 0, ioData->mBuffers[i].mDataByteSize);
        }

        std::ostream& operator<<(std::ostream& out, const AudioStreamBasicDescription& asbd)
        {
            out << "\n\tBitsPerChannel   :" << asbd.mBitsPerChannel
                << "\n\tBytesPerFrame    :" << asbd.mBytesPerFrame
                << "\n\tBytesPerPacket   :" << asbd.mBytesPerPacket
                << "\n\tChannelsPerFrame :" << asbd.mChannelsPerFrame
                << "\n\tFormatFlags      :" << asbd.mFormatFlags
                << "\n\tFormatID         :" << asbd.mFormatID
                << "\n\tFramesPerPacket  :" << asbd.mFramesPerPacket
                << "\n\tReserved         :" << asbd.mReserved
                << "\n\tSampleRate       :" << asbd.mSampleRate;
            return out;
        }
        std::vector<std::unique_ptr<IAudioDevice>> createInputAudioDeviceList()
        {
            return createDeviceInfoList(kAudioDevicePropertyScopeInput);
        }
        std::vector<std::unique_ptr<IAudioDevice>> createOutputAudioDeviceList()
        {
            return createDeviceInfoList(kAudioDevicePropertyScopeOutput);
        }

    }
}
