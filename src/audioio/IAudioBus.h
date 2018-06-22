#ifndef DAP_AUDIO_IO_IAUDIOBUS_H
#define DAP_AUDIO_IO_IAUDIOBUS_H

#include <cstdint>

namespace dap
{
    namespace audioio
    {
        class IAudioBus;
        class IAudioProcess;
    }
}
class dap::audioio::IAudioBus
{
public:
    IAudioBus()                 = default;
    IAudioBus(const IAudioBus&) = delete;
    IAudioBus(IAudioBus&&)      = delete;
    virtual ~IAudioBus()        = default;
    IAudioBus& operator=(const IAudioBus&) = delete;
    IAudioBus& operator=(IAudioBus&&)            = delete;
    virtual bool start()                         = 0;
    virtual bool stop()                          = 0;
    virtual void setAudioProcess(IAudioProcess*) = 0;
    virtual void removeAudioProcess()            = 0;
};

#endif // DAP_AUDIO_IO_IAUDIOBUS_H
