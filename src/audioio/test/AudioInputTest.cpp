#include "audioio/AudioBusFactory.h"
#include "audioio/AudioDeviceList.h"
#include "dap_gtest.h"

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

class AudioInputTest : public Test
{
};

DAP_TEST_F(AudioInputTest, test_start_stop)
{
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
    auto bufferSize = inputDevice->getBufferSize();
    auto sampleRate = inputDevice->getSampleRate();
    std::cout << "bufferSize: " << bufferSize << " sampleRate: " << sampleRate << std::endl;
    try
    {
        auto input =
            AudioBusFactory::create(Scope::Input, inputDevice->getId(), bufferSize, sampleRate);
        const bool started = input->start();
        const bool stopped = input->stop();
        DAP_ASSERT_TRUE(started);
        DAP_ASSERT_TRUE(stopped);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        DAP_ASSERT_TRUE(false);
    }
}
