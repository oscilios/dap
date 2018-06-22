#ifndef DAP_BASE_SEMAPHORE_H
#define DAP_BASE_SEMAPHORE_H

#include <memory>

namespace dap
{
    class Semaphore;
}
class dap::Semaphore
{
    class Impl;
    std::unique_ptr<Impl> m_sem;

public:
    explicit Semaphore(int initialValue = 0);
    Semaphore(const Semaphore&) = delete;
    Semaphore(Semaphore&&)      = delete;
    ~Semaphore();
    Semaphore& operator=(const Semaphore&) = delete;
    Semaphore& operator=(Semaphore&&) = delete;
    int64_t wait();
    int64_t wait(int64_t ns);
    void signal(uint32_t count = 1);
};
#endif // DAP_BASE_SEMAPHORE_H
