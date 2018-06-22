#ifndef DAP_EXAMPLES_CRTP_SYNTH_OSC_EVENT_SYSTEM_H
#define DAP_EXAMPLES_CRTP_SYNTH_OSC_EVENT_SYSTEM_H

#include "osc/OscReceiver.h"

namespace crtp_synth
{
    class AudioProcess;
    class OscEventSystem;
}

class crtp_synth::OscEventSystem
{
    dap::OscReceiver m_oscReceiver;

public:
    OscEventSystem(AudioProcess& p);
    bool start()
    {
        return m_oscReceiver.run();
    }
    void stop()
    {
        return m_oscReceiver.stop();
    }
};

#endif // DAP_EXAMPLES_CRTP_SYNTH_OSC_EVENT_SYSTEM_H
