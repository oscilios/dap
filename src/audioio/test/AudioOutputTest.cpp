#include "dap_gtest.h"
#include "audioio/AudioDeviceList.h"
#include "audioio/AudioBusFactory.h"

using namespace testing;
using namespace dap;
using namespace dap::audioio;

class AudioOutputTest : public Test
{
};

DAP_TEST_F(AudioOutputTest, test_start_stop)
{
  auto outputDevices = AudioDeviceList::create(Scope::Output);
  DAP_ASSERT_GE(outputDevices.size(), 1u);
  IAudioDevice* outputDevice = outputDevices[0].get();
  DAP_ASSERT_TRUE(outputDevice != nullptr);
  auto bufferSize = outputDevice->getBufferSize();
  auto sampleRate = outputDevice->getSampleRate();
  std::cout << "bufferSize: " << bufferSize << " sampleRate: " << sampleRate << std::endl;
  try
  {
    auto output = AudioBusFactory::create(Scope::Output, outputDevice->getId(), bufferSize, sampleRate);
    const bool started = output->start();
    const bool stopped = output->stop();
    DAP_ASSERT_TRUE(started);
    DAP_ASSERT_TRUE(stopped);
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    DAP_ASSERT_TRUE(false);
  }
}
