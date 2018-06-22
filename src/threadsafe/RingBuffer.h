#ifndef DAP_THREADSAFE_RINGBUFFER_H
#define DAP_THREADSAFE_RINGBUFFER_H

#include <atomic>
#include <cassert>
#include <cstring>

#include "base/SystemAnnotations.h"
#include "base/TypeTraits.h"
#include "AtomicLock.h"

namespace dap
{
    namespace threadsafe
    {
        template <typename T, size_t TMaxSize>
        class RingBuffer;
    }
}

// RingBuffer that allows multiple readers and multiple writers by using atomics as a
// from of low locking mechanism. Note that only exclusiveWrite and exclusiveRead are
// protected from being accessed by multiple threads at the same time.

template <typename T, size_t TMaxSize>
class dap::threadsafe::RingBuffer final
{
    std::atomic<size_t> m_readIdx{0};
    std::atomic<bool> m_readMutex{false};
    alignas(64) char m_padToAvoidFalseSharing1[64 - sizeof(size_t) - sizeof(bool)];
    std::atomic<size_t> m_writeIdx{0};
    std::atomic<bool> m_writeMutex{false};
    alignas(64) char m_padToAvoidFalseSharing2[64 - sizeof(size_t) - sizeof(bool)];

#if (__GNUC__ <= 4 && __GNUC_MINOR__ < 8) // alignas only supported from gcc4.8
    __attribute__((aligned(16))) T m_buffer[TMaxSize];
#else
    alignas(16) T m_buffer[TMaxSize];
#endif
    size_t m_latency{0};
    static constexpr size_t m_bufferSize{TMaxSize};
    static constexpr size_t m_bitmask{TMaxSize - 1};

    bool indicesAreSane(size_t rIdx, size_t wIdx) const noexcept
    {
        return rIdx <= wIdx;
    }

public:
    RingBuffer()
    {
        static_assert(IsPowerOfTwo<TMaxSize>::value, "RingBuffer max size must be a power of two");
        static_assert(std::is_trivially_copyable<T>::value);
        std::memset(m_buffer, 0, m_bufferSize * sizeof(T)); // NOLINT
    }
    RingBuffer(const RingBuffer&) = delete;
    RingBuffer(RingBuffer&&) noexcept = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;
    RingBuffer& operator=(RingBuffer&&) noexcept = delete;
    ~RingBuffer() = default;

    void setLatency(size_t latency_)
    {
        assert (m_readIdx.load(std::memory_order_relaxed) == 0);
        assert (m_writeIdx.load(std::memory_order_relaxed) == 0);
        m_latency = std::min(latency_, m_bufferSize / 2);
    }
    size_t getBufferSize() const noexcept
    {
        return m_bufferSize;
    }
    size_t getLatency() const noexcept
    {
        return m_latency;
    }
    size_t getTotalReadSize() const noexcept
    {
        return m_readIdx.load(std::memory_order_relaxed);
    }
    size_t getTotalWrittenSize() const noexcept
    {
        return m_writeIdx.load(std::memory_order_relaxed);
    }

    // read size from buffer into output
    size_t DAP_ATTRIBUTE_NON_NULL read(T* const output, size_t size) noexcept
    {
        assert(indicesAreSane(m_readIdx.load(std::memory_order_relaxed),
                              m_writeIdx.load(std::memory_order_relaxed)) &&
               "m_readIdx > m_writeIdx"); // NOLINT

        const size_t rIdx = m_readIdx.load(std::memory_order_relaxed);
        const size_t wIdx = m_writeIdx.load(std::memory_order_acquire);

        size                     = std::min(size, wIdx - rIdx);
        const size_t rIdxWrapped = rIdx & m_bitmask;
        const size_t dist        = std::min(m_bufferSize - rIdxWrapped, size);

        std::memcpy(output, m_buffer + rIdxWrapped, dist * sizeof(T)); // NOLINT
        std::memcpy(output + dist, m_buffer, (size - dist) * sizeof(T)); // NOLINT

        m_readIdx.fetch_add(size, std::memory_order_release);

        assert(indicesAreSane(m_readIdx.load(std::memory_order_relaxed),
                              m_writeIdx.load(std::memory_order_relaxed)) &&
               "m_readIdx > m_writeIdx"); // NOLINT

        return size;
    }

    // write size from input into buffer
    size_t DAP_ATTRIBUTE_NON_NULL write(T const* const input, size_t size) noexcept
    {
        assert(indicesAreSane(m_readIdx.load(std::memory_order_relaxed),
                              m_writeIdx.load(std::memory_order_relaxed)) &&
               "m_readIdx > m_writeIdx"); // NOLINT

        const size_t wIdx        = m_writeIdx.load(std::memory_order_relaxed);
        const size_t rIdx        = m_writeIdx.load(std::memory_order_acquire);
        const size_t maxReadSize = std::min(size, wIdx - rIdx);

        size                     = std::min(size, m_bufferSize - maxReadSize);
        const size_t wIdxWrapped = wIdx & m_bitmask;
        const size_t dist        = std::min(m_bufferSize - wIdxWrapped, size);

        std::memcpy(m_buffer + wIdxWrapped, input, dist * sizeof(T)); // NOLINT
        std::memcpy(m_buffer, input + dist, (size - dist) * sizeof(T)); // NOLINT

        m_writeIdx.fetch_add(size, std::memory_order_release);

        assert(indicesAreSane(m_readIdx.load(std::memory_order_relaxed),
                              m_writeIdx.load(std::memory_order_relaxed)) &&
               "m_readIdx > m_writeIdx"); // NOLINT

        return size;
    }
    void clear()
    {
        m_readIdx.store(0, std::memory_order_release);
        m_writeIdx.store(m_latency, std::memory_order_relaxed);
    }
    // multi consumer read
    size_t exclusiveRead(T* output, size_t size) noexcept
    {
        AtomicLock lk(m_readMutex);
        size = read(output, size);
        return size;
    }
    // multi producer write
    size_t exclusiveWrite(T const* input, size_t size) noexcept
    {
        AtomicLock lk(m_writeMutex);
        size = write(input, size);
        return size;
    }
    void exclusiveClear()
    {
        AtomicLock rlk(m_readMutex);
        AtomicLock wlk(m_writeMutex);
        clear();
    }
};

#endif // DAP_THREADSAFE_RINGBUFFER_H
