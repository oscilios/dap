#ifndef DAP_FASTMATH_VARIABLE_ARRAY_H
#define DAP_FASTMATH_VARIABLE_ARRAY_H

#include "Array.h"
#include "Variable.h"

namespace dap
{
    namespace fastmath
    {
        template <typename T, typename Allocator>
        class VarArray;
    }
}
template <typename T, typename Allocator = dap::fastmath::AlignedAllocator<typename T::value_type>>
class dap::fastmath::VarArray
{
    static_assert(isSame<T, Variable<typename T::value_type>>(),
                  "dap::fastmath::Array does not support non arithmetic types");

public:
    using underlying_type            = typename T::value_type;
    using underlying_pointer         = underlying_type*;
    using underlying_const_pointer   = const underlying_type*;
    using underlying_reference       = underlying_type&;
    using underlying_const_reference = const underlying_type&;
    using size_type                  = std::size_t;
    using difference_type            = std::ptrdiff_t;
    using underlying_array_type      = Array<underlying_type, Allocator>;

    using value_type = T;
    using array_type = VarArray<value_type, Allocator>;

private:
    Array<underlying_type, Allocator> m_array;

public:
    explicit VarArray() = default;
    explicit VarArray(size_type size, underlying_const_reference value = underlying_type(0))
    : m_array(size, value)
    {
    }
    VarArray(underlying_pointer ptr, size_type start, size_type end, bool ownMemory = true)
    : m_array(ptr, start, end, ownMemory)
    {
    }
    VarArray(underlying_const_pointer ptr, size_type start, size_type end)
    : m_array(ptr, start, end)
    {
    }
    VarArray(const std::initializer_list<underlying_type>& list)
    : m_array(list)
    {
    }
    VarArray(underlying_pointer ptr, size_type size, bool ownMemory = true)
    : VarArray(ptr, 0, size, ownMemory)
    {
    }
    VarArray(underlying_const_pointer ptr, size_type size)
    : VarArray(ptr, 0, size)
    {
    }
    explicit VarArray(underlying_array_type& other, bool ownMemory = true)
    : VarArray(other.data(), 0, other.size(), ownMemory)
    {
    }
    explicit VarArray(const underlying_array_type& other)
    : VarArray(other.data(), 0, other.size())
    {
    }
    VarArray(underlying_array_type& other, size_type start, size_type end, bool ownMemory = true)
    : VarArray(other.data(), start, end, ownMemory)
    {
    }
    VarArray(const underlying_array_type& other, size_type start, size_type end)
    : VarArray(other.data(), start, end)
    {
    }
    explicit VarArray(underlying_array_type&& other) noexcept
    : m_array(std::move(other))
    {
    }
    VarArray(VarArray& other, bool ownMemory = true)
    : VarArray(other.data(), 0, other.size(), ownMemory)
    {
    }
    VarArray(const VarArray& other)
    : VarArray(other.data(), 0, other.size())
    {
    }
    VarArray(VarArray& other, size_type start, size_type end, bool ownMemory = true)
    : VarArray(other.data(), start, end, ownMemory)
    {
    }
    VarArray(const VarArray& other, size_type start, size_type end)
    : VarArray(other.data(), start, end)
    {
    }
    VarArray(VarArray&& other) noexcept
    : m_array(std::move(other.m_array))
    {
    }

    template <typename VectorAlloc>
    explicit VarArray(const std::vector<underlying_type, VectorAlloc>& v, bool ownMemory = true)
    : m_array(v, ownMemory)
    {
    }
    virtual ~VarArray() = default;
    VarArray& operator=(const underlying_array_type& other)
    {
        m_array = other;
        return *this; // NOLINT
    }
    VarArray& operator=(const VarArray& other) = default;
    VarArray& operator=(VarArray&& other) noexcept
    {
        m_array = std::move(other.m_array);
        return *this; // NOLINT
    }
    VarArray& operator=(const std::initializer_list<underlying_type>& list)
    {
        m_array = list;
        return *this; // NOLINT
    }
    underlying_pointer data()
    {
        return m_array.data();
    }
    underlying_const_pointer data() const
    {
        return m_array.data();
    }
    auto begin()
    {
        return m_array.begin();
    }
    auto begin() const
    {
        return m_array.begin();
    }
    auto end()
    {
        return m_array.end();
    }
    auto end() const
    {
        return m_array.end();
    }
    underlying_reference operator[](size_type n)
    {
        return m_array[n];
    }
    underlying_const_reference operator[](size_type n) const
    {
        return m_array[n];
    }
    underlying_reference back(size_type n = 0)
    {
        return m_array.back(n);
    }
    underlying_const_reference back(size_type n = 0) const
    {
        return m_array.back(n);
    }
    size_type size() const
    {
        return m_array.size();
    }
    void resize(size_type size, underlying_const_reference value = 0)
    {
        m_array.resize(size, value);
    }
    void push_back(underlying_const_reference value = 0)
    {
        m_array.push_back(value);
    }
    bool ownsMemory() const
    {
        return m_array.ownsMemory();
    }
    bool isAllocationForbidden() const
    {
        return m_array.isAllocationForbidden();
    }
    void forbidAllocation(bool b)
    {
        m_array.forbidAllocation(b);
    }
    void copy(underlying_const_pointer ptr, size_type size)
    {
        m_array.copy(ptr, size);
    }
    void copy(underlying_const_pointer ptr, size_type start, size_type end)
    {
        m_array.copy(ptr, start, end);
    }
    void copy(const underlying_array_type& other)
    {
        m_array.copy(other.data(), other.size());
    }

    // operations
    void fill(underlying_const_reference value)
    {
        m_array.fill(value);
    }
    void fill(underlying_const_reference value, size_type from, size_type to)
    {
        m_array.fill(value, from, to);
    }
    void reset()
    {
        m_array.clear();
    }
    void clear()
    {
        m_array.clear();
    }
    bool isEqual(const VarArray& other) const
    {
        return m_array == other.m_array;
    }
    underlying_type rms() const
    {
        return m_array.rms();
    }
    underlying_type accumulate() const
    {
        m_array.accumulate();
    }

    VarArray& linspace(const underlying_type& low, const underlying_type& high)
    {
        m_array.linspace(low, high);
        return *this;
    }
    VarArray& normalize()
    {
        m_array.normalize();
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // lazy operators interface
    ////////////////////////////////////////////////////////////////////////////////

    //*******************
    //* As a map
    //*******************

    const array::ConstMapType<underlying_type> map() const
    {
        return array::const_map(m_array.data(), m_array.size());
    }
    array::MapType<underlying_type> map()
    {
        return array::map(m_array.data(), m_array.size());
    }
    explicit VarArray(array::ConstMapType<underlying_type> mappedArray)
    : m_array(mappedArray)
    {
    }

    template <typename BinaryOp>
    DAP_NO_EXPLICIT_CTOR VarArray(const BinaryOp& op)
    : m_array(op)
    {
    }

    template <typename BinaryOp>
    VarArray& operator=(const BinaryOp& op)
    {
        m_array = op;
        return *this; // NOLINT
    }

    //*******************
    //* As a vector
    //*******************

    explicit VarArray(const vector::VectorType<underlying_type>& vec)
    : m_array(vec)
    {
    }
    VarArray& operator=(const vector::VectorType<underlying_type>& vec)
    {
        m_array = vec;
        return *this; // NOLINT
    }

    const vector::ConstMapType<underlying_type> vec() const
    {
        return vector::const_map<underlying_type>(m_array.data(), m_array.size());
    }
    vector::MapType<underlying_type> vec()
    {
        return vector::map(m_array.data(), m_array.size());
    }

    //*************************
    //* As a fixed size matrix
    //*************************

    template <size_t Rows, size_t Cols, int Order = matrix::ColMajor>
    explicit VarArray(const matrix::MatrixType<underlying_type, Rows, Cols, Order>& mat)
    : m_array(mat)
    {
    }
    template <size_t Rows, size_t Cols, int Order = matrix::ColMajor>
    VarArray& operator=(const matrix::MatrixType<underlying_type, Rows, Cols, Order>& mat)
    {
        m_array = mat;
        return *this; // NOLINT
    }

    template <size_t Rows, size_t Cols, int Order = matrix::ColMajor>
    matrix::ConstMapType<underlying_type, Rows, Cols, Order> mat() const
    {
        return matrix::const_map<underlying_type, Cols, Rows, Order>(m_array.data(),
                                                                     m_array.size());
    }
    template <size_t Rows, size_t Cols, int Order = matrix::ColMajor>
    matrix::MapType<underlying_type, Rows, Cols, Order> mat()
    {
        return matrix::map<underlying_type, Cols, Rows, Order>(m_array.data(), m_array.size());
    }

    //*****************************
    //* As a dynamic size matrix
    //*****************************

    template <int Order = matrix::ColMajor>
    explicit VarArray(const matrix::MatrixTypeDynamic<underlying_type, Order>& mat)
    : m_array(mat)
    {
    }
    template <int Order       = matrix::ColMajor>
    VarArray& operator=(const matrix::MatrixTypeDynamic<underlying_type, Order>& mat)
    {
        m_array = mat;
        return *this; // NOLINT
    }

    // non realtime safe since it may allocate memory
    template <int Order = matrix::ColMajor>
    matrix::ConstMapTypeDynamic<underlying_type, Order> mat(size_t rows, size_t cols) const
    {
        assert(rows * cols == m_array.size());
        return matrix::const_map<underlying_type, Order>(m_array.data(), rows, cols);
    }
    template <int Order = matrix::ColMajor>
    matrix::MapTypeDynamic<underlying_type, Order> mat(size_t rows, size_t cols)
    {
        assert(rows * cols == m_array.size());
        return matrix::const_map<underlying_type, Order>(m_array.data(), rows, cols);
    }
};
namespace dap
{
    namespace fastmath
    {
        template <typename T, typename Allocator>
        bool operator==(const VarArray<T, Allocator>& a, const VarArray<T, Allocator>& b)
        {
            return a.isEqual(b);
        }
        template <typename T, typename Allocator>
        bool operator!=(const VarArray<T, Allocator>& a, const VarArray<T, Allocator>& b)
        {
            return !(a == b);
        }
        template <typename T, typename Allocator>
        inline std::ostream& operator<<(std::ostream& out, const VarArray<T, Allocator>& a)
        {
            return streamContainer(out, a);
        }
    }
}

#endif // DAP_FASTMATH_VARIABLE_ARRAY_H
