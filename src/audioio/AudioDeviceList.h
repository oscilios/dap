#ifndef DAP_AUDIO_IO_AUDIO_DEVICE_LIST_H
#define DAP_AUDIO_IO_AUDIO_DEVICE_LIST_H

#include "Scope.h"
#include "IAudioDevice.h"
#include <vector>
#include <memory>

namespace dap
{
  namespace audioio
  {
    class AudioDeviceList;
  }
}
class dap::audioio::AudioDeviceList
{
public:
  static std::vector<std::unique_ptr<IAudioDevice>> create(Scope scope);
};
#endif // DAP_AUDIO_IO_AUDIO_DEVICE_LIST_H
