#ifndef DAP_FASTMATH_ALIGNED_ALLOCATOR_H
#define DAP_FASTMATH_ALIGNED_ALLOCATOR_H

#include "base/TypeTraits.h"
#include <cstdlib>
#include <limits>
#include <cassert>
#include <stdexcept>
#include <memory>

namespace dap
{
    namespace fastmath
    {
        template <typename T, unsigned int Alignas>
        class AlignedAllocator;
    }
}

template <typename T, unsigned int Alignas = 16>
class dap::fastmath::AlignedAllocator
{
    static_assert(IsPowerOfTwo<Alignas>::value, "Alignment size needs to be a power of two");

public:
    using value_type      = T;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using AlignasMask     = std::integral_constant<int, Alignas - 1u>;

    template <typename U>
    struct rebind
    {
        using other = AlignedAllocator<U>;
    };

    inline pointer allocate(size_type cnt,
                            typename std::allocator<void>::const_pointer /*unused*/ = nullptr)
    {
        size_t bytes = cnt * sizeof(T);
        void* data   = nullptr;
#ifdef _WIN32
        data = _aligned_malloc(datasize * sizeof(T), Alignas);
#else
        if (posix_memalign(&data, Alignas, bytes) != 0)
        {
            throw std::bad_alloc();
        }
#endif
        AlignasMask mask;
        assert((size_t(data) & mask) == 0);
        if ((data == nullptr) || ((size_t(data) & mask) != 0))
        {
            throw std::bad_alloc();
        }
        return reinterpret_cast<pointer>(data); // NOLINT
    }
    inline void deallocate(pointer p, size_type /*unused*/)
    {
#ifdef _WIN32
        _aligned_free(p);
#else
        std::free(p); // NOLINT
#endif
    }

    inline size_type max_size() const
    {
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }

    inline void construct(pointer p, const T& t = T())
    {
        new (p) T(t);
    }
    inline void destroy(pointer p)
    {
        p->~T();
    }

    inline bool operator==(AlignedAllocator const& /*unused*/)
    {
        return true;
    }
    inline bool operator!=(AlignedAllocator const& a)
    {
        return !operator==(a);
    }
};
#endif // DAP_FASTMATH_ALIGNED_ALLOCATOR_H
