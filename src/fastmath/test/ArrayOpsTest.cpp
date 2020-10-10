#include <gtest/gtest.h>
#include "fastmath/ArrayOps.h"
#include "fastmath/AlignedVector.h"
#include <cmath>
#include <limits>

using namespace testing;
using namespace dap;

template <typename T>
using aligned_vector = dap::fastmath::AlignedVector<T>;

template <typename T, typename Alloc>
void print(const std::vector<T, Alloc>& v)
{
    for (const auto& x : v)
    {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

template <typename T, typename Alloc>
void assert_near(const std::vector<T, Alloc>& a, const std::vector<T, Alloc>& b, float eps)
{
    ASSERT_EQ(a.size(), b.size());
    for (size_t i = 0; i < a.size(); ++i)
    {
        ASSERT_NEAR(a[i], b[i], eps);
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

template <typename T>
void test_all()
{
    aligned_vector<T> v(10, T(1));
    ASSERT_TRUE(fastmath::all(v.data(), v.size()));

    v[0] = T(0);
    ASSERT_FALSE(fastmath::all(v.data(), v.size()));
}
template <typename T>
void test_any()
{
    aligned_vector<T> v(10, T(0));
    ASSERT_FALSE(fastmath::any(v.data(), v.size()));

    v[0] = T(1);
    ASSERT_TRUE(fastmath::any(v.data(), v.size()));
}
template <typename T>
void test_allFinite()
{
    aligned_vector<T> v(10, T(0));
    ASSERT_TRUE(fastmath::allFinite(v.data(), v.size()));

    v[0] = std::numeric_limits<T>::infinity();
    ASSERT_FALSE(fastmath::allFinite(v.data(), v.size()));
}
template <typename T>
void test_hasNaN()
{
    aligned_vector<T> v(10, T(0));
    ASSERT_FALSE(fastmath::hasNaN(v.data(), v.size()));

    v[0] = std::log(T(-1.0));
    ASSERT_TRUE(fastmath::hasNaN(v.data(), v.size()));
}
template <typename T>
void test_abs()
{
    aligned_vector<T> v({T(-1), T(-2), T(-3)});
    fastmath::abs(v.data(), v.data(), v.size());
    aligned_vector<T> expected({T(1), T(2), T(3)});
    ASSERT_EQ(expected, v);
}
template <typename T>
void test_abs2()
{
    aligned_vector<T> v({T(-1), T(-2), T(-3)});
    fastmath::abs2(v.data(), v.data(), v.size());
    aligned_vector<T> expected({T(1), T(4), T(9)});
    ASSERT_EQ(expected, v);
}
template <typename T>
void test_normalize()
{
    aligned_vector<T> v({T(-1), T(-2), T(-3)});
    fastmath::normalize(v.data(), v.size());
    T sum = std::sqrt(14);
    aligned_vector<T> expected({T(-1.0 / sum), T(-2.0f / sum), T(-3.0 / sum)});
    ASSERT_EQ(expected, v);
}
template <typename T>
void test_cos()
{
    auto k = T(M_PI);
    aligned_vector<T> v({T(k), T(2 * k), T(3 * k)});
    fastmath::cos(v.data(), v.data(), v.size());
    aligned_vector<T> expected({T(-1), T(1), T(-1)});
    assert_near(expected, v, 1e-6);
}
template <typename T>
void test_sin()
{
    auto k = T(M_PI / 2.0);
    aligned_vector<T> v({T(k), T(2 * k), T(3 * k)});
    fastmath::sin(v.data(), v.data(), v.size());
    aligned_vector<T> expected({T(1), T(0), T(-1)});
    assert_near(expected, v, 1e-6);
}
template <typename T>
void test_tan()
{
    auto k = T(M_PI / 4.0);
    aligned_vector<T> v({T(k), T(3 * k), T(5 * k)});
    fastmath::tan(v.data(), v.data(), v.size());
    aligned_vector<T> expected({T(1), T(-1), T(1)});
    assert_near(expected, v, 1e-6);
}
template <typename T>
void test_acos()
{
    auto k = T(M_PI);
    aligned_vector<T> v({T(-1), T(1), T(-1)});
    fastmath::acos(v.data(), v.data(), v.size());
    aligned_vector<T> expected({T(k), T(0), T(k)});
    assert_near(expected, v, 1e-6);
}
template <typename T>
void test_asin()
{
    auto k = T(M_PI / 2.0);
    aligned_vector<T> v({T(1), T(0), T(-1)});
    fastmath::asin(v.data(), v.data(), v.size());
    aligned_vector<T> expected({T(k), T(0), T(-k)});
    assert_near(expected, v, 1e-6);
}
template <typename T>
void test_max()
{
    const size_t size = 10;
    aligned_vector<T> v(size, 0);
    for (size_t i = 0; i < size; ++i)
    {
        v[i] = i;
    }
    ASSERT_EQ(T(9), fastmath::max(v.data(), v.size()));
}
template <typename T>
void test_min()
{
    const size_t size = 10;
    aligned_vector<T> v(size, 0);
    for (size_t i = 0; i < size; ++i)
    {
        v[i] = i;
    }
    ASSERT_EQ(T(0), fastmath::min(v.data(), v.size()));
}
template <typename T>
void test_sum()
{
    const size_t size = 10;
    T sum = 0;
    aligned_vector<T> v(size, 0);
    for (size_t i = 0; i < size; ++i)
    {
        sum += i;
        v[i] = i;
    }
    ASSERT_EQ(T(sum), fastmath::sum(v.data(), v.size()));
}
template <typename T>
void test_prod()
{
    const size_t size = 10;
    T prod = 1;
    aligned_vector<T> v(size, 0);
    for (size_t i = 0; i < size; ++i)
    {
        v[i] = i + 1;
        prod *= i + 1;
    }
    ASSERT_EQ(T(prod), fastmath::prod(v.data(), v.size()));
}
template <typename T>
void test_mean()
{
    const size_t size = 10;
    T sum = 0;
    aligned_vector<T> v(size, 0);
    for (size_t i = 0; i < size; ++i)
    {
        sum += i;
        v[i] = i;
    }
    ASSERT_EQ(T(sum) / T(size), fastmath::mean(v.data(), v.size()));
}
template <typename T>
void test_fill()
{
    const size_t size = 10;
    T value = 3;
    aligned_vector<T> v(size, 0);
    aligned_vector<T> expected(size, value);
    fastmath::fill(value, v.data(), v.size());
    ASSERT_EQ(expected, v);
}
template <typename T>
void test_linspace()
{
    const size_t size = 10;
    aligned_vector<T> v(size, 0);
    aligned_vector<T> expected(size, 0);
    T delta = std::is_integral<T>::value ? 0 : T(1) / T(size - 1);
    for (size_t i = 0; i < size; ++i)
    {
        expected[i] = i + i * delta;
    }
    fastmath::linspace(T(0), T(10), v.data(), v.size());
    assert_near(expected, v, 1e-6);
}
template <typename T>
void test_norm()
{
    aligned_vector<T> v({T(1), T(2), T(3)});
    float result = 0;
    fastmath::norm(&result, v.data(), v.size());
    ASSERT_FLOAT_EQ(sqrt(14.), result);
}
template <typename T>
void test_squaredNorm()
{
    aligned_vector<T> v({T(1), T(2), T(3)});
    ASSERT_FLOAT_EQ(14., fastmath::squaredNorm(v.data(), v.size()));
}
template <typename T>
void test_square()
{
    aligned_vector<T> v({T(-1), T(-2), T(-3)});
    fastmath::square(v.data(), v.data(), v.size());
    aligned_vector<T> expected({T(1), T(4), T(9)});
    ASSERT_EQ(expected, v);
}
template <typename T>
void test_cube()
{
    aligned_vector<T> v({T(-1), T(-2), T(-3)});
    fastmath::cube(v.data(), v.data(), v.size());
    aligned_vector<T> expected({T(-1), T(-8), T(-27)});
    ASSERT_EQ(expected, v);
}
template <typename T>
void test_pow()
{
    aligned_vector<T> v({T(-1), T(-2), T(-3)});
    fastmath::pow(T(3), v.data(), v.data(), v.size());
    aligned_vector<T> expected({T(-1), T(-8), T(-27)});
    ASSERT_EQ(expected, v);
}
template <typename T>
void test_sqrt()
{
    aligned_vector<T> v({T(1), T(4), T(9)});
    fastmath::sqrt(v.data(), v.data(), v.size());
    aligned_vector<T> expected({T(1), T(2), T(3)});
    ASSERT_EQ(expected, v);
}
template <typename T>
void test_exp()
{
    double e = M_E;
    aligned_vector<T> v({T(1), T(2), T(3)});
    fastmath::exp(v.data(), v.data(), v.size());
    aligned_vector<T> expected({T(e), T(e * e), T(e * e * e)});
    assert_near(expected, v, 1e-6);
}
template <typename T>
void test_log()
{
    double e = M_E;
    aligned_vector<T> v({T(e), T(e * e), T(e * e * e)});
    fastmath::log(v.data(), v.data(), v.size());
    aligned_vector<T> expected({T(1), T(2), T(3)});
    assert_near(expected, v, 1e-6);
}
template <typename T>
void test_inverse()
{
    aligned_vector<T> v({T(1), T(2), T(3)});
    fastmath::inverse(v.data(), v.data(), v.size());
    aligned_vector<T> expected({T(1), T(1.0 / 2.0), T(1.0 / 3.0)});
    assert_near(expected, v, 1e-6);
}
template <typename T>
void test_conjugate()
{
    using std::complex;
    const complex<T> a(1, 2);
    const complex<T> b(3, 4);
    const complex<T> c(5, 6);
    const complex<T> ac(1, -2);
    const complex<T> bc(3, -4);
    const complex<T> cc(5, -6);
    aligned_vector<complex<T>> v({a, b, c});
    fastmath::conjugate(v.data(), v.data(), v.size());
    aligned_vector<complex<T>> expected({ac, bc, cc});
    assert_near(expected, v, 1e-6);
}
template <typename T>
void test_add()
{
    aligned_vector<T> x({T(1), T(2), T(3)});
    aligned_vector<T> y({T(4), T(5), T(6)});
    fastmath::add(x.data(), x.data(), y.data(), x.size());
    aligned_vector<T> expected({T(5), T(7), T(9)});
    assert_near(expected, x, 1e-6);
}
template <typename T>
void test_sub()
{
    aligned_vector<T> x({T(4), T(15), T(26)});
    aligned_vector<T> y({T(1), T(2), T(3)});
    fastmath::sub(x.data(), x.data(), y.data(), x.size());
    aligned_vector<T> expected({T(3), T(13), T(23)});
    assert_near(expected, x, 1e-6);
}
template <typename T>
void test_mul()
{
    aligned_vector<T> x({T(1), T(2), T(3)});
    aligned_vector<T> y({T(4), T(5), T(6)});
    fastmath::mul(x.data(), x.data(), y.data(), x.size());
    aligned_vector<T> expected({T(4), T(10), T(18)});
    assert_near(expected, x, 1e-6);
}
template <typename T>
void test_div()
{
    aligned_vector<T> x({T(10), T(20), T(30)});
    aligned_vector<T> y({T(2), T(5), T(6)});
    fastmath::div(x.data(), x.data(), y.data(), x.size());
    aligned_vector<T> expected({T(5), T(4), T(5)});
    assert_near(expected, x, 1e-6);
}

TEST(ArrayOpsTest, all)
{
    test_all<int>();
    test_all<float>();
    test_all<size_t>();
    test_all<double>();
}
TEST(ArrayOpsTest, any)
{
    test_any<int>();
    test_any<float>();
    test_any<size_t>();
    test_any<double>();
}
TEST(ArrayOpsTest, allFinite)
{
    test_allFinite<float>();
    test_allFinite<double>();
}
TEST(ArrayOpsTest, hasNaN)
{
    test_hasNaN<float>();
    test_hasNaN<double>();
}
TEST(ArrayOpsTest, abs)
{
    test_abs<int>();
    test_abs<float>();
    test_abs<double>();
}
TEST(ArrayOpsTest, abs2)
{
    test_abs2<int>();
    test_abs2<float>();
    test_abs2<double>();
}
TEST(ArrayOpsTest, normalize)
{
    test_normalize<int>();
    test_normalize<float>();
    test_normalize<double>();
}
TEST(ArrayOpsTest, cos)
{
    test_cos<float>();
    test_cos<double>();
}
TEST(ArrayOpsTest, sin)
{
    test_sin<float>();
    test_sin<double>();
}
TEST(ArrayOpsTest, tan)
{
    test_tan<float>();
    test_tan<double>();
}
TEST(ArrayOpsTest, acos)
{
    test_acos<float>();
    test_acos<double>();
}
TEST(ArrayOpsTest, asin)
{
    test_asin<float>();
    test_asin<double>();
}
TEST(ArrayOpsTest, max)
{
    test_max<int>();
    test_max<size_t>();
    test_max<float>();
    test_max<double>();
}
TEST(ArrayOpsTest, min)
{
    test_min<int>();
    test_min<size_t>();
    test_min<float>();
    test_min<double>();
}
TEST(ArrayOpsTest, sum)
{
    test_sum<int>();
    test_sum<size_t>();
    test_sum<float>();
    test_sum<double>();
}
TEST(ArrayOpsTest, prod)
{
    test_prod<int>();
    test_prod<size_t>();
    test_prod<float>();
    test_prod<double>();
}
TEST(ArrayOpsTest, mean)
{
    test_mean<int>();
    test_mean<size_t>();
    test_mean<float>();
    test_mean<double>();
}
TEST(ArrayOpsTest, fill)
{
    test_fill<int>();
    test_fill<size_t>();
    test_fill<float>();
    test_fill<double>();
}
TEST(ArrayOpsTest, linspace)
{
    test_linspace<int>();
    test_linspace<size_t>();
    test_linspace<float>();
    test_linspace<double>();
}
TEST(ArrayOpsTest, norm)
{
    test_norm<float>();
    test_norm<double>();
}
TEST(ArrayOpsTest, squaredNorm)
{
    test_squaredNorm<float>();
    test_squaredNorm<double>();
}
TEST(ArrayOpsTest, square)
{
    test_square<int>();
    test_square<size_t>();
    test_square<float>();
    test_square<double>();
}
TEST(ArrayOpsTest, cube)
{
    test_cube<int>();
    test_cube<size_t>();
    test_cube<float>();
    test_cube<double>();
}
TEST(ArrayOpsTest, pow)
{
    test_pow<int>();
    test_pow<size_t>();
    test_pow<float>();
    test_pow<double>();
}
TEST(ArrayOpsTest, sqrt)
{
    test_sqrt<int>();
    test_sqrt<size_t>();
    test_sqrt<float>();
    test_sqrt<double>();
}
TEST(ArrayOpsTest, exp)
{
    test_exp<int>();
    test_exp<size_t>();
    test_exp<float>();
    test_exp<double>();
}
TEST(ArrayOpsTest, log)
{
    test_log<float>();
    test_log<double>();
}
TEST(ArrayOpsTest, inverse)
{
    test_inverse<float>();
    test_inverse<double>();
}
TEST(ArrayOpsTest, conjugate)
{
    test_conjugate<int>();
    test_conjugate<size_t>();
    test_conjugate<float>();
    test_conjugate<double>();
}
TEST(ArrayOpsTest, add)
{
    test_add<int>();
    test_add<size_t>();
    test_add<float>();
    test_add<double>();
}
TEST(ArrayOpsTest, sub)
{
    test_sub<int>();
    test_sub<size_t>();
    test_sub<float>();
    test_sub<double>();
}
TEST(ArrayOpsTest, mul)
{
    test_mul<int>();
    test_mul<size_t>();
    test_mul<float>();
    test_mul<double>();
}
TEST(ArrayOpsTest, div)
{
    test_div<int>();
    test_div<size_t>();
    test_div<float>();
    test_div<double>();
}
