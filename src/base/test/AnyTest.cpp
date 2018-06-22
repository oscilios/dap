#include "dap_gtest.h"
#include <vector>
#include <iostream>

#include "base/Any.h"

using namespace testing;
using namespace dap;

enum class MyEnum
{
    A = 0,
    B = 1,
    C = 2
};
inline std::ostream& operator<<(std::ostream& out, MyEnum e)
{
    switch (e)
    {
        case MyEnum::A:
            out << "A";
            return out;
        case MyEnum::B:
            out << "B";
            return out;
        case MyEnum::C:
            out << "C";
            return out;
    }
    return out;
}

class AnyTest : public Test
{
};

DAP_TEST_F(AnyTest, integer)
{
    Any a(10);
    // std::cout << a.to<int>() << std::endl;
    DAP_ASSERT_EQ(10, a.to<int>());
}
DAP_TEST_F(AnyTest, float)
{
    Any a(0.7f);
    // std::cout << a.to<float>() << std::endl;
    DAP_ASSERT_EQ(0.7f, a.to<float>());
}
DAP_TEST_F(AnyTest, ptr)
{
    int x = 42;
    // int* xptr = &x;
    Any a(&x);
    // std::cout << a.to<float>() << std::endl;
    DAP_ASSERT_EQ(42, *a.to<int*>());
    x = 43;
    DAP_ASSERT_EQ(43, *a.to<int*>());
}
DAP_TEST_F(AnyTest, assignment)
{
    Any a(10);
    DAP_ASSERT_EQ(10, a.to<int>());
    a = 5;
    DAP_ASSERT_EQ(5, a.to<int>());
    DAP_ASSERT_EQ(std::string("5"), a.toString());
}
DAP_TEST_F(AnyTest, assignmentWithAny)
{
    Any a(10);
    DAP_ASSERT_EQ(10, a.to<int>());
    Any b(5);
    a = b;
    DAP_ASSERT_EQ(5, a.to<int>());
    DAP_ASSERT_EQ(5, b.to<int>());
}
DAP_TEST_F(AnyTest, equality)
{
    Any a(10);
    ASSERT_TRUE(a == 10);
    // ASSERT_TRUE(10 == a);
}
DAP_TEST_F(AnyTest, typeCheck)
{
    Any a(10);
    try
    {
        // cannot assign as creation type was an int
        a = 1.5f;
        ASSERT_TRUE(false);
    }
    catch (const std::bad_typeid& e)
    {
        ASSERT_TRUE(true);
    }
}
DAP_TEST_F(AnyTest, vectorOfAnys)
{
    std::vector<Any> v({Any(1), Any(2), Any(3), Any(4)});
    // for (auto& x : v) std::cout << (int)x << ", ";
    // std::cout << std::endl;
    DAP_ASSERT_EQ(1, v[0].to<int>());
    DAP_ASSERT_EQ(2, v[1].to<int>());
    DAP_ASSERT_EQ(3, v[2].to<int>());
}
DAP_TEST_F(AnyTest, vectorOfAnysOfDifferentType)
{
    std::vector<Any> v({Any(1), Any(2.2f), Any(std::string("hello"))});
    // for (auto& x : v) std::cout << (int)x << ", ";
    // std::cout << std::endl;
    DAP_ASSERT_EQ(1, v[0].to<int>());
    DAP_ASSERT_EQ(2.2f, v[1].to<float>());
    DAP_ASSERT_EQ("hello", v[2].to<std::string>());
}
DAP_TEST_F(AnyTest, vectorRef)
{
    Any vec(std::vector<float>({0.1f, 0.2f, 0.3f}));
    const std::vector<float>& vref = vec.to<std::vector<float>>();
    // for (const auto& x : vecref) std::cout << x << ", ";
    // std::cout << std::endl;
    DAP_ASSERT_EQ(0.1f, vref[0]);
    DAP_ASSERT_EQ(0.2f, vref[1]);
    DAP_ASSERT_EQ(0.3f, vref[2]);
    DAP_ASSERT_EQ(std::string("(3) [ 0.1 0.2 0.3 ]"), vec.toString());
}
DAP_TEST_F(AnyTest, vectorGrowth)
{
    Any vec(std::vector<float>({0.1f, 0.2f, 0.3f}));
    vec                         = std::vector<float>({0.5, 0.6, 0.7, 0.8});
    const std::vector<float>& v = vec.to<std::vector<float>>();
    // for (const auto& x : v.to<std::vector<float>>())
    //    std::cout << (float)x << ", ";
    // std::cout << std::endl;
    DAP_ASSERT_EQ(0.5f, v[0]);
    DAP_ASSERT_EQ(0.6f, v[1]);
    DAP_ASSERT_EQ(0.7f, v[2]);
    DAP_ASSERT_EQ(0.8f, v[3]);
}
DAP_TEST_F(AnyTest, assignToVectorInt)
{
    std::vector<int> vec({1, 2, 3, 4});
    Any v(std::move(vec));
    std::vector<int> vec2 = v.to<std::vector<int>>();
    DAP_ASSERT_EQ(1, vec2[0]);
    DAP_ASSERT_EQ(2, vec2[1]);
    DAP_ASSERT_EQ(3, vec2[2]);
    DAP_ASSERT_EQ(4, vec2[3]);
}
DAP_TEST_F(AnyTest, assignToVectorAny)
{
    // TODO (eaylon)
    std::vector<int> vec({1, 2, 3, 4});

    std::vector<Any> v(vec.begin(), vec.end());

    // std::vector<Any> v(vec.size(), Any((std::decay<decltype(vec[0])>::type)(0))); // works but
    // all reference same object
    // for (int i=0; i<vec.size(); i++)
    //    v[i] = vec[i];

    DAP_ASSERT_EQ(1, v[0].to<int>());
    DAP_ASSERT_EQ(2, v[1].to<int>());
    DAP_ASSERT_EQ(3, v[2].to<int>());
    DAP_ASSERT_EQ(4, v[3].to<int>());
}
DAP_TEST_F(AnyTest, enums)
{
    Any a(MyEnum::B);
    std::cout << a.type_info().name() << std::endl;
    DAP_ASSERT_EQ(MyEnum::B, a.to<MyEnum>());
}

DAP_TEST_F(AnyTest, moveAssignment)
{
    std::string s("42");
    Any x(std::move(s));
    DAP_ASSERT_EQ("42", x.to<std::string>());
}
