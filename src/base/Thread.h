#ifndef DAP_BASE_THREAD_RAII_H
#define DAP_BASE_THREAD_RAII_H

#include <thread>

namespace dap
{
    class Thread;
}

class dap::Thread
{
public:
    using RaiiFunction = void (std::thread::*)();

private:
    std::thread m_thread;
    RaiiFunction m_function;

public:
    Thread(std::thread&& t, RaiiFunction func)
    : m_thread(std::move(t))
    , m_function(func)
    {
    }
    Thread(const Thread&) = delete;
    Thread(Thread&&) = delete;
    ~Thread()
    {
        if (m_thread.joinable())
            (m_thread.*m_function)();
    }
    Thread& operator=(const Thread&) = delete;
    Thread& operator=(Thread&&) = delete;

    std::thread& get()
    {
        return m_thread;
    }
};

#endif // DAP_BASE_THREAD_RAII_H
