#include "AudioDevice.h"
#include "AudioUtilities.h"
#include <CoreServices/CoreServices.h>
#include <cmath>
#include <ostream>

using dap::coreaudio::AudioDevice;

size_t static nextPowerOfTwo(size_t n)
{
    n--;
    n |= (n >> 1);
    n |= (n >> 2);
    n |= (n >> 4);
    n |= (n >> 8);
    n |= (n >> 16);
    return ++n;
}

AudioDevice::AudioDevice(AudioDeviceID devid, AudioObjectPropertyScope scope)
{
    init(devid, scope);
}
bool AudioDevice::setBufferSize(UInt32 size)
{
    UInt32 bufferSize = size;
    if (bufferSize == m_bufferSize)
        return true;

    AudioObjectPropertyAddress address = {kAudioDevicePropertyBufferFrameSize, m_scope, 0};
    setProperty(m_ID, &address, &bufferSize);
    getProperty(m_ID, &address, &m_bufferSize);
    return m_bufferSize == bufferSize;
}

bool AudioDevice::setSampleRate(float sampleRate)
{
    Float64 sr                         = sampleRate;
    AudioObjectPropertyAddress address = {kAudioDevicePropertyNominalSampleRate, m_scope, 0};
    setProperty(m_ID, &address, &sr);
    getProperty(m_ID, &address, &m_sampleRate);
    return std::fabs(m_sampleRate - sr) < 1e-6;
}

bool AudioDevice::setExclusiveAccess()
{
    // Will set device exclusivity to process with PID
    // Use with care, setting exclusivity can prevent other applications from using this device
    auto pid                           = getpid();
    AudioObjectPropertyAddress address = {
        kAudioDevicePropertyHogMode, kAudioObjectPropertyScopeGlobal, 0};
    return setProperty(m_ID, &address, &pid);
}

void AudioDevice::init(AudioDeviceID id, AudioObjectPropertyScope scope)
{
    m_ID           = id;
    m_scope        = scope;
    m_safetyOffset = 0;
    m_bufferSize   = 0;
    m_bufferSizeList.clear();
    m_channelCount = 0;
    m_name.clear();
    m_manufacturer.clear();
    m_sampleRate = 0;
    m_sampleRateList.clear();
    m_supportsFloat = false;
    m_supportsInt   = false;

    if (m_ID == kAudioDeviceUnknown)
        return;

    // device safety offset
    // indicates the number of frames in ahead (for output) or behind (for input)
    // the current hardware position that is safe to do IO.
    AudioObjectPropertyAddress address = {kAudioDevicePropertySafetyOffset, m_scope, 0};
    getProperty(m_ID, &address, &m_safetyOffset);

    // device buffersize
    address.mSelector = kAudioDevicePropertyBufferFrameSize;
    getProperty(m_ID, &address, &m_bufferSize);

    // device buffersize range
    address.mSelector = kAudioDevicePropertyBufferFrameSizeRange;
    AudioValueRange bufferSizeRange;
    getProperty(m_ID, &address, &bufferSizeRange);
    if (bufferSizeRange.mMinimum < bufferSizeRange.mMaximum)
    {
        size_t nextPow2 = nextPowerOfTwo(static_cast<size_t>(bufferSizeRange.mMinimum));
        while (nextPow2 <= bufferSizeRange.mMaximum)
        {
            m_bufferSizeList.push_back(nextPow2);
            nextPow2 = nextPowerOfTwo(nextPow2 + 1);
        }
    }
    else
    {
        m_bufferSizeList.push_back(m_bufferSize);
    }

    // device actual sample rate
    address.mSelector = kAudioDevicePropertyActualSampleRate;
    getProperty(m_ID, &address, &m_sampleRate);

    // device supported sample rates
    address.mSelector = kAudioDevicePropertyAvailableNominalSampleRates;
    if (auto propertySize = getPropertySize(m_ID, &address))
    {
        address.mSelector               = kAudioDevicePropertyAvailableNominalSampleRates;
        const auto sampleRateRangeCount = propertySize / sizeof(AudioValueRange);
        std::vector<AudioValueRange> outRanges(sampleRateRangeCount);
        getProperty(m_ID, &address, outRanges.data());
        for (const auto sr : outRanges)
        {
            m_sampleRateList.emplace_back(sr.mMinimum);
            assert(sr.mMinimum == sr.mMaximum);
        }
    }

    // device format
    address.mSelector = kAudioDevicePropertyStreamFormat;
    getProperty(m_ID, &address, &m_format);

    if ((m_format.mFormatFlags & kAudioFormatFlagIsFloat) != 0u)
        m_supportsFloat = true;
    if ((m_format.mFormatFlags & kAudioFormatFlagIsSignedInteger) != 0u)
        m_supportsInt = true;

    // compute channel count
    address.mSelector = kAudioDevicePropertyStreamConfiguration;
    if (auto propertySize = getPropertySize(m_ID, &address))
    {
        auto buflist = static_cast<AudioBufferList*>(malloc(propertySize));
        getProperty(m_ID, &address, buflist);
        for (UInt32 i = 0; i < buflist->mNumberBuffers; ++i)
            m_channelCount += buflist->mBuffers[i].mNumberChannels;
        free(buflist);
    }

    // device name
    address.mSelector = kAudioDevicePropertyDeviceName;
    char deviceName[256];
    getProperty(m_ID, &address, &deviceName);
    m_name = std::string(deviceName);

    // device manufacturer
    address.mSelector = kAudioDevicePropertyDeviceManufacturerCFString;
    char deviceManufacturer[256];
    getProperty(m_ID, &address, &deviceManufacturer);
    m_manufacturer = std::string(deviceManufacturer);

    // obtain all available formats
    address = {kAudioStreamPropertyAvailablePhysicalFormats,
               kAudioObjectPropertyScopeGlobal,
               kAudioObjectPropertyElementMaster};

    if (auto propertySize = getPropertySize(m_ID, &address))
    {
        auto numFormats = propertySize / sizeof(AudioStreamRangedDescription);
        std::vector<AudioStreamRangedDescription> outData(numFormats);
        getProperty(m_ID, &address, outData.data());

        for (size_t i = 0; i < numFormats; ++i)
        {
            auto flag = outData[i].mFormat.mFormatFlags;
            if ((flag & kAudioFormatFlagIsFloat) != 0u)
                m_supportsFloat = true;
            if ((flag & kAudioFormatFlagIsSignedInteger) != 0u)
                m_supportsInt = true;
        }
    }

    // obtain device latency
    address = {kAudioDevicePropertyLatency, m_scope, kAudioObjectPropertyElementMaster};
    if (!getProperty(m_ID, &address, &m_latency))
    {
        m_latency = 0;
    }
}
