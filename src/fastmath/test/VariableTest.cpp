#include "dap_gtest.h"
#include <vector>
#include <iostream>
#include <limits>
#include <typeindex>

#include "fastmath/Var.h"
#include "fastmath/TypeTraits.h"

using namespace testing;
using namespace dap;
using namespace dap::fastmath;

class VariableTest : public Test
{
};

template <typename T>
auto compute_bench(const T& x, const T& y)
{
    return (log(2 * x + y + x * y + pow(x * y, y)));
}

template <class SrcContainer, class DestContainer, class Func>
void eval_expr(const SrcContainer& src, DestContainer& dest, Func&& func)
{
    assert(src.size() == dest.size());
    auto destIt = dest.begin();
    auto srcIt = src.begin();
    while (srcIt != src.end())
    {
        *destIt = func(*srcIt);
        ++destIt;
        ++srcIt;
    }
}

DAP_TEST_F(VariableTest, simple)
{
    var n = 42;
    DAP_ASSERT_EQ(42, int(n));

    n = 43;
    DAP_ASSERT_EQ(43, n);

    n = 4.2f;
    DAP_ASSERT_FLOAT_EQ(4.2f, n);

    int m = 44;
    n = m;
    DAP_ASSERT_EQ(m, n);
    DAP_ASSERT_EQ(44, n);
    DAP_ASSERT_EQ(44, static_cast<int>(n));

    // operator =

    var n2 = 55;

    // copy assignment operator
    n = n2;
    DAP_ASSERT_EQ(55, n);

    // plus operator
    n = n + var(1);
    DAP_ASSERT_EQ(56, n);

    n = n + var(1.5);
    DAP_ASSERT_FLOAT_EQ(57.5f, n);

    // assign to a float
    const float foo = n;
    DAP_ASSERT_FLOAT_EQ(57.5f, foo);

    // overflow
    n       = std::numeric_limits<int>::max();
    float f = std::numeric_limits<int>::max();
    n = n + var(1);
    DAP_ASSERT_EQ(f + 1.0f, n);

    n = 3.0f;
    n2 = 200.0f;
    DAP_ASSERT_EQ(0.015f, n/n2);
}

DAP_TEST_F(VariableTest, literals_and_Variables_combined)
{
    var n = 42;
    n = n + 1 + var(4) + 2 * n;
    std::cout << exprTypeStr(n + 1 + var(4) + 2 * n) << std::endl;
    DAP_ASSERT_EQ(131, n);
}

DAP_TEST_F(VariableTest, container)
{
    std::vector<var> v({1, 2, 3, 4, 5});
    int i = 1;
    for (const auto& x : v)
    {
        DAP_ASSERT_EQ(i++, int(x));
    }
}

DAP_TEST_F(VariableTest, expression)
{
    {
        var x = 2;
        var y = 3.0f;
        var z = 5.0;
        var w = x * y + x / z - y / z;
        std::cout << exprTypeStr(x * y + x / z - y / z) << std::endl;
        DAP_ASSERT_FLOAT_EQ(5.8f, w);
    }

    {
        var x;
        auto expr = x * x + x * x;
        DAP_ASSERT_EQ(18, expr(3));
        DAP_ASSERT_FLOAT_EQ(std::sin(18), sin(expr(3)));
    }

}

DAP_TEST_F(VariableTest, containerFunctor)
{
    {
        std::vector<var> v({1, 2, 3, 4, 5});
        var x; // NOLINT

        eval_expr(v, v, x * x + x * x);
        std::cout << exprTypeStr(x * x + x * x) << std::endl;

        int i = 1;
        for (const auto& k : v)
        {
            DAP_ASSERT_FLOAT_EQ(2 * i * i, int(k));
            ++i;
        }
    }

    {
        std::vector<var> v({1, 2, 3, 4, 5});
        var x; // NOLINT
        auto sqr = x * x + x * x;
        std::for_each(v.begin(), v.end(), [&sqr](var& val) { val = sin(sqr(val));} );

        //for (auto& val : v)
        //    val = sin(sqr(val));

        int i = 1;
        for (const auto& k : v)
        {
            DAP_ASSERT_FLOAT_EQ(std::sin(2 * i * i), k);
            ++i;
        }
    }
}
struct MyOp
{
    template <typename X, typename Y>
    auto operator()(X x, Y y) const noexcept
    {
        return pow(x, y + x) + 5;
    }
};
DAP_TEST_F(VariableTest, isArithmetic)
{
    ASSERT_TRUE(fastmath::isArithmetic<var>());
    using MulOpFloat = BinaryExpression<MulOp, var, var>;
    using PowOpFloat = UnaryExpression<PowOp, var>;
    ASSERT_TRUE(fastmath::isBaseExpression<var>());
    ASSERT_TRUE(fastmath::isBaseExpression<MulOpFloat>());
    ASSERT_TRUE(fastmath::isBaseExpression<PowOpFloat>());
}
DAP_TEST_F(VariableTest, power)
{
    var x = 3;
    var y = 2;
    var z = pow(x, y + x) + var(5);
    DAP_ASSERT_FLOAT_EQ(248.0f, z);
    z = pow(x, y) + 5;
    DAP_ASSERT_FLOAT_EQ(14.0f, z);
    z = 3 + pow(pow(x, y), 2);
    DAP_ASSERT_FLOAT_EQ(84.0f, z);
}
DAP_TEST_F(VariableTest, exp)
{
    var x = 3;
    var y = 2;
    var z = exp(2 * x + y);
    std::cout << exprTypeStr(exp(2 * x + y)) << std::endl;
    DAP_ASSERT_FLOAT_EQ(std::exp(8), z);
}
DAP_TEST_F(VariableTest, hypot)
{
    var x = 3;
    var y = 2.5;
    var z = hypot(x, y);
    std::cout << exprTypeStr(hypot(x, y)) << std::endl;
    DAP_ASSERT_FLOAT_EQ(3.905124837953327, z);
}
DAP_TEST_F(VariableTest, compare)
{
    var x = 3;
    var y = 2.5;

    DAP_ASSERT_TRUE(x > y);
    DAP_ASSERT_FALSE(x < y);

    DAP_ASSERT_TRUE(x * x >= y * y);
    DAP_ASSERT_FALSE(x * x <= y * y);

    DAP_ASSERT_TRUE(x * y == y * x);
    DAP_ASSERT_TRUE(x * y + 1 != y * x);
}
DAP_TEST_F(VariableTest, maxmin)
{
    var x = 3;
    var y = 2.5;

    DAP_ASSERT_FLOAT_EQ(3, max(x, y));
    DAP_ASSERT_FLOAT_EQ(2.5, min(x, y));
}
DAP_TEST_F(VariableTest, plus_equal_operator)
{
    var x = 3;
    var y = 2.5;
    var z = 1.0;

    z += x * y;
    DAP_ASSERT_FLOAT_EQ(8.5, z);

    z += 1;
    DAP_ASSERT_FLOAT_EQ(9.5, z);

    z *= pow(x, 2);
    DAP_ASSERT_FLOAT_EQ(85.5, z);

    z -= 85;
    DAP_ASSERT_FLOAT_EQ(0.5, z);

    z /= 2 * y;
    DAP_ASSERT_FLOAT_EQ(0.1, z);
}
DAP_TEST_F(VariableTest, fastmath_array)
{
    var_array a({1, 2, 3, 4});
    auto f = a.vec().dot(a.vec());
    DAP_ASSERT_FLOAT_EQ(30, f);

    var_array v({5, 6});
    var_array res = a.mat<2, 2>() * v.vec();
    DAP_ASSERT_FLOAT_EQ(23, res[0]);
    DAP_ASSERT_FLOAT_EQ(34, res[1]);
}
#ifdef NDEBUG
DAP_TEST_F(VariableTest, benchmark)
{
    // using normal arithmetic
    const float x1 = 3.0f;
    const float y1 = 2.0f;
    float z1       = 0.0f;
    size_t count   = 0;
    auto start = std::chrono::steady_clock::now();
    while (count++ < 1000000)
    {
        z1 = compute_bench(x1, y1);
    }
    auto end                            = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff1 = end - start;

    // using Variable and expresssions
    const var x = 3;
    const var y = 2;
    var z       = 0;
    count = 0;
    std::cout << exprTypeStr(2 * x + y + x * y + pow(x, y) + pow(x * y, y)) << std::endl;
    start = std::chrono::steady_clock::now();
    while (count++ < 1000000)
    {
        z = compute_bench(x, y);
    }
    end                                = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;

    // back to using normal arithmetic
    count = 0;
    start = std::chrono::steady_clock::now();
    while (count++ < 1000000)
    {
        z1 = compute_bench(x1, y1);
    }
    end                                 = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff2 = end - start;

    std::cout << "diff: " << diff.count() << " " << diff1.count() << " "
              << diff.count() / diff1.count() << std::endl;
    std::cout << "diff: " << diff1.count() << " " << diff2.count() << " "
              << diff1.count() / diff2.count() << std::endl;

    DAP_ASSERT_LE(diff.count() / diff1.count(), 1.5);

    DAP_ASSERT_FLOAT_EQ(std::log(50), z);
    DAP_ASSERT_FLOAT_EQ(std::log(50), z1);
}
DAP_TEST_F(VariableTest, benchmark_fastmath_array)
{
    // int
    size_t size = 32;
    Array<float> a1(size);
    var_array a2(size);
    for (size_t i = 0; i < size; ++i)
    {
        a1[i] = i * 0.1f;
        a2[i] = i * 0.1f;
    }

    auto start   = std::chrono::steady_clock::now();
    size_t count = 0;
    auto res1 = 1.0f;
    while (count++ < 1000000)
    {
        res1 += a1.vec().dot(a1.vec());
    }
    auto end                            = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff1 = end - start;

    // var
    start    = std::chrono::steady_clock::now();
    count    = 0;
    var res2 = 1.0f;
    while (count++ < 1000000)
    {
        res2 += a2.vec().dot(a2.vec());
    }
    end                                 = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff2 = end - start;

    // int
    start     = std::chrono::steady_clock::now();
    count     = 0;
    auto res3 = 1.0f;
    while (count++ < 1000000)
    {
        res3 += a1.vec().dot(a1.vec());
    }
    end                                 = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff3 = end - start;

    // manual
    start     = std::chrono::steady_clock::now();
    count     = 0;
    auto res4 = 1.0f;
    while (count++ < 1000000)
    {
        auto dot = 0.0f;
        for (auto x : a1)
        {
            dot += x * x;
        }
        res4 += dot;
    }
    end                                 = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff4 = end - start;

    std::cout << diff1.count() << " " << diff2.count() << " " << diff3.count() << " "
              << diff4.count() << std::endl;
    DAP_ASSERT_FLOAT_EQ(1.0f, res2 / res1);
    DAP_ASSERT_FLOAT_EQ(1.0f, res3 / res1);
    DAP_ASSERT_FLOAT_EQ(1.0f, res4 / res1);

    DAP_ASSERT_LE(diff2.count() / diff1.count(), 1.5);
    DAP_ASSERT_LE(diff2.count() / diff3.count(), 1.5);
    DAP_ASSERT_GE(diff4.count() / diff2.count(), 2.5);
}
#endif // NDEBUG
