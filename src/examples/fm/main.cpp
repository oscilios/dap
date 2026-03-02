#include "AudioProcess.h"
#include "audioio/AudioDeviceList.h"
#include <cassert>
#include <thread>

using namespace dap::audioio;

auto getOutputDeviceProperties()
{
    auto outputDevices = AudioDeviceList::create(dap::audioio::Scope::Output);
    assert(outputDevices.size() > 0u);
    IAudioDevice* outputDevice = outputDevices[0].get();
    assert(outputDevice != nullptr);

    return std::make_tuple(outputDevice->getId(),
                           outputDevice->getBufferSize(),
                           outputDevice->getSampleRate(),
                           outputDevice->getChannelCount());
}

int main()
{
    int32_t deviceId;
    size_t bufferSize;
    float sampleRate;
    size_t channelCount;
    std::tie(deviceId, bufferSize, sampleRate, channelCount) = getOutputDeviceProperties();

    std::cout << "deviceId: " << deviceId << " bufferSize: " << bufferSize
              << " sampleRate: " << sampleRate << " channelCount: " << channelCount << std::endl;

    fm::AudioProcess process(deviceId, channelCount, bufferSize, sampleRate);

    auto& synth = process.getSynth();

    synth.setGain(0.25f);

    //synth.setCarrierFreq(523.25f);
    //synth.setModFreq(523.25f);
    //synth.setModIndex(1.2f);

    synth.setCarrierFreq(349.23f);
    synth.setModFreq(523.25f);
    synth.setModIndex(1.5f);

    //synth.setCarrierFreq(440.0f);
    //synth.setModFreq(880.0f);
    //synth.setModIndex(0.7f);

    [[maybe_unused]] bool started = process.start();
    assert(started);

    std::cout << "press q + enter to exit....\n";
    while (std::cin.get() != 'q')
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    [[maybe_unused]] const bool stopped = process.stop();
    assert(stopped);

    return 0;
}

