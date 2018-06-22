#include "AudioInput.h"
#include "AudioDeviceListener.h"
#include "AudioUtilities.h"
#include "audioio/IAudioProcess.h"
#include "threadsafe/AtomicLock.h"
#include <iostream>
#include <thread>

#include <CoreServices/CoreServices.h>

using dap::audioio::NullAudioProcess;
using dap::coreaudio::AudioInput;

AudioInput::AudioInput(AudioDeviceID input, uint32_t bufferSize, float sampleRate)
: m_device(input, kAudioDevicePropertyScopeInput)
, m_audioProcess(&NullAudioProcess::instance())
{
    if (!m_device.setBufferSize(bufferSize))
        std::cout << "AudioInput: could not set bufferSize" << std::endl;

    if (!m_device.setSampleRate(sampleRate))
    {
        if (!m_device.setSampleRate(sampleRate))
            std::cout << "AudioInput: could not set sample rate" << std::endl;
    }

    if (init() != noErr)
    {
        throw std::runtime_error("ERROR: Cannot initialize AudioInput");
    }
    m_listener = std::make_unique<AudioDeviceListener>(m_device.getId(), m_device.getName());
}
AudioInput::~AudioInput()
{
    cleanup();
}

OSStatus AudioInput::setupAUHAL()
{
    OSStatus err = noErr;

    AudioComponent comp;
    AudioComponentDescription desc;

    desc.componentType         = kAudioUnitType_Output;
    desc.componentSubType      = kAudioUnitSubType_HALOutput;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags        = 0;
    desc.componentFlagsMask    = 0;

    // Finds a component that meets the desc spec's
    comp = AudioComponentFindNext(nullptr, &desc);
    if (comp == nullptr)
        exit(-1);

    // gains access to the services provided by the component
    err = AudioComponentInstanceNew(comp, &m_auUnit);
    DAP_ASSERT_NOERR(err);

    // AUHAL needs to be initialized before anything is done to it
    err = AudioUnitInitialize(m_auUnit);
    DAP_ASSERT_NOERR(err);

    err = enableIO();
    DAP_ASSERT_NOERR(err);

    err = setDeviceAsCurrent();
    DAP_ASSERT_NOERR(err);

    err = setupCallback();
    DAP_ASSERT_NOERR(err);

    // Don't setup buffers until you know what the
    // input and output device audio streams look like.

    err = AudioUnitInitialize(m_auUnit);

    return err;
}
OSStatus AudioInput::setDeviceAsCurrent()
{
    auto deviceId = m_device.getId();
    OSStatus err = err = AudioUnitSetProperty(m_auUnit,
                                              kAudioOutputUnitProperty_CurrentDevice,
                                              kAudioUnitScope_Global,
                                              0,
                                              &deviceId,
                                              sizeof(deviceId));
    DAP_ASSERT_NOERR(err);
    return err;
}
OSStatus AudioInput::enableIO()
{
    UInt32 enableIO;

    // You must enable the Audio Unit (AUHAL) for input and disable output
    // BEFORE setting the AUHAL's current device.

    // Enable input on the AUHAL
    enableIO     = 1;
    OSStatus err = AudioUnitSetProperty(m_auUnit,
                                        kAudioOutputUnitProperty_EnableIO,
                                        kAudioUnitScope_Input,
                                        1, // input element
                                        &enableIO,
                                        sizeof(enableIO));
    DAP_ASSERT_NOERR(err);

    // disable Output on the AUHAL
    enableIO = 0;
    err      = AudioUnitSetProperty(m_auUnit,
                               kAudioOutputUnitProperty_EnableIO,
                               kAudioUnitScope_Output,
                               0, // output element
                               &enableIO,
                               sizeof(enableIO));
    DAP_ASSERT_NOERR(err);
    return err;
}
OSStatus AudioInput::setupCallback()
{
    AURenderCallbackStruct callbackStruct;
    callbackStruct.inputProc       = renderCallback;
    callbackStruct.inputProcRefCon = this;

    // Setup the input callback.
    OSStatus err = AudioUnitSetProperty(m_auUnit,
                                        kAudioOutputUnitProperty_SetInputCallback,
                                        kAudioUnitScope_Global,
                                        0,
                                        &callbackStruct,
                                        sizeof(callbackStruct));
    DAP_ASSERT_NOERR(err);
    return err;
}
OSStatus AudioInput::setupBuffers()
{
    OSStatus err = noErr;
    UInt32 bufferSizeFrames;

    AudioStreamBasicDescription asbd, asbd_dev;
    Float64 rate = 0;

    // Get the size of the IO buffer(s)
    UInt32 propertySize = sizeof(bufferSizeFrames);
    err                 = AudioUnitGetProperty(m_auUnit,
                               kAudioDevicePropertyBufferFrameSize,
                               kAudioUnitScope_Global,
                               0,
                               &bufferSizeFrames,
                               &propertySize);
    DAP_ASSERT_NOERR(err);

    UInt32 bufferSizeBytes = bufferSizeFrames * sizeof(Float32);

    // Get the Stream Format (Input client side)
    propertySize = sizeof(asbd_dev);
    err          = AudioUnitGetProperty(m_auUnit,
                               kAudioUnitProperty_StreamFormat,
                               kAudioUnitScope_Input,
                               1,
                               &asbd_dev,
                               &propertySize);
    DAP_ASSERT_NOERR(err);

    // Get the Stream Format (client side)
    propertySize = sizeof(asbd);
    err          = AudioUnitGetProperty(
        m_auUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &asbd, &propertySize);
    DAP_ASSERT_NOERR(err);

    // Set the format of all the AUs to the input devices channel count
    asbd.mChannelsPerFrame = asbd_dev.mChannelsPerFrame;

    // We must get the sample rate of the input device and set it to the stream format of AUHAL
    propertySize                       = sizeof(Float64);
    AudioObjectPropertyAddress address = {kAudioDevicePropertyNominalSampleRate,
                                          kAudioObjectPropertyScopeGlobal,
                                          kAudioObjectPropertyElementMaster};
    getProperty(m_device.getId(), &address, &rate);

    asbd.mSampleRate = rate;
    propertySize     = sizeof(asbd);

    // Set the new formats to the AUs...
    err = AudioUnitSetProperty(
        m_auUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &asbd, propertySize);
    DAP_ASSERT_NOERR(err);

    // malloc buffer lists
    // calculate number of buffers from channels
    const UInt32 bytes = sizeof(AudioBufferList) * asbd.mChannelsPerFrame;

    m_buffer                 = static_cast<AudioBufferList*>(malloc(bytes));
    m_buffer->mNumberBuffers = asbd.mChannelsPerFrame;

    // pre-malloc buffers for AudioBufferLists
    m_inputs.clear();
    m_inputs.resize(m_buffer->mNumberBuffers);
    for (UInt32 ch = 0; ch < m_buffer->mNumberBuffers; ch++)
    {
        m_buffer->mBuffers[ch].mNumberChannels = 1;
        m_buffer->mBuffers[ch].mDataByteSize   = bufferSizeBytes;
        m_buffer->mBuffers[ch].mData           = malloc(bufferSizeBytes);
        m_inputs[ch]                           = static_cast<float*>(m_buffer->mBuffers[ch].mData);
    }
    MakeBufferSilent(m_buffer);

    // std::cout << "ASBD input device: " << asbd_dev << std::endl;
    // std::cout << "input ASBD: " << asbd << std::endl;

    return err;
}
OSStatus AudioInput::renderCallback(void* inRefCon,
                                    AudioUnitRenderActionFlags* ioActionFlags,
                                    const AudioTimeStamp* inTimeStamp,
                                    UInt32 inBusNumber,
                                    UInt32 inNumberFrames,
                                    AudioBufferList* /*ioData*/)
{
    auto This = static_cast<AudioInput*>(inRefCon);
    assert(This);
    if ((This == nullptr) || This->m_stopRequested)
        return -1;

    dap::threadsafe::AtomicLock lk(This->m_mtx);

    OSStatus err = AudioUnitRender(
        This->m_auUnit, ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, This->m_buffer);
    This->m_samplesProcessed += inNumberFrames;
    This->m_audioProcess->setInputs(This->m_inputs.data());
    DAP_ASSERT_NOERR(err);
    return err;
}
OSStatus AudioInput::init()
{
    OSStatus err = setupAUHAL();
    DAP_ASSERT_NOERR(err);

    err = setupBuffers();
    DAP_ASSERT_NOERR(err);

    return err;
}
void AudioInput::cleanup()
{
    stop();
    m_listener.reset();

    for (UInt32 i = 0; i < m_buffer->mNumberBuffers; i++)
        free(m_buffer->mBuffers[i].mData);
    free(m_buffer);

    AudioUnitUninitialize(m_auUnit);
    AudioComponentInstanceDispose(m_auUnit);
}
bool AudioInput::isRunning() const
{
    OSStatus err        = noErr;
    UInt32 auhalRunning = 0;
    UInt32 size         = 0;
    size                = sizeof(auhalRunning);

    if (m_auUnit != nullptr)
    {
        err = AudioUnitGetProperty(m_auUnit,
                                   kAudioOutputUnitProperty_IsRunning,
                                   kAudioUnitScope_Global,
                                   0, // input element
                                   &auhalRunning,
                                   &size);
        DAP_ASSERT_NOERR(err);
    }

    return auhalRunning != 0u;
}
bool AudioInput::start()
{
    dap::threadsafe::AtomicLock lk(m_mtx);

    OSStatus err = noErr;
    if (!isRunning())
    {
        m_stopRequested    = false;
        m_samplesProcessed = 0;
        err                = AudioOutputUnitStart(m_auUnit);
        DAP_ASSERT_NOERR(err);
    }
    return err == noErr;
}
bool AudioInput::stop()
{
    dap::threadsafe::AtomicLock lk(m_mtx);
    m_stopRequested = true;
    OSStatus err    = AudioOutputUnitStop(m_auUnit);
    DAP_ASSERT_NOERR(err);
    return err == noErr;
}
void AudioInput::setAudioProcess(audioio::IAudioProcess* p)
{
    dap::threadsafe::AtomicLock lk(m_mtx);
    if (isRunning())
    {
        std::cerr << "AudioInput::setAudioProcess, cannot set audio process while running."
                  << std::endl;
        return;
    }
    m_audioProcess = p;
}
void AudioInput::removeAudioProcess()
{
    dap::threadsafe::AtomicLock lk(m_mtx);
    if (isRunning())
    {
        std::cerr << "AudioInput::removeAudioProcess, cannot remove audio process while running."
                  << std::endl;
        return;
    }
    m_audioProcess = &NullAudioProcess::instance();
}
