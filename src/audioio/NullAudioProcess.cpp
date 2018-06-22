#include "IAudioProcess.h"

namespace dap
{
    namespace audioio
    {
        NullAudioProcess& NullAudioProcess::instance() noexcept
        {
            static NullAudioProcess p;
            return p;
        }
    }
}
