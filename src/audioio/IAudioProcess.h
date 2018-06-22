#ifndef DAP_AUDIO_IO_AUDIO_PROCESS_H
#define DAP_AUDIO_IO_AUDIO_PROCESS_H

namespace dap
{
    namespace audioio
    {
        class IAudioProcess
        {
        public:
            IAudioProcess()                     = default;
            IAudioProcess(const IAudioProcess&) = default;
            IAudioProcess(IAudioProcess&&)      = default;
            IAudioProcess& operator=(const IAudioProcess&) = default;
            IAudioProcess& operator=(IAudioProcess&&)   = default;
            virtual ~IAudioProcess()                    = default;
            virtual bool process()                      = 0;
            virtual void setInputs(float const* const*) = 0;
            virtual void setOutputs(float**)            = 0;
            virtual bool start()                        = 0;
            virtual bool stop()                         = 0;
        };

        class NullAudioProcess : public IAudioProcess
        {
            NullAudioProcess() noexcept = default;

        public:
            static NullAudioProcess& instance() noexcept;
            bool process() noexcept override
            {
                return true;
            }
            void setInputs(float const* const* /*inputs*/) noexcept override
            {
            }
            void setOutputs(float** /*outputs*/) noexcept override
            {
            }
            bool start() noexcept override
            {
                return true;
            }
            bool stop() noexcept override
            {
                return true;
            }
        };
    }
}
#endif // DAP_AUDIO_IO_COREAUDIO_AUDIO_PROCESS_H
