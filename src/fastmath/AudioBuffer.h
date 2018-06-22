#ifndef DAP_FASTMATH_AUDIO_BUFFER_H
#define DAP_FASTMATH_AUDIO_BUFFER_H

#include "fastmath/Array.h"
#include "fastmath/AlignedVector.h"
#include <vector>

namespace dap
{
    namespace fastmath
    {
        template <typename T>
        class AudioBuffer;
    }
}

template <typename T>
class dap::fastmath::AudioBuffer
{

public:
    using value_type = T;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using reference = value_type &;
    using const_reference = const value_type &;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template <typename U>
    using Allocator = fastmath::AlignedAllocator<U>;

    using array_type = Array<T, Allocator<T> >;
    using vector_type = AlignedVector<array_type>;
    using ptr_vector_type = AlignedVector<pointer>;

    using iterator = typename vector_type::iterator;

    using Channel = array_type;

private:
    size_type m_channelCount{0};
    size_type m_channelSize{0};
    vector_type m_buffer;
    ptr_vector_type m_ptrs;

    void updatePtrs()
    {
        for (size_type i = 0; i < m_ptrs.size(); ++i)
        {
            m_ptrs[i] = m_buffer[i].data();
        }
    }

    void forbidAllocation(bool b)
    {
        for (auto& channel : m_buffer)
        {
            channel.forbidAllocation(b);
        }
    }
    void
    allocate(size_type channelCount, size_type channelSize, const_reference value = value_type(0))
    {
        if ((m_channelCount == channelCount) && (m_channelSize == channelSize))
        {
            return;
        }
        m_channelCount = channelCount;
        m_channelSize = channelSize;
        forbidAllocation(false);
        m_buffer.resize(m_channelCount, Channel(m_channelSize, value));
        m_ptrs.resize(m_channelCount, nullptr);
        updatePtrs();
        forbidAllocation(true);
    }

public:
    explicit AudioBuffer() = default;
    AudioBuffer(size_type channelCount,
                size_type channelSize,
                const_reference value = value_type(0))
    {
        allocate(channelCount, channelSize, value);
    }
    AudioBuffer(const AudioBuffer& other)
    {
        allocate(other.channelCount(), other.channelSize());
        for (size_type i = 0; i < m_channelCount; ++i)
        {
            m_buffer[i] = other.channel(i);
        }
    }
    AudioBuffer(AudioBuffer&& other) noexcept
    {
        allocate(other.channelCount(), 0);
        for (size_type i = 0; i < m_channelCount; ++i)
        {
            m_buffer[i] = std::move(other.channel(i));
        }
    }
    AudioBuffer(const std::initializer_list<Channel>& list)
    {
        size_type size = 0;
        for (const auto& channel : list)
        {
            if (size != 0 && size != channel.size())
            {
                throw std::runtime_error("AudioBuffer size mismatch.");
            }
            else
            {
                size = channel.size();
            }
        }
        allocate(list.size(), size);
        size_type i = 0;
        for (const auto& channel : list)
        {
            m_buffer[i++] = channel;
        }
    }
    AudioBuffer(const std::initializer_list<value_type>& list)
    {
        allocate(list.size(), size_type(1));
        m_buffer[0] = list;
    }
    AudioBuffer& operator=(const AudioBuffer& other)
    {
        allocate(other.channelCount(), other.channelSize());
        for (size_type i = 0; i < m_channelCount; ++i)
        {
            m_buffer[i] = other.channel(i);
        }
        return *this;
    }
    AudioBuffer& operator=(AudioBuffer&& other) noexcept
    {
        allocate(other.channelCount(), 0);
        for (size_type i = 0; i < m_channelCount; ++i)
        {
            m_buffer[i] = std::move(other.channel(i));
        }
        return *this;
    }
    ~AudioBuffer() = default;
    Channel& channel(size_type ch)
    {
        return m_buffer[ch];
    }
    const Channel& channel(size_type ch) const
    {
        return m_buffer[ch];
    }
    size_type channelCount() const
    {
        return m_channelCount;
    }
    size_type channelSize() const
    {
        return m_channelSize;
    }
    ptr_vector_type& channelData()
    {
        return m_ptrs;
    }
    const ptr_vector_type& channelData() const
    {
        return m_ptrs;
    }
    pointer* data()
    {
        return m_ptrs.data();
    }
    pointer const* data() const
    {
        return m_ptrs.data();
    }
    void
    resize(size_type channelCount, size_type channelSize, const_reference value = value_type(0))
    {
        allocate(channelCount, channelSize, value);
    }
    iterator begin()
    {
        return m_buffer.begin();
    }
    const iterator begin() const
    {
        return m_buffer.begin();
    }
    iterator end()
    {
        return m_buffer.end();
    }
    const iterator end() const
    {
        return m_buffer.end();
    }
    void clear()
    {
        for (auto& channel : m_buffer)
        {
            channel.clear();
        }
    }
    void fill(const_reference value)
    {
        for (auto& channel : m_buffer)
        {
            channel.fill(value);
        }
    }
};

namespace dap
{
    namespace fastmath
    {
        template <typename T>
        inline std::ostream& operator<<(std::ostream& out, const AudioBuffer<T>& buffer)
        {
            const auto channelCount = buffer.channelCount();
            const auto bufferSize = buffer.channelSize();
            out << "(" << channelCount << "x" << bufferSize << ") [ ";
            for (size_t ch = 0; ch < channelCount; ++ch)
            {
                const auto& channel = buffer.channel(ch);
                out << "[";
                for (size_t samp = 0; samp < bufferSize; ++samp)
                {
                    out << channel[samp] << " ";
                }
                out << "]";
            }
            out << "]";
            return out;
        }
    }
}


#endif // DAP_FASTMATH_AUDIO_BUFFER_H
