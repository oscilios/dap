#include "dsp/Mixer.h"
#include <gtest/gtest.h>

#include <vector>

using namespace testing;
using namespace dap;
using namespace dap::dsp;

TEST(MixerTest, mixer)
{
    Mixer mixer;
    Mixer::Bus bus;
    ASSERT_FLOAT_EQ(300.0f / std::sqrt(4.0f), mixer(1*10, 2*20, 3*30, 4*40));
    ASSERT_FLOAT_EQ(300.0f / std::sqrt(4.0f), mixer(bus(1,10), bus(2,20), bus(3,30), bus(4,40)));
}
TEST(MixerTest, normalized_mixer)
{
    NormalizedMixer mixer;
    NormalizedMixer::Bus bus;
    ASSERT_FLOAT_EQ(
        300.0f / std::sqrt(30.0f),
        mixer(make_tuple(1, 10), make_tuple(2, 20), make_tuple(3, 30), make_tuple(4, 40)));
    ASSERT_FLOAT_EQ(300.0f / std::sqrt(30.0f),
                        mixer(bus(1, 10), bus(2, 20), bus(3, 30), bus(4, 40)));
}
