#include "Semaphore.h"

#ifdef __APPLE__
#include <dispatch/dispatch.h>
#endif

using dap::Semaphore;

#ifdef __APPLE__
class Semaphore::Impl
{
    dispatch_semaphore_t m_sem;

public:
    explicit Impl(int initialValue)
    : m_sem(dispatch_semaphore_create(initialValue))
    {
    }
    Impl(const Impl&) =delete;
    Impl(Impl&&) =delete;
    ~Impl()
    {
        dispatch_release(m_sem);
    }
    Impl& operator=(const Impl&) =delete;
    Impl& operator=(Impl&&) =delete;
    int64_t wait()
    {
        return dispatch_semaphore_wait(m_sem, DISPATCH_TIME_FOREVER);
    }
    int64_t wait(int64_t ns)
    {
        return dispatch_semaphore_wait(m_sem, dispatch_time(DISPATCH_TIME_NOW, ns));
    }
    void signal(uint32_t count)
    {
        while ((count--) != 0)
            dispatch_semaphore_signal(m_sem);
    }
};
#else
#error "Semaphore not defined for other platforms than OSX"
#endif

Semaphore::Semaphore(int initialValue)
: m_sem(std::make_unique<Semaphore::Impl>(initialValue))
{
}
Semaphore::~Semaphore() = default;
int64_t Semaphore::wait()
{
    return m_sem->wait();
}
int64_t Semaphore::wait(int64_t ns)
{
    return m_sem->wait(ns);
}
void Semaphore::signal(uint32_t count)
{
    m_sem->signal(count);
}
