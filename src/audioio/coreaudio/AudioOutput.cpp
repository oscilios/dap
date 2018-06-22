#include "AudioOutput.h"
#include "AudioDeviceListener.h"
#include "AudioUtilities.h"
#include "audioio/IAudioProcess.h"
#include "threadsafe/AtomicLock.h"
#include <iostream>
#include <thread>

#include <CoreServices/CoreServices.h>

using dap::audioio::NullAudioProcess;
using dap::coreaudio::AudioOutput;

AudioOutput::AudioOutput(AudioDeviceID output, uint32_t bufferSize, float sampleRate)
: m_device(output, kAudioDevicePropertyScopeOutput)
, m_audioProcess(&NullAudioProcess::instance())
{
    if (!m_device.setBufferSize(bufferSize))
        std::cout << "AudioOutput: could not set bufferSize" << std::endl;

    if (!m_device.setSampleRate(sampleRate))
    {
        if (!m_device.setSampleRate(sampleRate))
            std::cout << "AudioOutput: could not set sample rate" << std::endl;
    }

    if (init() != noErr)
    {
        throw std::runtime_error("ERROR: Cannot initialize AudioOutput");
    }
    m_listener = std::make_unique<AudioDeviceListener>(m_device.getId(), m_device.getName());
}
AudioOutput::~AudioOutput()
{
    cleanup();
}
OSStatus AudioOutput::setupGraph()
{
    OSStatus err = NewAUGraph(&m_auGraph);
    DAP_ASSERT_NOERR(err);

    err = AUGraphOpen(m_auGraph);
    DAP_ASSERT_NOERR(err);

    err = makeGraph();
    DAP_ASSERT_NOERR(err);

    err = setDeviceAsCurrent();
    DAP_ASSERT_NOERR(err);

    // Tell the output unit not to reset timestamps
    // Otherwise sample rate changes will cause sync los
    UInt32 startAtZero = 0;
    err                = AudioUnitSetProperty(m_auUnit,
                               kAudioOutputUnitProperty_StartTimestampsAtZero,
                               kAudioUnitScope_Global,
                               0,
                               &startAtZero,
                               sizeof(startAtZero));
    DAP_ASSERT_NOERR(err);

    AURenderCallbackStruct callbackStruct;
    callbackStruct.inputProc       = renderCallback;
    callbackStruct.inputProcRefCon = this;

    err = AudioUnitSetProperty(m_auUnit,
                               kAudioUnitProperty_SetRenderCallback,
                               kAudioUnitScope_Global,
                               0, // global bus
                               &callbackStruct,
                               sizeof(callbackStruct));

    DAP_ASSERT_NOERR(err);

    return err;
}
OSStatus AudioOutput::makeGraph()
{
    AudioComponentDescription desc;
    desc.componentType         = kAudioUnitType_Output;
    desc.componentSubType      = kAudioUnitSubType_DefaultOutput;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags        = 0;
    desc.componentFlagsMask    = 0;

    // create the nodes
    // This creates a node in the graph that is an AudioUnit, using
    // the supplied ComponentDescription to find and open that unit
    OSStatus err = AUGraphAddNode(m_auGraph, &desc, &m_auNode);
    DAP_ASSERT_NOERR(err);

    // Get Audio Units from AUGraph node
    err = AUGraphNodeInfo(m_auGraph, m_auNode, nullptr, &m_auUnit);
    DAP_ASSERT_NOERR(err);

    return err;
}
OSStatus AudioOutput::setDeviceAsCurrent()
{
    auto deviceId = m_device.getId();
    OSStatus err  = AudioUnitSetProperty(m_auUnit,
                                        kAudioOutputUnitProperty_CurrentDevice,
                                        kAudioUnitScope_Global,
                                        0,
                                        &deviceId,
                                        sizeof(deviceId));

    DAP_ASSERT_NOERR(err);
    return err;
}
OSStatus AudioOutput::setupBuffers()
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

    // Get the Stream Format (Output client side)
    propertySize = sizeof(asbd_dev);
    err          = AudioUnitGetProperty(m_auUnit,
                               kAudioUnitProperty_StreamFormat,
                               kAudioUnitScope_Output,
                               0,
                               &asbd_dev,
                               &propertySize);
    DAP_ASSERT_NOERR(err);

    propertySize = sizeof(asbd);
    err          = AudioUnitGetProperty(
        m_auUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &asbd, &propertySize);
    DAP_ASSERT_NOERR(err);

    // Set the format of all the AUs to the output device channel count
    asbd.mChannelsPerFrame = asbd_dev.mChannelsPerFrame;

    // We must get the sample rate of the input device and set it to the stream format of AUHAL
    propertySize                       = sizeof(Float64);
    AudioObjectPropertyAddress address = {kAudioDevicePropertyNominalSampleRate,
                                          kAudioObjectPropertyScopeGlobal,
                                          kAudioObjectPropertyElementMaster};

    getProperty(m_device.getId(), &address, &rate);

    asbd.mSampleRate = rate;
    propertySize     = sizeof(asbd);

    // Set the new audio stream formats for the rest of the AUs...
    DAP_ASSERT_NOERR(err);
    err = AudioUnitSetProperty(
        m_auUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &asbd, propertySize);
    DAP_ASSERT_NOERR(err);

    m_outputs.resize(asbd.mChannelsPerFrame, nullptr);

    // std::cout << "ASBD output device: " << asbd_dev << std::endl;
    // std::cout << "output ASBD: " << asbd << std::endl;

    return err;
}
OSStatus AudioOutput::renderCallback(void* inRefCon,
                                     AudioUnitRenderActionFlags* /*ioActionFlags*/,
                                     const AudioTimeStamp* /*inTimeStamp*/,
                                     UInt32 /*inBusNumber*/,
                                     UInt32 inNumberFrames,
                                     AudioBufferList* ioData)
{
    auto This = static_cast<AudioOutput*>(inRefCon);
    assert(This);
    if (This == nullptr)
        return -1;

    dap::threadsafe::AtomicLock lk(This->m_mtx);
    if (This->m_stopRequested)
        return noErr;
    OSStatus err                = noErr;
    const size_t outputChannels = ioData->mNumberBuffers;
    assert(This->m_outputs.size() == outputChannels);
    for (size_t ch = 0; ch < outputChannels; ++ch)
    {
        This->m_outputs[ch] = static_cast<float*>(ioData->mBuffers[ch].mData);
    }
    This->m_audioProcess->process();
    This->m_samplesProcessed += inNumberFrames;
    return err;
}
OSStatus AudioOutput::init()
{
    OSStatus err = setupGraph();
    DAP_ASSERT_NOERR(err);
    err = setupBuffers();
    DAP_ASSERT_NOERR(err);
    err = AUGraphInitialize(m_auGraph);
    DAP_ASSERT_NOERR(err);

    return err;
}
void AudioOutput::cleanup()
{
    stop();
    m_listener.reset();
    AUGraphClose(m_auGraph);
    DisposeAUGraph(m_auGraph);
}
bool AudioOutput::isRunning() const
{
    if (m_auGraph == nullptr)
        return false;

    Boolean graphRunning = FALSE;
    OSStatus err         = AUGraphIsRunning(m_auGraph, &graphRunning);
    DAP_ASSERT_NOERR(err);
    return graphRunning != FALSE;
}
bool AudioOutput::start()
{
    dap::threadsafe::AtomicLock lk(m_mtx);
    OSStatus err = noErr;
    if (!isRunning())
    {
        m_stopRequested    = false;
        m_samplesProcessed = 0;
        err                = AUGraphStart(m_auGraph);
        DAP_ASSERT_NOERR(err);
    }
    return err == noErr;
}
bool AudioOutput::stop()
{
    {
        dap::threadsafe::AtomicLock lk(m_mtx);
        m_stopRequested = true;
    }
    OSStatus err = AUGraphStop(m_auGraph);
    DAP_ASSERT_NOERR(err);
    return err == noErr;
}
void AudioOutput::setAudioProcess(audioio::IAudioProcess* p)
{
    dap::threadsafe::AtomicLock lk(m_mtx);
    if (isRunning())
    {
        std::cerr << "AudioOutput::setAudioProcess, cannot set audio process while running."
                  << std::endl;
        return;
    }
    m_audioProcess = p;
    m_audioProcess->setOutputs(m_outputs.data());
}
void AudioOutput::removeAudioProcess()
{
    dap::threadsafe::AtomicLock lk(m_mtx);
    if (isRunning())
    {
        std::cerr << "AudioOutput::removeAudioProcess, cannot remove audio process while running."
                  << std::endl;
        return;
    }
    m_audioProcess = &NullAudioProcess::instance();
}
