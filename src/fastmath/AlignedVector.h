#ifndef DAP_FASTMATH_VECTOR_H
#define DAP_FASTMATH_VECTOR_H

#include "base/Streamable.h"
#include "fastmath/FastmathAlignedAllocator.h"
#include <vector>
#include <ostream>

namespace dap
{
    namespace fastmath
    {
        template <typename T>
        using AlignedVector = std::vector<T, dap::fastmath::AlignedAllocator<T> >;

        template <typename T>
        void copy(AlignedVector<T>& dst, T const* const src, size_t size)
        {
            size = std::min(dst.size(), size);
            std::memcpy(dst.data(), src, sizeof(T) * size);
        }
        template <typename T>
        void copy(AlignedVector<T>& dst, const AlignedVector<T>& src)
        {
            copy(dst, src.data(), src.size());
        }
        template <typename T>
        void reset(AlignedVector<T>& dst)
        {
            std::memset(dst.data(), 0, dst.size() * sizeof(T));
        }

        template <typename T>
        inline std::ostream& operator<<(std::ostream& out, const AlignedVector<T>& v)
        {
            return streamContainer(out, v);
        }
    }
}

#endif // DAP_FASTMATH_VECTOR_H
