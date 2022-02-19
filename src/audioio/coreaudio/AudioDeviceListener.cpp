#include "AudioDeviceListener.h"
#include "AudioUtilities.h"
#include <CoreServices/CoreServices.h>
#include <iostream>

using dap::coreaudio::AudioDeviceListener;

enum AudioDataSourceID
{
    InternalSpeakers = 1769173099, // or else use 'ispk',
    Headphones       = 1751412846  // or else use 'hdpn'
};

AudioDeviceListener::AudioDeviceListener(AudioDeviceID deviceId, std::string name)
: m_id(deviceId)
, m_name(std::move(name))
{
    addDeviceListeners();
}

AudioDeviceListener::~AudioDeviceListener()
{
    removeDeviceListeners();
}

OSStatus AudioDeviceListener::onAudioObjectPropertyNotification(
    AudioObjectID /*inObjectID*/,
    UInt32 inNumberAddresses,
    const AudioObjectPropertyAddress inAddresses[],
    void* inClientData)
{
    auto This = static_cast<AudioDeviceListener*>(inClientData);
    if (This == nullptr)
        return noErr;

    const std::string name = This->m_name;
    auto log               = [](std::string msg) { std::cerr << msg << std::endl; };

    for (UInt32 i = 0; i < inNumberAddresses; ++i)
    {
        const AudioObjectPropertyAddress& address = inAddresses[i];
        switch (address.mSelector)
        {
            case kAudioDevicePropertyDeviceIsRunning:
            {
                UInt32 isRunning = 0;
                if (getProperty(This->m_id, &address, &isRunning) && (isRunning != 0u))
                    log(name + " audio device has stopped.");
                break;
            }
            case kAudioObjectPropertyControlList:
            {
                log(name + " Audio control properties changed");
                break;
            }
            case kAudioDevicePropertyDeviceHasChanged:
            {
                log(name + " audio device properties have changed.");
                break;
            }
            case kAudioDeviceProcessorOverload:
            {
                log(name + " audio device processor overload.");
                break;
            }
            case kAudioDevicePropertyDeviceIsAlive:
            {
                log(name + " audio device was removed.");
                break;
            }
            case kAudioDevicePropertyStreams:
            {
                log(name + " audio device stream properties have changed.");
                break;
            }
            case kAudioStreamPropertyPhysicalFormat:
            {
                log(name + " audio device physical format has changed.");
                break;
            }
            case kAudioDevicePropertyDataSource:
            {
                AudioDeviceID defaultDevice = 0;

                const AudioObjectPropertyAddress defaultAddr = {
                    kAudioHardwarePropertyDefaultOutputDevice,
                    kAudioObjectPropertyScopeGlobal,
                    kAudioObjectPropertyElementMain};

                getProperty(kAudioObjectSystemObject, &defaultAddr, &defaultDevice);

                AudioObjectPropertyAddress sourceAddr;
                sourceAddr.mSelector = kAudioDevicePropertyDataSource;
                sourceAddr.mScope    = kAudioDevicePropertyScopeOutput;
                sourceAddr.mElement  = kAudioObjectPropertyElementMain;

                UInt32 dataSourceId = 0;
                getProperty(defaultDevice, &sourceAddr, &dataSourceId);

                if (dataSourceId == AudioDataSourceID::InternalSpeakers)
                {
                    log(name + " audio output changed to internal speakers");
                }
                else if (dataSourceId == AudioDataSourceID::Headphones)
                {
                    log(name + " audio output changed to headphones");
                }
            }
        }
    }
    return noErr;
}

bool AudioDeviceListener::addListener(AudioObjectID objectId,
                                      AudioObjectPropertyAddress const* const address)
{
    return AudioObjectAddPropertyListener(
               objectId, address, onAudioObjectPropertyNotification, this) != noErr;
}
bool AudioDeviceListener::removeListener(AudioObjectID objectId,
                                         AudioObjectPropertyAddress const* const address)
{
    return AudioObjectRemovePropertyListener(
               objectId, address, onAudioObjectPropertyNotification, this) != noErr;
}

void AudioDeviceListener::addDeviceListeners()
{
    AudioObjectPropertyAddress address = {kAudioDevicePropertyStreams,
                                          kAudioDevicePropertyScopeInput,
                                          kAudioObjectPropertyElementMain};

    if (auto propertySize = getPropertySize(m_id, &address))
    {
        auto streams = static_cast<AudioStreamID*>(malloc(propertySize));

        if (getProperty(m_id, &address, streams))
        {
            UInt32 numStreams = propertySize / sizeof(AudioStreamID);

            for (UInt32 i = 0; i < numStreams; i++)
            {
                UInt32 isInput;
                address.mSelector = kAudioStreamPropertyDirection;
                address.mScope    = kAudioObjectPropertyScopeGlobal;

                if (getProperty(streams[i], &address, &isInput))
                {
                    address.mSelector = kAudioStreamPropertyPhysicalFormat;
                    addListener(streams[i], &address);
                }
            }
        }

        if (streams != nullptr)
            free(streams);
    }

    // add listener for checking when device running state changes
    address.mSelector = kAudioDevicePropertyDeviceIsRunning;
    if (getPropertySize(m_id, &address) != 0u)
    {
        addListener(m_id, &address);
    }

    // add listener for checking when device controls change:
    // Note that if a notification is received for this property, any cached AudioObjectIDs for the
    // device become invalid and need to be re-fetched.
    address.mSelector = kAudioObjectPropertyControlList;
    if (getPropertySize(m_id, &address) != 0u)
    {
        addListener(m_id, &address);
    }

    // listener for unforseable device changes
    address.mSelector = kAudioDevicePropertyDeviceHasChanged;
    address.mScope    = kAudioObjectPropertyScopeGlobal;
    address.mElement  = kAudioObjectPropertyElementMain;
    addListener(m_id, &address);

    // listener for processor overload
    address.mSelector = kAudioDeviceProcessorOverload;
    addListener(m_id, &address);

    // listener for when the device becomes alive or dies
    address.mSelector = kAudioDevicePropertyDeviceIsAlive;
    addListener(m_id, &address);

    // listener for headphones being plugged/un-plugged
    AudioDeviceID defaultOutputDevice = 0;

    address.mSelector = kAudioHardwarePropertyDefaultOutputDevice;
    getProperty(kAudioObjectSystemObject, &address, &defaultOutputDevice);

    address.mSelector = kAudioDevicePropertyDataSource;
    address.mScope    = kAudioDevicePropertyScopeOutput;
    address.mElement  = kAudioObjectPropertyElementMain;
    addListener(defaultOutputDevice, &address);
}

void AudioDeviceListener::removeDeviceListeners()
{
    AudioObjectPropertyAddress address = {kAudioDevicePropertyStreams,
                                          kAudioDevicePropertyScopeInput,
                                          kAudioObjectPropertyElementMain};

    if (auto propertySize = getPropertySize(m_id, &address))
    {

        auto streams = static_cast<AudioStreamID*>(malloc(propertySize));

        if (getProperty(m_id, &address, streams))
        {
            UInt32 numStreams = propertySize / sizeof(AudioStreamID);

            for (UInt32 i = 0; i < numStreams; i++)
            {
                UInt32 isInput;
                address.mSelector = kAudioStreamPropertyDirection;
                address.mScope    = kAudioObjectPropertyScopeGlobal;

                if (getProperty(streams[i], &address, &isInput))
                {
                    address.mSelector = kAudioStreamPropertyPhysicalFormat;
                    removeListener(streams[i], &address);
                }
            }
        }

        if (streams != nullptr)
            free(streams);
    }
    // add listener for checking when device running state changes
    address.mSelector = kAudioDevicePropertyDeviceIsRunning;
    if (getPropertySize(m_id, &address) != 0u)
    {
        removeListener(m_id, &address);
    }

    // add listener for checking when device controls change:
    // Note that if a notification is received for this property, any cached AudioObjectIDs for the
    // device become invalid and need to be re-fetched.
    address.mSelector = kAudioObjectPropertyControlList;
    if (getPropertySize(m_id, &address) != 0u)
    {
        removeListener(m_id, &address);
    }

    // listener for unforseable device changes
    address.mSelector = kAudioDevicePropertyDeviceHasChanged;
    address.mScope    = kAudioObjectPropertyScopeGlobal;
    address.mElement  = kAudioObjectPropertyElementMain;
    removeListener(m_id, &address);

    // listener for processor overload
    address.mSelector = kAudioDeviceProcessorOverload;
    removeListener(m_id, &address);

    // listener for when the device becomes alive or dies
    address.mSelector = kAudioDevicePropertyDeviceIsAlive;
    removeListener(m_id, &address);

    // listener for headphones being plugged/un-plugged
    AudioDeviceID defaultOutputDevice = 0;

    address.mSelector = kAudioHardwarePropertyDefaultOutputDevice;
    getProperty(kAudioObjectSystemObject, &address, &defaultOutputDevice);

    address.mSelector = kAudioDevicePropertyDataSource;
    address.mScope    = kAudioDevicePropertyScopeOutput;
    address.mElement  = kAudioObjectPropertyElementMain;
    removeListener(defaultOutputDevice, &address);
}
