#include "dsp/DelayLine.h"
#include "dsp/FeedbackLine.h"
#include "dap_gtest.h"

using namespace testing;
using namespace dap;
using namespace dap::dsp;

class DelayTest : public Test
{
};

DAP_TEST_F(DelayTest, 4samples)
{
    DelayLine<float, 8> delay;
    std::array<float, 20> expected(
        {{0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}});
    for (size_t i = 0; i < 20; i++)
    {
        DAP_ASSERT_FLOAT_EQ(expected[i], delay(i, 4));
    }
}
DAP_TEST_F(DelayTest, 4d5samples)
{
    DelayLine<float, 8> delay;
    std::array<float, 20> expected(
        {{0,    0,    0,    0,    0,    0.5f,  1.5f,  2.5f,  3.5f,  4.5f,
          5.5f, 6.5f, 7.5f, 8.5f, 9.5f, 10.5f, 11.5f, 12.5f, 13.5f, 14.5f}});
    for (size_t i = 0; i < 20; i++)
    {
        DAP_ASSERT_FLOAT_EQ(expected[i], delay(i, 4.5f));
    }
}
DAP_TEST_F(DelayTest, 4samplesWithFeedback)
{
    const float g = 0.5;
    const float g2 = g*g;
    const float k = std::sqrt(1.0f + g2);
    const float k2 = k*k;
    const float k3 = k*k*k;
    FeedbackLine<float, 8> delay;
    std::array<float, 20> expected({{0,
                                     0,
                                     0,
                                     0,
                                     0,
                                     1.0f / k,
                                     2.0f / k,
                                     3.0f / k,
                                     4.0f / k,
                                     5.0f / k,
                                     6.0f / k + g * 1.0f / k2,
                                     7.0f / k + g * 2.0f / k2,
                                     8.0f / k + g * 3.0f / k2,
                                     9.0f / k + g * 4.0f / k2,
                                     10.0f / k + g * 5.0f / k2,
                                     11.0f / k + g * 6.0f / k2 + g2 * 1.0f / k3,
                                     12.0f / k + g * 7.0f / k2 + g2 * 2.0f / k3,
                                     13.0f / k + g * 8.0f / k2 + g2 * 3.0f / k3,
                                     14.0f / k + g * 9.0f / k2 + g2 * 4.0f / k3,
                                     15.0f / k + g * 10.0f / k2 + g2 * 5.0f / k3}});
    for (size_t i = 0; i < 20; i++)
    {
        DAP_ASSERT_FLOAT_EQ(expected[i], delay(i, 4.0f, g));
    }
}
