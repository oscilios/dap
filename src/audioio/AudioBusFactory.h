#ifndef DAP_AUDIO_IO_AUDIO_BUS_FACTORY_H
#define DAP_AUDIO_IO_AUDIO_BUS_FACTORY_H

#include "Scope.h"
#include "IAudioBus.h"
#include <memory>

namespace dap
{
    namespace audioio
    {
        struct AudioBusFactory
        {
            static std::unique_ptr<IAudioBus>
            create(Scope scope, int32_t deviceId, size_t bufferSize, float sampleRate);
        };
    }
}
#endif // DAP_AUDIO_IO_AUDIO_BUS_FACTORY_H
