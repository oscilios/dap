#include "dsp/Mixer.h"
#include "dsp/DelayLine.h"
#include "dsp/FeedbackLine.h"
#include "dsp/CombFilter.h"
#include "fastmath/Var.h"
#include "dap_gtest.h"

#include <vector>

using namespace testing;
using namespace dap;
using namespace dap::dsp;

class VarTest : public Test
{
};

DAP_TEST_F(VarTest, mixer)
{
    Mixer mixer;
    Mixer::Bus bus;
    DAP_ASSERT_FLOAT_EQ(300.0f / std::sqrt(4.0f),
                        mixer(bus(ivar(1), ivar(10)), bus(fvar(2), fvar(10)*ivar(2)), bus(3, 30), bus(4, 40)));
}
DAP_TEST_F(VarTest, normalized_mixer)
{
    NormalizedMixer mixer;
    NormalizedMixer::Bus bus;
    DAP_ASSERT_FLOAT_EQ(300.0f / std::sqrt(30.0f),
                        mixer(bus(ivar(1), ivar(10)), bus(fvar(2), fvar(10)*ivar(2)), bus(3, 30), bus(4, 40)));
}
DAP_TEST_F(VarTest, delay_4samples)
{
    DelayLine<fvar, 8> delay;
    std::array<float, 20> expected(
        {{0, 0, 0, 0, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30}});
    for (size_t i = 0; i < 20; i++)
    {
        auto input = fvar(i) * fvar(2.0);
        DAP_ASSERT_FLOAT_EQ(expected[i], delay(input, 4));
    }
}
DAP_TEST_F(VarTest, 4d5samples)
{
    DelayLine<fvar, 8> delay;
    std::array<float, 20> expected(
        {{0, 0, 0, 0, 0, 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29}});
    for (size_t i = 0; i < 20; i++)
    {
        auto input = fvar(i) * fvar(2.0);
        DAP_ASSERT_FLOAT_EQ(expected[i], delay(input, 4.5f));
    }
}
DAP_TEST_F(VarTest, 4samplesWithFeedback)
{
    const float g = 0.5;
    const float g2 = g*g;
    const float k = std::sqrt(1.0f + g2);
    const float k2 = k*k;
    const float k3 = k*k*k;
    FeedbackLine<fvar, 8> delay;
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
        DAP_ASSERT_FLOAT_EQ(expected[i], delay(ivar(i), fvar(4.0f), fvar(1.0)/fvar(2.0)));
    }
}

DAP_TEST_F(VarTest, FeedbackCombFilter)
{
  FeedbackCombFilter<float, 16> fcomb;
  FeedbackCombFilter<fvar, 16> vcomb;
  for (size_t i = 0; i < 32; i++)
      DAP_ASSERT_FLOAT_EQ(fcomb(i / 32.0f, 4, 0.5f),
                          vcomb(fvar(i) / fvar(32), ivar(2) * ivar(2), fvar(1) / fvar(2)));
}

DAP_TEST_F(VarTest, FeedforwardCombFilter)
{
  FeedforwardCombFilter<float, 16> fcomb;
  FeedforwardCombFilter<fvar, 16> vcomb;
  for (size_t i = 0; i < 32; i++)
      DAP_ASSERT_FLOAT_EQ(fcomb(i / 32.0f, 4, 0.5f),
                          vcomb(fvar(i) / fvar(32), ivar(2) * ivar(2), fvar(1) / fvar(2)));
}
