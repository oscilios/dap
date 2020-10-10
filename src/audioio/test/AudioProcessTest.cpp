#include "PassThroughAudioProcess.h"
#include "audioio/AudioDeviceList.h"
#include <gtest/gtest.h>
#include <thread>

using namespace testing;
using namespace dap;
using namespace dap::audioio;

TEST(AudioProcessTest, test_start_stop)
{
    // input device
    auto inputDevices = AudioDeviceList::create(Scope::Input);
    ASSERT_GE(inputDevices.size(), 1u);
    IAudioDevice* inputDevice = inputDevices[0].get();
    ASSERT_TRUE(inputDevice != nullptr);
    auto bufferSize        = inputDevice->getBufferSize();
    auto sampleRate        = inputDevice->getSampleRate();
    auto inputChannelCount = inputDevice->getChannelCount();

    // output device
    auto outputDevices = AudioDeviceList::create(Scope::Output);
    ASSERT_GE(outputDevices.size(), 1u);
    IAudioDevice* outputDevice = outputDevices[0].get();
    ASSERT_TRUE(outputDevice != nullptr);

    outputDevice->setBufferSize(bufferSize);
    ASSERT_EQ(bufferSize, outputDevice->getBufferSize());
    if (!outputDevice->setSampleRate(sampleRate)) // sometimes fails
        outputDevice->setSampleRate(sampleRate);
    ASSERT_EQ(sampleRate, outputDevice->getSampleRate());
    auto outputChannelCount = outputDevice->getChannelCount();

    std::cout << "bufferSize: " << bufferSize << " sampleRate: " << sampleRate
              << " input channels: " << inputChannelCount
              << " output channels: " << outputChannelCount << std::endl;
    try
    {
        PassThroughAudioProcess process(inputDevice->getId(),
                                        outputDevice->getId(),
                                        inputChannelCount,
                                        outputChannelCount,
                                        bufferSize,
                                        sampleRate);
        const bool started = process.start();
        const auto start   = std::chrono::system_clock::now();
        const auto dur     = std::chrono::milliseconds(10000);
        auto elapsed       = std::chrono::system_clock::now() - start;
        while (elapsed < dur)
        {
            elapsed = std::chrono::system_clock::now() - start;
            std::this_thread::yield();
        }
        const bool stopped = process.stop();
        ASSERT_TRUE(started);
        ASSERT_TRUE(stopped);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        ASSERT_TRUE(false);
    }
}
