#include "AudioProcess.h"
#include "OscEventSystem.h"
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

    crtp_synth::AudioProcess process(deviceId, channelCount, bufferSize, sampleRate);
    crtp_synth::OscEventSystem eventSystem(process);

    bool started = process.start();
    assert(started);
    started = eventSystem.start();
    assert(started);

    std::cout << "press q + enter to exit....\n";
    while (std::cin.get() != 'q')
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    eventSystem.stop();
    const bool stopped = process.stop();
    assert(stopped);

    return 0;
}
