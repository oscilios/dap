#include "PassThroughAudioProcess.h"
#include "audioio/AudioDeviceList.h"
#include "dap_gtest.h"
#include <thread>

using namespace testing;
using namespace dap;
using namespace dap::audioio;

static bool startsWith(const std::string& line, const std::string& preffix)
{
    return line.find_first_of(preffix, 0) == 0;
}

static const char* defaultDevicePrefix()
{
#ifdef __APPLE__
    return "Built-in";
#else
    return "Unknown default device prefix.";
#endif
}

class AudioProcessTest : public Test
{
};

DAP_TEST_F(AudioProcessTest, test_start_stop)
{
    // input device
    auto inputDevices = AudioDeviceList::create(Scope::Input);
    DAP_ASSERT_GE(inputDevices.size(), 1u);
    IAudioDevice* inputDevice = nullptr;
    for (const auto& dev : inputDevices)
    {
        if (startsWith(dev->getName(), defaultDevicePrefix()))
        {
            inputDevice = dev.get();
        }
    }
    DAP_ASSERT_TRUE(inputDevice != nullptr);
    auto bufferSize        = inputDevice->getBufferSize();
    auto sampleRate        = inputDevice->getSampleRate();
    auto inputChannelCount = inputDevice->getChannelCount();

    // output device
    auto outputDevices = AudioDeviceList::create(Scope::Output);
    DAP_ASSERT_GE(outputDevices.size(), 1u);
    IAudioDevice* outputDevice = nullptr;
    for (const auto& dev : outputDevices)
    {
        if (startsWith(dev->getName(), defaultDevicePrefix()))
        {
            outputDevice = dev.get();
        }
    }
    DAP_ASSERT_TRUE(outputDevice != nullptr);

    outputDevice->setBufferSize(bufferSize);
    DAP_ASSERT_EQ(bufferSize, outputDevice->getBufferSize());
    if (!outputDevice->setSampleRate(sampleRate)) // sometimes fails
        outputDevice->setSampleRate(sampleRate);
    DAP_ASSERT_EQ(sampleRate, outputDevice->getSampleRate());
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
        DAP_ASSERT_TRUE(started);
        DAP_ASSERT_TRUE(stopped);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        DAP_ASSERT_TRUE(false);
    }
}
