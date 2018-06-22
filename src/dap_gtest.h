#ifndef DAP_GTEST_CLANG_TIDY_NO_WARNINGS
#define DAP_GTEST_CLANG_TIDY_NO_WARNINGS

#include <gtest/gtest.h>
#include <type_traits>

// This is a layer on top of gtest in order to overcome lots of clang-tidy warnings
// It is not complete, so please add new functions as needed

// Helper macro for defining tests.
#define DAP_GTEST_TEST_(test_case_name, test_name, parent_class, parent_id)                     \
    class GTEST_TEST_CLASS_NAME_(test_case_name, test_name)                                     \
    : public parent_class { /* NOLINT */                                                        \
    public:                                                                                     \
        GTEST_TEST_CLASS_NAME_(test_case_name, test_name)() = default;                          \
                                                                                                \
    private:                                                                                    \
        virtual void TestBody();                                                                \
        static ::testing::TestInfo* const test_info_ GTEST_ATTRIBUTE_UNUSED_;                   \
        DAP_GTEST_DISALLOW_COPY_AND_ASSIGN_(GTEST_TEST_CLASS_NAME_(test_case_name, test_name)); \
        static ::testing::TestInfo* initialize_test_info() noexcept                             \
        {                                                                                       \
            try                                                                                 \
            {                                                                                   \
                ::testing::TestInfo* res = ::testing::internal::MakeAndRegisterTestInfo(        \
                    #test_case_name,                                                            \
                    #test_name,                                                                 \
                    NULL,                                                                       \
                    NULL,                                                                       \
                    ::testing::internal::CodeLocation(__FILE__, __LINE__),                      \
                    (parent_id),                                                                \
                    parent_class::SetUpTestCase,                                                \
                    parent_class::TearDownTestCase,                                             \
                    new ::testing::internal::TestFactoryImpl<GTEST_TEST_CLASS_NAME_(            \
                        test_case_name, test_name)>);                                           \
                return res;                                                                     \
            }                                                                                   \
            catch (...)                                                                         \
            {                                                                                   \
                return nullptr;                                                                 \
            }                                                                                   \
        }                                                                                       \
    };                                                                                          \
                                                                                                \
    ::testing::TestInfo* const GTEST_TEST_CLASS_NAME_(test_case_name, test_name)::test_info_ = initialize_test_info(); /*NOLINT*/ \
    void GTEST_TEST_CLASS_NAME_(test_case_name, test_name)::TestBody()

///////////////////////////////////////////////////////////////////////////////

#define DAP_TEST_F(test_fixture, test_name) \
    DAP_GTEST_TEST_(                        \
        test_fixture, test_name, test_fixture, ::testing::internal::GetTypeId<test_fixture>())

#define DAP_GTEST_DISALLOW_COPY_AND_ASSIGN_(type)       \
    type(type const&) = delete;            /* NOLINT */ \
    type(type&&)      = delete;            /* NOLINT */ \
    type& operator=(type const&) = delete; /* NOLINT */ \
    type& operator=(type&&) = delete;      /* NOLINT */ \
    ~type()                 = default;     /* NOLINT */

///////////////////////////////////////////////////////////////////////////////

#define DAP_GTEST_ASSERT_EQ(expected, actual)                                                    \
    ASSERT_PRED_FORMAT2(                                                                         \
        ::testing::internal::EqHelper<std::is_null_pointer<decltype(expected)>::value>::Compare, \
        expected,                                                                                \
        actual)

#define DAP_ASSERT_EQ(val1, val2) DAP_GTEST_ASSERT_EQ(val1, val2)
#define DAP_ASSERT_FLOAT_EQ(expected, actual) ASSERT_FLOAT_EQ(expected, actual)
#define DAP_ASSERT_NEAR(val1, val2, abs_error) ASSERT_NEAR(val1, val2, abs_error)
#define DAP_ASSERT_FALSE(val) ASSERT_FALSE(val)
#define DAP_ASSERT_TRUE(val) ASSERT_TRUE(val)
#define DAP_ASSERT_LE(val1, val2) ASSERT_LE(val1, val2)
#define DAP_ASSERT_GE(val1, val2) ASSERT_GE(val1, val2)

///////////////////////////////////////////////////////////////////////////////

#define DAP_EXPECT_EQ(expected, actual)                                                          \
    EXPECT_PRED_FORMAT2(                                                                         \
        ::testing::internal::EqHelper<std::is_null_pointer<decltype(expected)>::value>::Compare, \
        expected,                                                                                \
        actual)

#endif // DAP_GTEST_CLANG_TIDY_NO_WARNINGS
