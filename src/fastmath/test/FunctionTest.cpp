#include <gtest/gtest.h>
#include <vector>
#include <iostream>
#include <limits>
#include <typeindex>

#include "fastmath/Var.h"
#include "fastmath/Function.h"

using namespace testing;
using namespace dap;
using namespace dap::fastmath;

struct MyOp
{
    template <typename X, typename Y>
    auto operator()(X x, Y y) const noexcept
    {
        return pow(x, y + x) + 5;
    }
};
struct MyOp2
{
    template <typename X>
    auto operator()(X x) const noexcept
    {
        return x + 3;
    }
};

TEST(FunctionTest, function_composition)
{
    var x = 3;
    var y = 2;

    Function<MyOp> f;
    Function<MyOp2> f2;
    {
    auto expected = std::pow(3, 5) + 5;
    auto result = f(x,y);
    ASSERT_EQ(expected, result());
    }
    {
    auto expected = (std::pow(3, 5) + 5) + 3;
    auto result = f2(f(x,y));
    ASSERT_EQ(expected, result());
    }
}
