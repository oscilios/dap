#ifndef DAP_THREADSAFE_ATOMIC_LOCK_H
#define DAP_THREADSAFE_ATOMIC_LOCK_H

namespace dap
{
    namespace threadsafe
    {
        class AtomicLock;
    }
}
class dap::threadsafe::AtomicLock
{
    std::atomic<bool>& m_mtx;

public:
    explicit AtomicLock(std::atomic<bool>& mtx)
    : m_mtx(mtx)
    {
        while (m_mtx.exchange(true))
        {
        }
    }
    AtomicLock(const AtomicLock&) = delete;
    AtomicLock(AtomicLock&&) = delete;
    ~AtomicLock()
    {
        m_mtx = false;
    }
    AtomicLock& operator=(const AtomicLock&) = delete;
    AtomicLock& operator=(AtomicLock&&) = delete;
};
#endif // DAP_THREADSAFE_ATOMIC_LOCK_H
