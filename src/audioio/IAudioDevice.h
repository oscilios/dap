#ifndef DAP_AUDIO_IO_IAUDIODEVICE_H
#define DAP_AUDIO_IO_IAUDIODEVICE_H

#include "Scope.h"
#include <string>
#include <vector>

namespace dap
{
    namespace audioio
    {
        class IAudioDevice;
    }
}

class dap::audioio::IAudioDevice
{
public:
    IAudioDevice()                    = default;
    IAudioDevice(const IAudioDevice&) = default;
    IAudioDevice(IAudioDevice&&)      = default;
    virtual ~IAudioDevice()           = default;
    IAudioDevice& operator=(const IAudioDevice&) = default;
    IAudioDevice& operator=(IAudioDevice&&)                       = default;
    virtual bool setBufferSize(uint32_t size)                     = 0;
    virtual bool setSampleRate(float sampleRate)                  = 0;
    virtual bool setExclusiveAccess()                             = 0;
    virtual int32_t getId() const                                 = 0;
    virtual bool isValid() const                                  = 0;
    virtual uint32_t getBufferSize() const                        = 0;
    virtual uint32_t getChannelCount() const                      = 0;
    virtual float getSampleRate() const                           = 0;
    virtual std::vector<uint32_t> getSupportedBufferSizes() const = 0;
    virtual std::vector<float> getSupportedSampleRates() const    = 0;
    virtual const std::string& getName() const                    = 0;
    virtual const std::string& getManufacturer() const            = 0;
    virtual bool getFormatFloatSupported() const                  = 0;
    virtual bool getFormatIntSupported() const                    = 0;
    virtual uint32_t getSafetyOffset() const                      = 0;
    virtual uint32_t getLatency() const                           = 0;
};

#endif // DAP_AUDIO_IO_IAUDIODEVICE_H
