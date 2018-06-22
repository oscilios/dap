#include "crtp/ugen/Mixer.h"
#include "dap_gtest.h"

#include <vector>

using namespace testing;
using namespace dap;
using namespace dap::crtp;

class MixerTest : public Test
{
};

DAP_TEST_F(MixerTest, basic)
{
    using bus_t = MixerBus<Node::Inputs<float, float>>;
    Mixer<Node::Inputs<bus_t, bus_t, bus_t, bus_t>> mixer;

    mixer.input("bus_0"_s).input("gain"_s) = 1.0f;
    mixer.input("bus_1"_s).input("gain"_s) = 2.0f;
    mixer.input("bus_2"_s).input("gain"_s) = 3.0f;
    mixer.input("bus_3"_s).input("gain"_s) = 4.0f;

    mixer.input("bus_0"_s).input("signal"_s) = 10.0f;
    mixer.input("bus_1"_s).input("signal"_s) = 20.0f;
    mixer.input("bus_2"_s).input("signal"_s) = 30.0f;
    mixer.input("bus_3"_s).input("signal"_s) = 40.0f;

    DAP_ASSERT_EQ(300.0f / std::sqrt(30.0f), mixer());
}
