#include "dap_gtest.h"
#include <iostream>
#include <vector>

#include "base/AbstractFactory.h"
#include "base/TypeTraits.h"

using namespace testing;
using namespace dap;

namespace dap
{
    namespace test
    {
        class Base
        {
        public:
            Base()            = default;
            Base(const Base&) = default;
            Base(Base&&)      = default; // NOLINT
            Base& operator=(const Base&) = default;
            Base& operator=(Base&&) noexcept = default; // NOLINT
            virtual ~Base()                  = default;
            virtual int ivalue()             = 0;
            virtual float fvalue()           = 0;
        };
        template <typename ProductType, typename... ConstructorArgs>
        class BaseRegistrar : public Registrar<Base, ProductType, ConstructorArgs...>
        {
        };

        class Derived1 : public Base, public BaseRegistrar<Derived1>
        {
        public:
            constexpr static const char* getName() noexcept
            {
                return "Derived1";
            }
            int ivalue() override
            {
                return 1;
            }
            float fvalue() override
            {
                return 0;
            }
        };
        class Derived2 : public Base, public BaseRegistrar<Derived2>
        {
        public:
            constexpr static const char* getName() noexcept
            {
                return "Derived2";
            }
            int ivalue() override
            {
                return 2;
            }
            float fvalue() override
            {
                return 0;
            }
        };
        class Derived3 : public Base, public BaseRegistrar<Derived3>
        {
        public:
            constexpr static const char* getName() noexcept
            {
                return "Derived2"; // same name as Derived2 so one of the two will not be possible to register
            }
            int ivalue() override
            {
                return 2;
            }
            float fvalue() override
            {
                return 0;
            }
        };

        // class taking one parameter
        class DerivedN : public Base, public BaseRegistrar<DerivedN, int>
        {
            int m_value{-1};

        public:
            constexpr static const char* getName() noexcept
            {
                return "DerivedN";
            }
            explicit DerivedN(int n)
            : m_value(n)
            {
            }
            int ivalue() override
            {
                return m_value;
            }
            float fvalue() override
            {
                return 0;
            }
        };

        class DerivedN2 : public Base, public BaseRegistrar<DerivedN2, int, float>
        {
            int m_ivalue{-1};
            float m_fvalue{-1};

        public:
            constexpr static const char* getName() noexcept
            {
                return "DerivedN2";
            }
            DerivedN2(int i, float f)
            : m_ivalue(i)
            , m_fvalue(f)
            {
            }
            int ivalue() override
            {
                return m_ivalue;
            }
            float fvalue() override
            {
                return m_fvalue;
            }
        };

        template <typename T>
        class DerivedNTemplate : public Base, public BaseRegistrar<DerivedNTemplate<T>, T>
        {
            T m_value;

        public:
            constexpr static const char* getName() noexcept
            {
                return "DerivedNTemplate";
            }
            explicit DerivedNTemplate(T x)
            : m_value(x)
            {
            }
            int ivalue() override
            {
                return static_cast<int>(m_value);
            }
            float fvalue() override
            {
                return static_cast<float>(m_value);
            }
        };

        template <typename... Args>
        typename AbstractFactory<test::Base, Args...>::Ptr
        createBaseProduct(const char* name, Args&&... args)
        {
            using TestBaseFactory = AbstractFactory<test::Base, Args&&...>;
            return TestBaseFactory::create(name, std::forward<Args>(args)...);
        }
    }
}

#define DAP_TEST_REGISTER(ProductType)                                 \
    template <>                                                        \
    const volatile bool Registrar<test::Base, ProductType>::value =    \
        AbstractFactory<test::Base>::register_(ProductType::getName(), \
                                               &Registrar<test::Base, ProductType>::create)

#define DAP_TEST_REGISTER_WITH_ARGS(ProductType, ...)                            \
    template <>                                                                  \
    const volatile bool Registrar<test::Base, ProductType, __VA_ARGS__>::value = \
        AbstractFactory<test::Base, __VA_ARGS__>::register_(                     \
            ProductType::getName(), &Registrar<test::Base, ProductType, __VA_ARGS__>::create)

DAP_TEST_REGISTER(dap::test::Derived1);
// DAP_TEST_REGISTER(dap::test::Derived1); // impossible to double register as it yields a compile error
DAP_TEST_REGISTER(dap::test::Derived2);
// DAP_TEST_REGISTER(dap::test::Derived3); // can't be done because it throws runtime error for duplicate key
DAP_TEST_REGISTER_WITH_ARGS(dap::test::DerivedN, int&&);
DAP_TEST_REGISTER_WITH_ARGS(dap::test::DerivedN2, int&&, float&&);
DAP_TEST_REGISTER_WITH_ARGS(dap::test::DerivedNTemplate<double>, double&&);
DAP_TEST_REGISTER_WITH_ARGS(dap::test::DerivedNTemplate<int>, int&&);
DAP_TEST_REGISTER_WITH_ARGS(dap::test::DerivedNTemplate<unsigned int>, unsigned int&&);

class AbstractFactoryTest : public Test
{
};

DAP_TEST_F(AbstractFactoryTest, createDerivedWithNoArguments)
{
    using namespace dap::test;

    auto d1 = createBaseProduct("Derived1");
    DAP_ASSERT_TRUE(d1 != nullptr);
    DAP_ASSERT_EQ(1, d1->ivalue());

    auto d2 = createBaseProduct("Derived2");
    DAP_ASSERT_TRUE(d2 != nullptr);
    DAP_ASSERT_EQ(2, d2->ivalue());

    try
    {
        auto nil = createBaseProduct("nil");
        DAP_ASSERT_TRUE(false);
    }
    catch (const std::exception& e)
    {
        DAP_ASSERT_TRUE(true);
    }
}

DAP_TEST_F(AbstractFactoryTest, createDerivedWithOneArgument)
{
    using namespace dap::test;
    auto dN = createBaseProduct("DerivedN", 3);

    DAP_ASSERT_TRUE(dN != nullptr);
    DAP_ASSERT_EQ(3, dN->ivalue());
}

DAP_TEST_F(AbstractFactoryTest, createDerivedWithTwoArguments)
{
    using namespace dap::test;

    auto dN2 = createBaseProduct("DerivedN2", 42, 4.2f);

    DAP_ASSERT_TRUE(dN2 != nullptr);
    DAP_ASSERT_EQ(42, dN2->ivalue());
    DAP_ASSERT_FLOAT_EQ(4.2f, dN2->fvalue());
}
DAP_TEST_F(AbstractFactoryTest, createDerivedTemplateWithOneArgument)
{
    using namespace dap::test;
    auto withInt    = createBaseProduct("DerivedNTemplate", 42);
    auto withDouble = createBaseProduct("DerivedNTemplate", 4.2);
    auto withUInt   = createBaseProduct("DerivedNTemplate", 43u);

    DAP_ASSERT_TRUE(withDouble != nullptr);
    DAP_ASSERT_EQ(4, withDouble->ivalue());
    DAP_ASSERT_FLOAT_EQ(4.2, withDouble->fvalue());
    DAP_ASSERT_FLOAT_EQ(42, withInt->ivalue());
    DAP_ASSERT_FLOAT_EQ(42.0f, withInt->fvalue());
    DAP_ASSERT_EQ(43, withUInt->ivalue());
    DAP_ASSERT_EQ(43.0f, withUInt->fvalue());
}
DAP_TEST_F(AbstractFactoryTest, dump)
{
    std::cout << "Factory 1:\n" << AbstractFactory<test::Base>() << std::endl;
    std::cout << "Factory 2:\n" << AbstractFactory<test::Base, int&&>() << std::endl;
    std::cout << "Factory 3:\n" << AbstractFactory<test::Base, int&&, float&&>() << std::endl;
    std::cout << "Factory 4:\n" << AbstractFactory<test::Base, double&&>() << std::endl;
    std::cout << "Factory 5:\n" << AbstractFactory<test::Base, unsigned int&&>() << std::endl;
}
