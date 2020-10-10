#include <gtest/gtest.h>
#include "audioio/AudioDeviceList.h"
#include "audioio/AudioBusFactory.h"

using namespace testing;
using namespace dap;
using namespace dap::audioio;

TEST(AudioOutputTest, test_start_stop)
{
  auto outputDevices = AudioDeviceList::create(Scope::Output);
  ASSERT_GE(outputDevices.size(), 1u);
  IAudioDevice* outputDevice = outputDevices[0].get();
  ASSERT_TRUE(outputDevice != nullptr);
  auto bufferSize = outputDevice->getBufferSize();
  auto sampleRate = outputDevice->getSampleRate();
  std::cout << "bufferSize: " << bufferSize << " sampleRate: " << sampleRate << std::endl;
  try
  {
    auto output = AudioBusFactory::create(Scope::Output, outputDevice->getId(), bufferSize, sampleRate);
    const bool started = output->start();
    const bool stopped = output->stop();
    ASSERT_TRUE(started);
    ASSERT_TRUE(stopped);
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    ASSERT_TRUE(false);
  }
}
