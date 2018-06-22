#ifndef DAP_THREAD_JOINER_H
#define DAP_THREAD_JOINER_H

#include <thread>
#include <vector>

// Given a vector of threads, joins them all at destruction

namespace dap
{
    class ThreadJoiner;
}

class dap::ThreadJoiner
{
    std::vector<std::thread>& m_threads;

public:
    explicit ThreadJoiner(std::vector<std::thread>& threads)
    : m_threads(threads)
    {
    }
    ThreadJoiner(const ThreadJoiner&) = delete;
    ThreadJoiner(ThreadJoiner&&)      = delete;
    ~ThreadJoiner()
    {
        for (auto& t : m_threads)
        {
            if (t.joinable())
                t.join();
        }
    }
    ThreadJoiner& operator=(const ThreadJoiner&) = delete;
    ThreadJoiner& operator=(ThreadJoiner&&) = delete;
};

#endif // DAP_THREAD_JOINER_H
