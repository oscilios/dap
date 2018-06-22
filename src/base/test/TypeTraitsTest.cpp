#include "dap_gtest.h"
#include <vector>
#include <iostream>

#include "base/TypeTraits.h"
#include "base/KeyValueTuple.h"
#include "base/Constants.h"
#include "base/VariadicOps.h"
#include <cmath>

using namespace testing;
using namespace dap;

class TypeTraitsTest : public Test
{
};

constexpr int int_generator(std::size_t x, std::size_t /*size*/)
{
    return x;
}

constexpr float float_generator(std::size_t x, std::size_t size)
{
    return float(x) / float(size);
}

constexpr float cube_generator(std::size_t x, std::size_t /*size*/)
{
    return power(x, 3);
}

DAP_TEST_F(TypeTraitsTest, size_of_array)
{
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    DAP_ASSERT_EQ(10u, array_size(a));
}
DAP_TEST_F(TypeTraitsTest, integer_sequence)
{
    auto seq = make_array<10>(int_generator);
    DAP_ASSERT_EQ(10u, seq.size());
    int i = 0;
    for (auto x : seq)
    {
        DAP_ASSERT_EQ(i++, x);
    }
}
DAP_TEST_F(TypeTraitsTest, float_sequence)
{

    auto seq = make_array<10>(float_generator);
    DAP_ASSERT_EQ(10u, seq.size());
    float i     = 0;
    float delta = 0.1f;
    for (auto x : seq)
    {
        DAP_ASSERT_FLOAT_EQ(i, x);
        i += delta;
    }
}
DAP_TEST_F(TypeTraitsTest, irange)
{
    auto seq = irange<int, 10, 20>();
    DAP_ASSERT_EQ(10u, seq.size());
    int i = 10;
    for (auto x : seq)
    {
        DAP_ASSERT_EQ(i++, x);
    }

    auto empty_seq = irange<int, 10, 10>();
    DAP_ASSERT_EQ(0u, empty_seq.size());
}
DAP_TEST_F(TypeTraitsTest, frange)
{
    size_t size = 10;
    auto seq = frange<float, 10>();
    DAP_ASSERT_EQ(size, seq.size());
    int i = 0;
    for (auto x : seq)
    {
        DAP_ASSERT_FLOAT_EQ(float(i) / float(size), x);
        i++;
    }
}
DAP_TEST_F(TypeTraitsTest, linspace)
{
    size_t size = 10;
    float start = 10.0f;
    float end   = 15.0f;
    auto seq = linspace<10>(start, end);
    DAP_ASSERT_EQ(size, seq.size());
    int i       = 0;
    float delta = (end - start) / float(size - 1);
    for (auto x : seq)
    {
        DAP_ASSERT_FLOAT_EQ(start + i * delta, x);
        i++;
    }
}
DAP_TEST_F(TypeTraitsTest, power_sequence)
{
    auto seq = make_array<10>(cube_generator);
    DAP_ASSERT_EQ(10u, seq.size());
    int i = 0;
    for (auto x : seq)
    {
        DAP_ASSERT_EQ(i * i * i, x);
        ++i;
    }
}
DAP_TEST_F(TypeTraitsTest, tuple_type_index)
{
    using t = std::tuple<int, float, char>;
    {
        auto index = Index<int, t>::value;
        DAP_ASSERT_EQ(0u, index);
    }
    {
        auto index = Index<float, t>::value;
        DAP_ASSERT_EQ(1u, index);
    }
    {
        auto index = Index<char, t>::value;
        DAP_ASSERT_EQ(2u, index);
    }

    using ts = decltype(std::make_tuple("hello"_s, "world"_s, "foo"_s, "bar"_s));
    {
        auto index = Index<decltype("hello"_s), ts>::value;
        DAP_ASSERT_EQ(0u, index);
    }
    {
        auto index = Index<decltype("world"_s), ts>::value;
        DAP_ASSERT_EQ(1u, index);
    }
    {
        auto index = Index<decltype("foo"_s), ts>::value;
        DAP_ASSERT_EQ(2u, index);
    }
    {
        auto index = Index<decltype("bar"_s), ts>::value;
        DAP_ASSERT_EQ(3u, index);
    }
}
DAP_TEST_F(TypeTraitsTest, tuple_contains_type)
{
    auto t = make_tuple("int"_s, "float"_s, "char"_s);
    DAP_ASSERT_TRUE(tupleContainsType("int"_s, t));
    DAP_ASSERT_TRUE(tupleContainsType("float"_s, t));
    DAP_ASSERT_TRUE(tupleContainsType("char"_s, t));
    DAP_ASSERT_FALSE(tupleContainsType("double"_s, t));
}
DAP_TEST_F(TypeTraitsTest, KeyValueTuple)
{
    auto t = make_key_value_tuple(make_tuple("int"_s, 10), make_tuple("float"_s, 20.0));
    DAP_ASSERT_EQ(10, std::get<0>(t.values));
    DAP_ASSERT_EQ(20.0, std::get<1>(t.values));
    DAP_ASSERT_EQ("int", to_string(std::get<0>(decltype(t)::keys{})));
    DAP_ASSERT_EQ("float", to_string(std::get<1>(decltype(t)::keys{})));
    DAP_ASSERT_EQ(2u, std::tuple_size<decltype(t)::keys>::value);
    DAP_ASSERT_EQ(10, t["int"_s]);
    t["int"_s] = 42;
    DAP_ASSERT_EQ(42, t["int"_s]);
}
DAP_TEST_F(TypeTraitsTest, VariadicOps)
{
    DAP_ASSERT_EQ(45, dap::sum(1,2,3,4,5,6,7,8,9));
    DAP_ASSERT_EQ(factorial(9), dap::prod(1,2,3,4,5,6,7,8,9));
    DAP_ASSERT_EQ(std::sqrt(285), dap::rms(1,2,3,4,5,6,7,8,9));
}
DAP_TEST_F(TypeTraitsTest, constexpr_string)
{
    DAP_ASSERT_EQ(std::string("128"), to_string(num_constexpr_string<128>::value));
    DAP_ASSERT_EQ(std::string("bus_512"), to_string("bus_"_s + num_constexpr_string<512>::value));
    DAP_ASSERT_EQ(std::string("bus_0"), to_string("bus_"_s + num_constexpr_string<0>::value));
}
