#include "dap_gtest.h"
#include "fastmath/VarArray.h"
#include <cmath>
#include <limits>
#include <vector>

using namespace testing;
using namespace dap;
using dap::fastmath::VarArray;
using dap::fastmath::Variable;

template <typename T>
using aligned_vector = std::vector<T, fastmath::AlignedAllocator<T>>;

class VarArrayTest : public Test
{
};

template <typename T, typename Alloc>
void print(const VarArray<T, Alloc>& v)
{
    for (const auto& x : v)
    {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

template <typename T, typename Alloc>
void assert_near(const VarArray<T, Alloc>& a, const VarArray<T, Alloc>& b, float eps)
{
    DAP_ASSERT_EQ(a.size(), b.size());
    for (size_t i = 0; i < a.size(); ++i)
    {
        DAP_ASSERT_NEAR(a[i], b[i], eps);
    }
}

template <typename T, typename Alloc>
void assert_eq(const VarArray<T, Alloc>& a, const VarArray<T, Alloc>& b)
{
    DAP_ASSERT_EQ(a.size(), b.size());
    for (size_t i = 0; i < a.size(); ++i)
    {
        DAP_ASSERT_EQ(a[i], b[i]);
    }
}
template <typename T, typename Alloc>
void assert_near(const std::vector<std::complex<T>, Alloc>& a,
                 const std::vector<std::complex<T>, Alloc>& b,
                 float eps)
{
    DAP_ASSERT_EQ(a.size(), b.size());
    for (size_t i = 0; i < a.size(); ++i)
    {
        DAP_ASSERT_NEAR(a[i].real(), b[i].real(), eps);
        DAP_ASSERT_NEAR(a[i].imag(), b[i].imag(), eps);
    }
}
DAP_TEST_F(VarArrayTest, empty_ctor)
{
    VarArray<Variable<int>> a{};
    DAP_ASSERT_EQ(0u, a.size());
    DAP_ASSERT_TRUE(nullptr == a.data());
    DAP_ASSERT_FALSE(a.ownsMemory());
}
DAP_TEST_F(VarArrayTest, size_ctor)
{
    size_t size = 10;
    VarArray<Variable<int>> a(size);
    DAP_ASSERT_EQ(size, a.size());
    DAP_ASSERT_TRUE(a.ownsMemory());
}
DAP_TEST_F(VarArrayTest, initializer_list)
{
    std::initializer_list<int> list({1, 2, 3, 4});
    VarArray<Variable<int>> a(list);
    DAP_ASSERT_EQ(a.size(), list.size());
    DAP_ASSERT_TRUE(a.ownsMemory());
    for (size_t i = 0; i < a.size(); i++)
    {
        DAP_ASSERT_EQ(int(i + 1), a[i]);
    }
}
DAP_TEST_F(VarArrayTest, ctor_from_ptr_owning)
{
    std::vector<int> v({1, 2, 3, 4});
    VarArray<Variable<int>> a(v.data(), v.size(), true);
    DAP_ASSERT_EQ(a.size(), v.size());
    DAP_ASSERT_TRUE(a.ownsMemory());
    for (size_t i = 0; i < a.size(); i++)
    {
        DAP_ASSERT_EQ(int(i + 1), a[i]);
    }
    size_t i = 0;
    for (auto& x : a)
    {
        DAP_ASSERT_EQ(v[i], x);
        const int val = (i + 1) * 2;
        x = val;
        DAP_ASSERT_EQ(val, a[i]);
        i++;
    }
}
DAP_TEST_F(VarArrayTest, ctor_from_ptr_not_owning)
{
    std::vector<int> v(100, 0);
    for (size_t i = 0; i < v.size(); i++)
    {
        v[i] = i;
    }
    VarArray<Variable<int>> a(v.data(), v.size(), false);
    DAP_ASSERT_EQ(a.size(), v.size());
    DAP_ASSERT_FALSE(a.ownsMemory());
    for (size_t i = 0; i < a.size(); i++)
    {
        DAP_ASSERT_EQ(int(i), a[i]);
    }
    int i = 0;
    for (auto& x : a)
    {
        DAP_ASSERT_EQ(v[i], x);
        const int val = (i + 1) * 2;
        x = val;
        DAP_ASSERT_EQ(val, a[i]);
        i++;
    }
}
DAP_TEST_F(VarArrayTest, operator_compare)
{
    VarArray<Variable<int>> a({1, 2, 3, 4, 5});
    VarArray<Variable<int>> b({1, 2, 3, 4, 5});
    assert_eq(a, b);
    DAP_ASSERT_TRUE(a == b);
    a[0] = 42;
    DAP_ASSERT_TRUE(a != b);
}
DAP_TEST_F(VarArrayTest, copy_ctor)
{
    VarArray<Variable<int>> a({1, 2, 3, 4, 5});
    VarArray<Variable<int>> b(a);
    DAP_ASSERT_TRUE(a == b);
}
DAP_TEST_F(VarArrayTest, forbidAllocation)
{
    VarArray<Variable<int>> a({1, 2, 3});
    VarArray<Variable<int>> result_no_alloc{};
    result_no_alloc.forbidAllocation(true);
    try
    {
        result_no_alloc = a;
    }
    catch (...)
    {
        DAP_ASSERT_TRUE(true);
    }
    VarArray<Variable<int>> result(a.size(), 0);
    result.forbidAllocation(true);
    result = a;
    DAP_ASSERT_EQ(a, result);
}
DAP_TEST_F(VarArrayTest, mapped_array_ctor)
{
    VarArray<Variable<int>> a({1, 2, 3, 4, 5});
    VarArray<Variable<int>> b(fastmath::array::const_map(a.data(), a.size()));
    DAP_ASSERT_TRUE(b.ownsMemory());
    DAP_ASSERT_EQ(a, b);
}
DAP_TEST_F(VarArrayTest, operator_equal)
{
    VarArray<Variable<int>> a({1, 2, 3, 4, 5});
    VarArray<Variable<int>> b = a;
    DAP_ASSERT_TRUE(a == b);
    VarArray<Variable<int>> c(a.size(), 0);
    c.forbidAllocation(true);
    c = a;
    DAP_ASSERT_EQ(a, b);
}
DAP_TEST_F(VarArrayTest, ref)
{
    bool ownMemory = false;
    VarArray<Variable<int>> a({1, 2, 3, 4, 5});
    VarArray<Variable<int>> b(a, ownMemory);
    DAP_ASSERT_TRUE(!b.ownsMemory());
    DAP_ASSERT_TRUE(a == b);
    b[0] = 42;
    DAP_ASSERT_EQ(a[0], b[0]);
    DAP_ASSERT_EQ(a[0], 42);
}
DAP_TEST_F(VarArrayTest, const_ref)
{
    bool ownMemory = false;
    VarArray<Variable<int>> a({1, 2, 3, 4, 5});
    const VarArray<Variable<int>>& cref = VarArray<Variable<int>>(a, ownMemory);
    DAP_ASSERT_TRUE(!cref.ownsMemory());
    DAP_ASSERT_EQ(a, cref);
    // cref[0] = 42; // Error
}
DAP_TEST_F(VarArrayTest, vector)
{
    bool ownMemory = false;
    std::vector<int> a({1, 2, 3, 4, 5});
    const VarArray<Variable<int>>& cref = VarArray<Variable<int>>(a, ownMemory);
    DAP_ASSERT_TRUE(!cref.ownsMemory());
    for (size_t i = 0; i < a.size(); i++)
    {
        DAP_ASSERT_EQ(a[i], cref[i]);
    }
    // cref[0] = 42; // Error

    VarArray<Variable<int>> ref(a, ownMemory);
    DAP_ASSERT_TRUE(!ref.ownsMemory());
    ref[0] = 42;
    DAP_ASSERT_EQ(42, a[0]);
    for (size_t i = 0; i < a.size(); i++)
    {
        DAP_ASSERT_EQ(a[i], ref[i]);
    }
}
DAP_TEST_F(VarArrayTest, ref2)
{
    VarArray<Variable<int>> a({1, 2, 3, 4, 5});
    VarArray<Variable<int>> b(a.data(), 1, 3, true);
    VarArray<Variable<int>> c(a.data(), 1, 3, false);
    DAP_ASSERT_TRUE(b.ownsMemory());
    DAP_ASSERT_TRUE(!c.ownsMemory());

    DAP_ASSERT_EQ(2u, b.size());
    DAP_ASSERT_EQ(2u, c.size());

    b[0] = 42;
    DAP_ASSERT_EQ(2, a[1]);
    DAP_ASSERT_EQ(42, b[0]);
    c[0] = 42;
    DAP_ASSERT_EQ(a[1], c[0]);
    DAP_ASSERT_EQ(42, a[1]);
}
DAP_TEST_F(VarArrayTest, range_loop)
{
    std::initializer_list<int> list({1, 2, 3, 4});
    std::vector<int> v(list);
    VarArray<Variable<int>> a(list);
    DAP_ASSERT_EQ(a.size(), list.size());
    DAP_ASSERT_TRUE(a.ownsMemory());
    int i = 0;
    for (auto x : a)
    {
        DAP_ASSERT_EQ(v[i++], x);
    }
}
DAP_TEST_F(VarArrayTest, resize)
{
    int value = 42;
    VarArray<Variable<int>> a(10, value);
    DAP_ASSERT_EQ(10u, a.size());
    a.resize(20);
    DAP_ASSERT_EQ(20u, a.size());
}
DAP_TEST_F(VarArrayTest, push_back)
{
    VarArray<Variable<int>> a(10, 5);
    DAP_ASSERT_EQ(10u, a.size());
    a.push_back(42);
    DAP_ASSERT_EQ(11u, a.size());
    DAP_ASSERT_EQ(42, a.back());
    DAP_ASSERT_EQ(5, a.back(1));
}
DAP_TEST_F(VarArrayTest, fill_and_clear)
{
    size_t size = 10;
    int value = 42;
    VarArray<Variable<int>> a(size, value);
    DAP_ASSERT_EQ(a.size(), size);
    DAP_ASSERT_TRUE(a.ownsMemory());
    for (auto i : a)
    {
        DAP_ASSERT_EQ(value, i);
    }

    a.clear();
    DAP_ASSERT_EQ(a.size(), size);
    DAP_ASSERT_TRUE(a.ownsMemory());
    for (auto i : a)
    {
        DAP_ASSERT_EQ(0, i);
    }
}
DAP_TEST_F(VarArrayTest, add_array_lazy)
{
    VarArray<Variable<int>> a({1, 2, 3});
    VarArray<Variable<int>> b({4, 5, 6});
    VarArray<Variable<int>> c({7, 8, 9});

    // no allocation during operation
    VarArray<Variable<int>> result_no_alloc(a.size(), 0);
    result_no_alloc.forbidAllocation(true);
    result_no_alloc = a.map() + b.map() + c.map();

    // will allocate during operation
    VarArray<Variable<int>> result = a.map() + b.map() + c.map();

    VarArray<Variable<int>> expected({12, 15, 18});
    DAP_ASSERT_EQ(expected, result_no_alloc);
    DAP_ASSERT_EQ(expected, result);
}
DAP_TEST_F(VarArrayTest, sub_array_lazy)
{
    VarArray<Variable<int>> a({1, 2, 3});
    VarArray<Variable<int>> b({4, 5, 6});
    VarArray<Variable<int>> c({7, 8, 9});
    VarArray<Variable<int>> result = a.map() - b.map() - c.map();

    VarArray<Variable<int>> result_no_alloc(a.size(), 0);
    result_no_alloc.forbidAllocation(true);
    result_no_alloc = a.map() - b.map() - c.map();

    VarArray<Variable<int>> expected({-10, -11, -12});
    DAP_ASSERT_EQ(expected, result);
    DAP_ASSERT_EQ(expected, result_no_alloc);
}
DAP_TEST_F(VarArrayTest, mul_array_lazy)
{
    VarArray<Variable<int>> a({1, 2, 3});
    VarArray<Variable<int>> b({4, 5, 6});
    VarArray<Variable<int>> c({7, 8, 9});
    VarArray<Variable<int>> result = a.map() * b.map() * c.map();

    VarArray<Variable<int>> result_no_alloc(a.size(), 0);
    result_no_alloc.forbidAllocation(true);
    result_no_alloc = a.map() * b.map() * c.map();

    VarArray<Variable<int>> expected({28, 80, 162});
    DAP_ASSERT_EQ(expected, result);
    DAP_ASSERT_EQ(expected, result_no_alloc);
}
DAP_TEST_F(VarArrayTest, div_array_lazy)
{
    VarArray<Variable<float>> a({1.0f, 2.0f, 3.0f});
    VarArray<Variable<float>> b({4.0f, 5.0f, 6.0f});
    VarArray<Variable<float>> c({7.0f, 8.0f, 9.0f});
    VarArray<Variable<float>> result = a.map() / b.map() / c.map();

    VarArray<Variable<float>> result_no_alloc(a.size(), 0);
    result_no_alloc.forbidAllocation(true);
    result_no_alloc = a.map() / b.map() / c.map();

    VarArray<Variable<float>> expected({0.25f / 7.0f, 0.4f / 8.0f, 0.5f / 9.0f});
    DAP_ASSERT_EQ(expected, result);
    DAP_ASSERT_EQ(expected, result_no_alloc);
}
DAP_TEST_F(VarArrayTest, mixed_op_array_lazy)
{
    VarArray<Variable<float>> a({1.0f, 2.0f, 3.0f});
    VarArray<Variable<float>> b({4.0f, 5.0f, 6.0f});
    VarArray<Variable<float>> c({7.0f, 8.0f, 9.0f});
    VarArray<Variable<float>> result = a.map() * b.map() + c.map() * 2.0f;

    VarArray<Variable<float>> result_no_alloc(a.size(), 0);
    result_no_alloc.forbidAllocation(true);
    result_no_alloc = a.map() * b.map() + c.map() * 2.0f;

    VarArray<Variable<float>> expected(
        {1.0f * 4.0f + 7.0f * 2.0f, 2.0f * 5.0f + 8.0f * 2.0f, 3.0f * 6.0f + 9.0f * 2.0f});
    DAP_ASSERT_EQ(expected, result);
    DAP_ASSERT_EQ(expected, result_no_alloc);
}
DAP_TEST_F(VarArrayTest, eigen_array_ops)
{
    VarArray<Variable<float>> a(10, 1.0f);
    a.forbidAllocation(true);
    DAP_ASSERT_TRUE(a.map().all());
    a.fill(0);
    a[0] = 1.0f;
    DAP_ASSERT_TRUE(a.map().any());
    DAP_ASSERT_TRUE(a.map().allFinite());
    a.map() << -1, -2, -3, -4, -5, -6, -7, -8, -9, -10;
    DAP_ASSERT_EQ(VarArray<Variable<float>>({-1, -2, -3, -4, -5, -6, -7, -8, -9, -10}), a);
    a = a.map().abs();
    DAP_ASSERT_EQ(VarArray<Variable<float>>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}), a);
    a = a.map().abs2();
    DAP_ASSERT_EQ(VarArray<Variable<float>>({1, 4, 9, 16, 25, 36, 49, 64, 81, 100}), a);
    a.linspace(1.0f, 10.0f);
    DAP_ASSERT_EQ(VarArray<Variable<float>>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}), a);
    const float sum = a.map().sum();
    DAP_ASSERT_EQ(55, sum);
    const float sum2 = a.map().square().sum();
    DAP_ASSERT_EQ(385, sum2);
    a.normalize();
    a.map() *= std::sqrt(sum2);
    DAP_ASSERT_EQ(VarArray<Variable<float>>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}), a);
    // etc
}
DAP_TEST_F(VarArrayTest, add_vec_lazy)
{
    VarArray<Variable<int>> a({1, 2, 3});
    VarArray<Variable<int>> b({4, 5, 6});
    VarArray<Variable<int>> c({7, 8, 9});

    // no allocation during operation
    VarArray<Variable<int>> result_no_alloc(a.size(), 0);
    result_no_alloc.forbidAllocation(true);
    result_no_alloc = a.vec() + b.vec() + c.vec();

    // will allocate during operation
    VarArray<Variable<int>> result = a.vec() + b.vec() + c.vec();

    VarArray<Variable<int>> expected({12, 15, 18});
    DAP_ASSERT_EQ(expected, result_no_alloc);
    DAP_ASSERT_EQ(expected, result);
}
DAP_TEST_F(VarArrayTest, sub_vec_lazy)
{
    VarArray<Variable<int>> a({1, 2, 3});
    VarArray<Variable<int>> b({4, 5, 6});
    VarArray<Variable<int>> c({7, 8, 9});
    VarArray<Variable<int>> result = a.vec() - b.vec() - c.vec();

    VarArray<Variable<int>> result_no_alloc(a.size(), 0);
    result_no_alloc.forbidAllocation(true);
    result_no_alloc = a.vec() - b.vec() - c.vec();

    VarArray<Variable<int>> expected({-10, -11, -12});
    DAP_ASSERT_EQ(expected, result);
    DAP_ASSERT_EQ(expected, result_no_alloc);
}
DAP_TEST_F(VarArrayTest, tranpose_vec_lazy)
{
    VarArray<Variable<int>> a({1, 2, 3});
    VarArray<Variable<int>> b({4, 5, 6});
    a.forbidAllocation(true);
    b.forbidAllocation(true);
    int result = a.vec().transpose() * b.vec();
    DAP_ASSERT_EQ(32, result);
}
DAP_TEST_F(VarArrayTest, dot_product_vec_lazy)
{
    VarArray<Variable<int>> a({1, 2, 3});
    VarArray<Variable<int>> b({4, 5, 6});
    a.forbidAllocation(true);
    b.forbidAllocation(true);
    int result = a.vec().dot(b.vec());
    DAP_ASSERT_EQ(32, result);
}
DAP_TEST_F(VarArrayTest, matrix_fixed_size)
{
    VarArray<Variable<int>> a({1, 2, 3, 4});
    VarArray<Variable<int>> v({5, 6});

    VarArray<Variable<int>> result = a.mat<2, 2>() * v.vec();
    VarArray<Variable<int>> expected({23, 34});
    DAP_ASSERT_EQ(expected, result);
}

#ifndef EIGEN_NO_MALLOC
DAP_TEST_F(VarArrayTest, matrix_dynamic)
{
    VarArray<Variable<int>> a({1, 2, 3, 4});
    VarArray<Variable<int>> v({5, 6});

    VarArray<Variable<int>> result = a.mat(2, 2) * v.vec();
    VarArray<Variable<int>> expected({23, 34});
    DAP_ASSERT_EQ(expected, result);
}
#endif
