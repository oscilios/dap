#include "audioio/AudioBusFactory.h"
#include "audioio/AudioDeviceList.h"
#include <gtest/gtest.h>

using namespace testing;
using namespace dap;
using namespace dap::audioio;

TEST(AudioInputTest, test_start_stop)
{
    auto inputDevices = AudioDeviceList::create(Scope::Input);
    ASSERT_GE(inputDevices.size(), 1u);
    IAudioDevice* inputDevice = inputDevices[0].get();
    ASSERT_TRUE(inputDevice != nullptr);
    auto bufferSize = inputDevice->getBufferSize();
    auto sampleRate = inputDevice->getSampleRate();
    std::cout << "bufferSize: " << bufferSize << " sampleRate: " << sampleRate << std::endl;
    try
    {
        auto input =
            AudioBusFactory::create(Scope::Input, inputDevice->getId(), bufferSize, sampleRate);
        const bool started = input->start();
        const bool stopped = input->stop();
        ASSERT_TRUE(started);
        ASSERT_TRUE(stopped);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        ASSERT_TRUE(false);
    }
}
