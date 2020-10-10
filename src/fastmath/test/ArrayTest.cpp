#include <gtest/gtest.h>
#include "fastmath/Array.h"
#include <cmath>
#include <limits>
#include <vector>

using namespace testing;
using namespace dap;
using dap::fastmath::Array;

template <typename T>
using aligned_vector = std::vector<T, fastmath::AlignedAllocator<T>>;

template <typename T, typename Alloc>
void print(const Array<T, Alloc>& v)
{
    for (const auto& x : v)
    {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

template <typename T, typename Alloc>
void assert_near(const Array<T, Alloc>& a, const Array<T, Alloc>& b, float eps)
{
    ASSERT_EQ(a.size(), b.size());
    for (size_t i = 0; i < a.size(); ++i)
    {
        ASSERT_NEAR(a[i], b[i], eps);
    }
}

template <typename T, typename Alloc>
void assert_eq(const Array<T, Alloc>& a, const Array<T, Alloc>& b)
{
    ASSERT_EQ(a.size(), b.size());
    for (size_t i = 0; i < a.size(); ++i)
    {
        ASSERT_EQ(a[i], b[i]);
    }
}
template <typename T, typename Alloc>
void assert_near(const std::vector<std::complex<T>, Alloc>& a,
                 const std::vector<std::complex<T>, Alloc>& b,
                 float eps)
{
    ASSERT_EQ(a.size(), b.size());
    for (size_t i = 0; i < a.size(); ++i)
    {
        ASSERT_NEAR(a[i].real(), b[i].real(), eps);
        ASSERT_NEAR(a[i].imag(), b[i].imag(), eps);
    }
}
TEST(ArrayTest, empty_ctor)
{
    Array<int> a {};
    ASSERT_EQ(0u, a.size());
    ASSERT_TRUE(nullptr == a.data());
    ASSERT_FALSE(a.ownsMemory());
}
TEST(ArrayTest, size_ctor)
{
    size_t size = 10;
    Array<int> a(size);
    ASSERT_EQ(size, a.size());
    ASSERT_TRUE(a.ownsMemory());
}
TEST(ArrayTest, initializer_list)
{
    std::initializer_list<int> list({1, 2, 3, 4});
    Array<int> a(list);
    ASSERT_EQ(a.size(), list.size());
    ASSERT_TRUE(a.ownsMemory());
    for (size_t i = 0; i < a.size(); i++)
    {
        ASSERT_EQ(int(i + 1), a[i]);
    }
}
TEST(ArrayTest, ctor_from_ptr_owning)
{
    std::vector<int> v({1, 2, 3, 4});
    Array<int> a(v.data(), v.size(), true);
    ASSERT_EQ(a.size(), v.size());
    ASSERT_TRUE(a.ownsMemory());
    for (size_t i = 0; i < a.size(); i++)
    {
        ASSERT_EQ(int(i + 1), a[i]);
    }
    size_t i = 0;
    for (auto& x : a)
    {
        ASSERT_EQ(v[i], x);
        const int val = (i + 1) * 2;
        x = val;
        ASSERT_EQ(val, a[i]);
        i++;
    }
}
TEST(ArrayTest, ctor_from_ptr_not_owning)
{
    std::vector<int> v(100, 0);
    for (size_t i = 0; i < v.size(); i++)
    {
        v[i] = i;
    }
    Array<int> a(v.data(), v.size(), false);
    ASSERT_EQ(a.size(), v.size());
    ASSERT_FALSE(a.ownsMemory());
    for (size_t i = 0; i < a.size(); i++)
    {
        ASSERT_EQ(int(i), a[i]);
    }
    int i = 0;
    for (auto& x : a)
    {
        ASSERT_EQ(v[i], x);
        const int val = (i + 1) * 2;
        x = val;
        ASSERT_EQ(val, a[i]);
        i++;
    }
}
TEST(ArrayTest, operator_compare)
{
    Array<int> a({1, 2, 3, 4, 5});
    Array<int> b({1, 2, 3, 4, 5});
    assert_eq(a, b);
    ASSERT_TRUE(a == b);
    a[0] = 42;
    ASSERT_TRUE(a != b);
}
TEST(ArrayTest, copy_ctor)
{
    Array<int> a({1, 2, 3, 4, 5});
    Array<int> b(a);
    ASSERT_TRUE(a == b);
}
TEST(ArrayTest, forbidAllocation)
{
    Array<int> a({1, 2, 3});
    Array<int> result_no_alloc {};
    result_no_alloc.forbidAllocation(true);
    try
    {
        result_no_alloc = a;
    }
    catch (...)
    {
        ASSERT_TRUE(true);
    }
    Array<int> result(a.size(), 0);
    result.forbidAllocation(true);
    result = a;
    ASSERT_EQ(a, result);
}
TEST(ArrayTest, mapped_array_ctor)
{
    Array<int> a({1, 2, 3, 4, 5});
    Array<int> b(fastmath::array::const_map(a.data(), a.size()));
    ASSERT_TRUE(b.ownsMemory());
    ASSERT_EQ(a, b);
}
TEST(ArrayTest, operator_equal)
{
    Array<int> a({1, 2, 3, 4, 5});
    Array<int> b = a;
    ASSERT_TRUE(a == b);
    Array<int> c(a.size(), 0);
    c.forbidAllocation(true);
    c = a;
    ASSERT_EQ(a, b);
}
TEST(ArrayTest, ref)
{
    bool ownMemory = false;
    Array<int> a({1, 2, 3, 4, 5});
    Array<int> b(a, ownMemory);
    ASSERT_TRUE(!b.ownsMemory());
    ASSERT_TRUE(a == b);
    b[0] = 42;
    ASSERT_EQ(a[0], b[0]);
    ASSERT_EQ(a[0], 42);
}
TEST(ArrayTest, const_ref)
{
    bool ownMemory = false;
    Array<int> a({1, 2, 3, 4, 5});
    const Array<int>& cref = Array<int>(a, ownMemory);
    ASSERT_TRUE(!cref.ownsMemory());
    ASSERT_EQ(a, cref);
    // cref[0] = 42; // Error
}
TEST(ArrayTest, vector)
{
    bool ownMemory = false;
    std::vector<int> a({1, 2, 3, 4, 5});
    const Array<int>& cref = Array<int>(a, ownMemory);
    ASSERT_TRUE(!cref.ownsMemory());
    for (size_t i = 0; i < a.size(); i++)
    {
        ASSERT_EQ(a[i], cref[i]);
    }
    // cref[0] = 42; // Error

    Array<int> ref(a, ownMemory);
    ASSERT_TRUE(!ref.ownsMemory());
    ref[0] = 42;
    ASSERT_EQ(42, a[0]);
    for (size_t i = 0; i < a.size(); i++)
    {
        ASSERT_EQ(a[i], ref[i]);
    }
}
TEST(ArrayTest, ref2)
{
    Array<int> a({1, 2, 3, 4, 5});
    Array<int> b(a.data(), 1, 3, true);
    Array<int> c(a.data(), 1, 3, false);
    ASSERT_TRUE(b.ownsMemory());
    ASSERT_TRUE(!c.ownsMemory());

    ASSERT_EQ(2u, b.size());
    ASSERT_EQ(2u, c.size());

    b[0] = 42;
    ASSERT_EQ(2, a[1]);
    ASSERT_EQ(42, b[0]);
    c[0] = 42;
    ASSERT_EQ(a[1], c[0]);
    ASSERT_EQ(42, a[1]);
}
TEST(ArrayTest, range_loop)
{
    std::initializer_list<int> list({1, 2, 3, 4});
    std::vector<int> v(list);
    Array<int> a(list);
    ASSERT_EQ(a.size(), list.size());
    ASSERT_TRUE(a.ownsMemory());
    int i = 0;
    for (auto x : a)
    {
        ASSERT_EQ(v[i++], x);
    }
}
TEST(ArrayTest, resize)
{
    int value = 42;
    Array<int> a(10, value);
    ASSERT_EQ(10u, a.size());
    a.resize(20);
    ASSERT_EQ(20u, a.size());
}
TEST(ArrayTest, push_back)
{
    Array<int> a(10, 5);
    ASSERT_EQ(10u, a.size());
    a.push_back(42);
    ASSERT_EQ(11u, a.size());
    ASSERT_EQ(42, a.back());
    ASSERT_EQ(5, a.back(1));
}
TEST(ArrayTest, fill_and_clear)
{
    size_t size = 10;
    int value = 42;
    Array<int> a(size, value);
    ASSERT_EQ(a.size(), size);
    ASSERT_TRUE(a.ownsMemory());
    for (auto i : a)
    {
        ASSERT_EQ(value, i);
    }

    a.clear();
    ASSERT_EQ(a.size(), size);
    ASSERT_TRUE(a.ownsMemory());
    for (auto i : a)
    {
        ASSERT_EQ(0, i);
    }
}
TEST(ArrayTest, add_array_lazy)
{
    Array<int> a({1, 2, 3});
    Array<int> b({4, 5, 6});
    Array<int> c({7, 8, 9});

    // no allocation during operation
    Array<int> result_no_alloc(a.size(), 0);
    result_no_alloc.forbidAllocation(true);
    result_no_alloc = a.map() + b.map() + c.map();

    // will allocate during operation
    Array<int> result = a.map() + b.map() + c.map();

    Array<int> expected({12, 15, 18});
    ASSERT_EQ(expected, result_no_alloc);
    ASSERT_EQ(expected, result);
}
TEST(ArrayTest, sub_array_lazy)
{
    Array<int> a({1, 2, 3});
    Array<int> b({4, 5, 6});
    Array<int> c({7, 8, 9});
    Array<int> result = a.map() - b.map() - c.map();

    Array<int> result_no_alloc(a.size(), 0);
    result_no_alloc.forbidAllocation(true);
    result_no_alloc = a.map() - b.map() - c.map();

    Array<int> expected({-10, -11, -12});
    ASSERT_EQ(expected, result);
    ASSERT_EQ(expected, result_no_alloc);
}
TEST(ArrayTest, mul_array_lazy)
{
    Array<int> a({1, 2, 3});
    Array<int> b({4, 5, 6});
    Array<int> c({7, 8, 9});
    Array<int> result = a.map() * b.map() * c.map();

    Array<int> result_no_alloc(a.size(), 0);
    result_no_alloc.forbidAllocation(true);
    result_no_alloc = a.map() * b.map() * c.map();

    Array<int> expected({28, 80, 162});
    ASSERT_EQ(expected, result);
    ASSERT_EQ(expected, result_no_alloc);
}
TEST(ArrayTest, div_array_lazy)
{
    Array<float> a({1.0f, 2.0f, 3.0f});
    Array<float> b({4.0f, 5.0f, 6.0f});
    Array<float> c({7.0f, 8.0f, 9.0f});
    Array<float> result = a.map() / b.map() / c.map();

    Array<float> result_no_alloc(a.size(), 0);
    result_no_alloc.forbidAllocation(true);
    result_no_alloc = a.map() / b.map() / c.map();

    Array<float> expected({0.25f / 7.0f, 0.4f / 8.0f, 0.5f / 9.0f});
    ASSERT_EQ(expected, result);
    ASSERT_EQ(expected, result_no_alloc);
}
TEST(ArrayTest, mixed_op_array_lazy)
{
    Array<float> a({1.0f, 2.0f, 3.0f});
    Array<float> b({4.0f, 5.0f, 6.0f});
    Array<float> c({7.0f, 8.0f, 9.0f});
    Array<float> result = a.map() * b.map() + c.map() * 2.0f;

    Array<float> result_no_alloc(a.size(), 0);
    result_no_alloc.forbidAllocation(true);
    result_no_alloc = a.map() * b.map() + c.map() * 2.0f;

    Array<float> expected(
        {1.0f * 4.0f + 7.0f * 2.0f, 2.0f * 5.0f + 8.0f * 2.0f, 3.0f * 6.0f + 9.0f * 2.0f});
    ASSERT_EQ(expected, result);
    ASSERT_EQ(expected, result_no_alloc);
}
TEST(ArrayTest, eigen_array_ops)
{
    Array<float> a(10, 1.0f);
    a.forbidAllocation(true);
    ASSERT_TRUE(a.map().all());
    a.fill(0);
    a[0] = 1.0f;
    ASSERT_TRUE(a.map().any());
    ASSERT_TRUE(a.map().allFinite());
    a.map() << -1, -2, -3, -4, -5, -6, -7, -8, -9, -10;
    ASSERT_EQ(Array<float>({-1, -2, -3, -4, -5, -6, -7, -8, -9, -10}), a);
    a = a.map().abs();
    ASSERT_EQ(Array<float>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}), a);
    a = a.map().abs2();
    ASSERT_EQ(Array<float>({1, 4, 9, 16, 25, 36, 49, 64, 81, 100}), a);
    a.linspace(1.0f, 10.0f);
    ASSERT_EQ(Array<float>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}), a);
    const float sum = a.map().sum();
    ASSERT_EQ(55, sum);
    const float sum2 = a.map().square().sum();
    ASSERT_EQ(385, sum2);
    a.normalize();
    a.map() *= std::sqrt(sum2);
    ASSERT_EQ(Array<float>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}), a);
    // etc
}
TEST(ArrayTest, add_vec_lazy)
{
    Array<int> a({1, 2, 3});
    Array<int> b({4, 5, 6});
    Array<int> c({7, 8, 9});

    // no allocation during operation
    Array<int> result_no_alloc(a.size(), 0);
    result_no_alloc.forbidAllocation(true);
    result_no_alloc = a.vec() + b.vec() + c.vec();

    // will allocate during operation
    Array<int> result = a.vec() + b.vec() + c.vec();

    Array<int> expected({12, 15, 18});
    ASSERT_EQ(expected, result_no_alloc);
    ASSERT_EQ(expected, result);
}
TEST(ArrayTest, sub_vec_lazy)
{
    Array<int> a({1, 2, 3});
    Array<int> b({4, 5, 6});
    Array<int> c({7, 8, 9});
    Array<int> result = a.vec() - b.vec() - c.vec();

    Array<int> result_no_alloc(a.size(), 0);
    result_no_alloc.forbidAllocation(true);
    result_no_alloc = a.vec() - b.vec() - c.vec();

    Array<int> expected({-10, -11, -12});
    ASSERT_EQ(expected, result);
    ASSERT_EQ(expected, result_no_alloc);
}
TEST(ArrayTest, tranpose_vec_lazy)
{
    Array<int> a({1, 2, 3});
    Array<int> b({4, 5, 6});
    a.forbidAllocation(true);
    b.forbidAllocation(true);
    int result = a.vec().transpose() * b.vec();
    ASSERT_EQ(32, result);
}
TEST(ArrayTest, dot_product_vec_lazy)
{
    Array<int> a({1, 2, 3});
    Array<int> b({4, 5, 6});
    a.forbidAllocation(true);
    b.forbidAllocation(true);
    int result = a.vec().dot(b.vec());
    ASSERT_EQ(32, result);
}
TEST(ArrayTest, matrix_fixed_size)
{
    Array<int> a({1, 2, 3, 4});
    Array<int> v({5, 6});

    Array<int> result = a.mat<2, 2>() * v.vec();
    Array<int> expected({23, 34});
    ASSERT_EQ(expected, result);
}

#ifndef EIGEN_NO_MALLOC
TEST(ArrayTest, matrix_dynamic)
{
    Array<int> a({1, 2, 3, 4});
    Array<int> v({5, 6});

    Array<int> result = a.mat(2, 2) * v.vec();
    Array<int> expected({23, 34});
    ASSERT_EQ(expected, result);
}
#endif
