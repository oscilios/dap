#ifndef DAP_FASTMATH_ARRAY_H
#define DAP_FASTMATH_ARRAY_H

#include "base/Streamable.h"
#include "base/TypeTraits.h"
#include "base/SystemAnnotations.h"
#include "FastmathAlignedAllocator.h"
#include "ArrayOps.h"
#include <iterator>
#include <cstring>
#include <type_traits>
#include <vector>

namespace dap
{
    namespace fastmath
    {
        template <typename T, typename Allocator>
        class Array;
    }
}

template <typename T, typename Allocator = dap::fastmath::AlignedAllocator<T> >
class dap::fastmath::Array
{
    static_assert(isArithmeticOrComplex<T>(),
                  "dap::fastmath::Array does not support non arithmetic types");

public:

    struct AllocationForbiddenException : std::exception
    {
        const char* what() const noexcept override
        {
            return "Array cannot allocate: allocation was forbidden.";
        }
    };

    using value_type      = T;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using array_type      = Array<value_type, Allocator>;

    template <typename Ttag, typename Tpointer, typename Treference>
    class Iterator : public std::iterator<Ttag, value_type, difference_type, Tpointer, Treference>
    {
        pointer m_ptr;

    public:
        explicit Iterator(pointer ptr_ = nullptr)
        : m_ptr(ptr_)
        {
        }
        Iterator& operator++()
        {
            m_ptr++;
            return *this;
        }
        Iterator operator++(int)
        {
            Iterator result = *this;
            ++(*this);
            return result;
        }
        bool operator==(Iterator other) const
        {
            return m_ptr == other.m_ptr;
        }
        bool operator!=(Iterator other) const
        {
            return !(*this == other);
        }
        reference operator*() const
        {
            return *m_ptr;
        }
    };

    // iterators
    using iterator               = Iterator<std::random_access_iterator_tag, pointer, reference>;
    using const_iterator         = Iterator<std::random_access_iterator_tag, const_pointer, const_reference>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator       = std::reverse_iterator<iterator>;

protected:
    // assertions
    void AssertSize() const
    {
        assert(m_size == static_cast<size_type>(m_end_ptr - m_start_ptr));
    }

    // memory management
    void allocate(size_type size)
    {
        if (m_size >= size)
        {
            return;
        }

        if (m_allocationForbidden)
        {
            throw AllocationForbiddenException();
        }

        m_start_ptr  = m_allocator.allocate(size);
        m_size       = size;
        m_ownsMemory = true;
        m_end_ptr    = m_start_ptr + m_size;
    }
    void deallocate()
    {
        if (!m_ownsMemory)
        {
            return;
        }
        if (!m_start_ptr)
        {
            return;
        }

        AssertSize();
        m_allocator.deallocate(m_start_ptr, m_end_ptr - m_start_ptr);
        m_size      = 0;
        m_start_ptr = nullptr;
        m_end_ptr   = nullptr;
    }

    // exceptions
    void throwIfSizeMisMatch(const Array& a)
    {
        if (a.size() != m_size)
        {
            throw std::runtime_error("Array size mismatch.");
        }
    }

    // initialization used in lazy operators
    // template <typename LazyOp>
    // void lazyInit(const LazyOp& op) // this template is too generic
    //{
    //    const size_type size = op.size();
    //    if (m_size != size) allocate(size);
    //    this->map() = op;
    //}

    template <typename LazyOp, typename Rhs>
    void lazyInit(const CwiseUnaryOp<LazyOp, Rhs>& op)
    {
        const size_type size = op.size();
        if (m_size != size)
        {
            allocate(size);
        }
        this->map() = op;
    }
    template <typename LazyOp, typename Lhs, typename Rhs>
    void lazyInit(const CwiseBinaryOp<LazyOp, Lhs, Rhs>& op)
    {
        const size_type size = op.size();
        if (m_size != size)
        {
            allocate(size);
        }
        this->map() = op;
    }
    template <typename Lhs, typename Rhs, int NestingFlags>
    void lazyInit(const CoeffBasedProduct<Lhs, Rhs, NestingFlags>& op)
    {
        const size_type size = op.size();
        if (m_size != size)
        {
            allocate(size);
        }
        this->map() = op;
    }

    Allocator m_allocator;
    pointer m_start_ptr{nullptr};
    pointer m_end_ptr{nullptr};
    size_type m_size{0};
    bool m_ownsMemory{false};
    bool m_allocationForbidden{false};

public:
    explicit Array() = default;
    explicit Array(size_type size, const_reference value = value_type(0))
    {
        if (size == 0u)
        {
            return;
        }
        allocate(size);
        fill(value);
    }
    Array(pointer ptr, size_type start, size_type end, bool ownMemory = true)
    {
        checkAlignment(ptr);

        if (end < start)
        {
            throw std::runtime_error("Array::Array start > end.");
        }

        const size_type size = end - start;
        if (ownMemory)
        {
            allocate(size);
            copy(ptr, start, end);
            return;
        }
        m_start_ptr = ptr + start;
        m_end_ptr   = m_start_ptr + size;
        m_size      = size;
    }
    Array(const_pointer ptr, size_type start, size_type end)
    {
        checkAlignment(ptr);

        if (end < start)
        {
            throw std::runtime_error("Array::Array start > end.");
        }

        const size_type size = end - start;
        allocate(size);
        copy(ptr, start, size);
    }
    Array(const std::initializer_list<value_type>& list)
    {
        const size_type size = list.size();
        allocate(size);
        copy(list.begin(), 0, size);
    }
    Array(pointer ptr, size_type size, bool ownMemory = true)
    : Array(ptr, 0, size, ownMemory)
    {
    }
    Array(const_pointer ptr, size_type size)
    : Array(ptr, 0, size)
    {
    }
    Array(Array& other, bool ownMemory = true)
    : Array(other.data(), 0, other.size(), ownMemory)
    {
    }
    Array(const Array& other)
    : Array(other.data(), 0, other.size())
    {
    }
    Array(Array& other, size_type start, size_type end, bool ownMemory = true)
    : Array(other.data(), start, end, ownMemory)
    {
    }
    Array(const Array& other, size_type start, size_type end)
    : Array(other.data(), start, end)
    {
    }
    Array(Array&& other) noexcept
    : m_start_ptr(std::move(other.m_start_ptr))
    , m_end_ptr(std::move(other.m_end_ptr))
    , m_size(std::move(other.m_size))
    , m_ownsMemory(std::move(other.m_ownsMemory))
    , m_allocationForbidden(std::move(other.m_allocationForbidden))
    {
        other.m_start_ptr  = nullptr;
        other.m_end_ptr    = nullptr;
        other.m_size       = 0;
        other.m_ownsMemory = false;
    }

    template <typename VectorAlloc>
    explicit Array(const std::vector<value_type, VectorAlloc>& v, bool ownMemory = true)
    : Array(const_cast<pointer>(v.data()), 0, v.size(), ownMemory) // NOLINT
    {
        if (!ownMemory)
        {
            checkAlignment(v.data());
        }
    }
    virtual ~Array()
    {
        deallocate();
    }
    Array& operator=(const Array& other)
    {
        allocate(other.size());
        copy(other.data(), other.size());
        return *this; // NOLINT
    }
    Array& operator=(Array&& other) noexcept
    {
        m_start_ptr = std::move(other.m_start_ptr);
        m_end_ptr = std::move(other.m_end_ptr);
        m_size = std::move(other.m_size);
        m_ownsMemory = std::move(other.m_ownsMemory);
        m_allocationForbidden = std::move(other.m_allocationForbidden);
        other.m_start_ptr  = nullptr;
        other.m_end_ptr    = nullptr;
        other.m_size       = 0;
        other.m_ownsMemory = false;
        return *this; // NOLINT
    }
    Array& operator=(const std::initializer_list<value_type>& list)
    {
        const size_type size = list.size();
        allocate(size);
        copy(list.begin(), 0, size);
        return *this; // NOLINT
    }
    pointer data()
    {
        return m_start_ptr;
    }
    const_pointer data() const
    {
        return m_start_ptr;
    }
    iterator begin()
    {
        return iterator(m_start_ptr);
    }
    const_iterator begin() const
    {
        return const_iterator(m_start_ptr);
    }
    iterator end()
    {
        return iterator(m_end_ptr);
    }
    const_iterator end() const
    {
        return const_iterator(m_end_ptr);
    }
    reference operator[](size_type n)
    {
        return *(m_start_ptr + n);
    }
    const_reference operator[](size_type n) const
    {
        return *(m_start_ptr + n);
    }
    size_type size() const
    {
        AssertSize();
        return m_size;
    }
    void resize(size_type size, const_reference value = 0)
    {
        allocate(size);
        fill(value);
    }
    void push_back(const_reference value = 0)
    {
        array_type tmp(m_size + 1u);
        tmp.copy(*this);
        std::swap(tmp, *this);
        back() = value;
    }
    reference back(size_type n = 0)
    {
        return *(m_end_ptr - n - 1);
    }
    const_reference back(size_type n = 0) const
    {
        return *(m_end_ptr - n - 1);
    }
    bool ownsMemory() const
    {
        return m_ownsMemory;
    }
    bool isAllocationForbidden() const
    {
        return m_allocationForbidden;
    }
    void forbidAllocation(bool b)
    {
        m_allocationForbidden = b;
    }
    void copy(const_pointer ptr, size_type size)
    {
        assert(size <= m_size);
        std::memcpy(m_start_ptr, ptr, std::min(size, m_size) * sizeof(value_type));
    }
    void copy(const_pointer ptr, size_type start, size_type end)
    {

        if (end < start)
        {
            throw std::runtime_error("Array::copy start > end.");
        }
        size_type size = end - start;
        assert(size <= m_size);
        std::memcpy(m_start_ptr, ptr, std::min(size, m_size) * sizeof(value_type));
    }
    void copy(const Array& other)
    {
        copy(other.data(), other.size());
    }

    // operations
    void fill(const_reference value)
    {
        fill(value, 0, m_size);
    }
    void fill(const_reference value, size_type from, size_type to)
    {
        this->map().segment(from, to).fill(value);
    }
    void reset()
    {
        clear();
    }
    void clear()
    {
        std::memset(m_start_ptr, 0, m_size * sizeof(value_type));
    }
    value_type rms() const
    {
        if (std::is_floating_point<value_type>::value)
        {
            return std::sqrt(this->vec().squaredNorm() / this->size());
        }
        return std::sqrt(float(this->vec().squaredNorm()) / float(this->size()));
    }
    value_type accumulate() const
    {
        return this->map().sum();
    }

    Array& linspace(const_reference low, const_reference high)
    {
        fastmath::linspace(low, high, m_start_ptr, m_size);
        return *this;
    }
    Array& normalize()
    {
        fastmath::normalize(m_start_ptr, m_size);
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // lazy operators interface
    ////////////////////////////////////////////////////////////////////////////////

     //*******************
     //* As a map
     //*******************

    const array::ConstMapType<value_type> map() const
    {
        return array::const_map(m_start_ptr, m_size);
    }
    array::MapType<value_type> map()
    {
        return array::map(m_start_ptr, m_size);
    }
    explicit Array(array::ConstMapType<value_type> mappedArray)
    {
        const size_t size = mappedArray.size();
        allocate(size);
        copy(mappedArray.data(), size);
    }

    template <typename BinaryOp>
    DAP_NO_EXPLICIT_CTOR Array(const BinaryOp& op)
    {
        lazyInit(op);
    }

    template <typename BinaryOp>
    Array& operator=(const BinaryOp& op)
    {
        lazyInit(op);
        return *this; // NOLINT
    }

     //*******************
     //* As a vector
     //*******************

    explicit Array(const vector::VectorType<value_type>& vec)
    {
        lazyInit(vec);
    }
    Array& operator=(const vector::VectorType<value_type>& vec)
    {
        lazyInit(vec);
        return *this; // NOLINT
    }

    const vector::ConstMapType<value_type> vec() const
    {
        return vector::const_map<value_type>(m_start_ptr, m_size);
    }
    vector::MapType<value_type> vec()
    {
        return vector::map(m_start_ptr, m_size);
    }

    //*************************
    //* As a fixed size matrix
    //*************************

    template <size_t Rows, size_t Cols, int Order = matrix::ColMajor>
    explicit Array(const matrix::MatrixType<value_type, Rows, Cols, Order>& mat)
    {
        lazyInit(mat);
    }
    template <size_t Rows, size_t Cols, int Order = matrix::ColMajor>
    Array& operator=(const matrix::MatrixType<value_type, Rows, Cols, Order>& mat)
    {
        lazyInit(mat);
        return *this; // NOLINT
    }

    template <size_t Rows, size_t Cols, int Order = matrix::ColMajor>
    matrix::ConstMapType<value_type, Rows, Cols, Order> mat() const
    {
        return matrix::const_map<value_type, Cols, Rows, Order>(m_start_ptr, m_size);
    }
    template <size_t Rows, size_t Cols, int Order = matrix::ColMajor>
    matrix::MapType<value_type, Rows, Cols, Order> mat()
    {
        return matrix::map<value_type, Cols, Rows, Order>(m_start_ptr, m_size);
    }

    //*****************************
    //* As a dynamic size matrix
    //*****************************

    template <int Order = matrix::ColMajor>
    explicit Array(const matrix::MatrixTypeDynamic<value_type, Order>& mat)
    {
        lazyInit(mat);
    }
    template <int Order       = matrix::ColMajor>
    Array& operator=(const matrix::MatrixTypeDynamic<value_type, Order>& mat)
    {
        lazyInit(mat);
        return *this; // NOLINT
    }

    // non realtime safe since it may allocate memory
    template <int Order = matrix::ColMajor>
    matrix::ConstMapTypeDynamic<value_type, Order> mat(size_t rows, size_t cols) const
    {
        assert(rows * cols == m_size);
        return matrix::const_map<value_type, Order>(m_start_ptr, rows, cols);
    }
    template <int Order = matrix::ColMajor>
    matrix::MapTypeDynamic<value_type, Order> mat(size_t rows, size_t cols)
    {
        assert(rows * cols == m_size);
        return matrix::map<value_type, Order>(m_start_ptr, rows, cols);
    }
};

namespace dap
{
    namespace fastmath
    {
        template <typename T, typename Allocator>
        bool operator==(const Array<T, Allocator>& a, const Array<T, Allocator>& b)
        {
            if (a.size() != b.size())
            {
                return false;
            }
            for (typename Array<T, Allocator>::size_type i = 0; i < a.size(); ++i)
            {
                if (a[i] != b[i])
                {
                    return false;
                }
            }
            return true;
        }
        template <typename T, typename Allocator>
        bool operator!=(const Array<T, Allocator>& a, const Array<T, Allocator>& b)
        {
            return !(a == b);
        }
        template <typename T, typename Allocator>
        inline std::ostream& operator<<(std::ostream& out, const Array<T, Allocator>& a)
        {
            return streamContainer(out, a);
        }
    }
}
#endif // DAP_FASTMATH_ARRAY_H
