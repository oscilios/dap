#include <gtest/gtest.h>
#include "fastmath/Taylor.h"
#include <cmath>
#include <limits>
#include <vector>

using namespace testing;
using namespace dap;

TEST(TaylorTest, sine_func)
{
    using precision = std::integral_constant<size_t, 31>;
    float x         = 0.75 * M_PI;
    float result = taylor::detail::sine(x, precision::value);
    ASSERT_FLOAT_EQ(std::sin(x), result);

    using sequence_size = std::integral_constant<size_t, 23>;
    constexpr auto arr = taylor::sine_array<double, sequence_size::value, precision::value>();
    ASSERT_FLOAT_EQ(sequence_size::value, arr.size());

    int i    = 0;
    double k = 2.0 * M_PI / double(sequence_size::value);
    for (auto value : arr)
    {
        // std::cout << value << std::endl;
        ASSERT_FLOAT_EQ(std::sin(k * i), value);
        i++;
    }
}

TEST(TaylorTest, cosine_func)
{
    using precision = std::integral_constant<size_t, 31>;
    double x        = M_PI / 3.0f;
    double result = taylor::detail::cosine(x, precision::value);
    ASSERT_FLOAT_EQ(std::cos(x), result);

    using sequence_size = std::integral_constant<size_t, 22>;
    constexpr auto arr = taylor::cosine_array<double, sequence_size::value, precision::value>();
    ASSERT_FLOAT_EQ(sequence_size::value, arr.size());

    int i    = 0;
    double k = 2.0 * M_PI / double(sequence_size::value);
    for (auto value : arr)
    {
        // std::cout << value << std::endl;
        ASSERT_FLOAT_EQ(std::cos(k * i), value);
        i++;
    }
}

TEST(TaylorTest, exp_func)
{
    using precision = std::integral_constant<size_t, 63>;
    double x        = M_PI / 3.0f;
    double result = taylor::detail::exp(x, precision::value);
    ASSERT_FLOAT_EQ(std::exp(x), result);
    // std::cout << std::exp(x) << " vs " <<  result << std::endl;

    using sequence_size = std::integral_constant<size_t, 22>;
    constexpr auto arr = taylor::exp_array<sequence_size::value, precision::value>(-10.0, 10.0);
    ASSERT_FLOAT_EQ(sequence_size::value, arr.size());

    double i = -10;
    double k = 20.0 / double(sequence_size::value);
    for (auto value : arr)
    {
        // std::cout << i << ":" << value << " vs " << std::exp(i) << std::endl;
        ASSERT_FLOAT_EQ(std::exp(i), value);
        i += k;
    }
}
