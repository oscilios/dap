#include "dsp/NoiseGenerator.h"
#include "dsp/UniformDistribution.h"
#include "dap_gtest.h"

using namespace testing;
using namespace dap;
using namespace dap::dsp;

class NoiseGeneratorTest : public Test
{
};

DAP_TEST_F(NoiseGeneratorTest, normalized)
{
    using NoiseGen = NoiseGenerator<UniformDistribution>;
    NoiseGen white;
    NoiseGen pink;
    NoiseGen brown;
    NoiseGen oneOverF3;
    float g = 1.0f;
    for (int i=0; i<44100; ++i)
    {
        DAP_ASSERT_LE(std::fabs(white(g, NoiseGen::Color::White)), 1.0f);
        DAP_ASSERT_LE(std::fabs(pink(g, NoiseGen::Color::Pink)), 1.0f);
        DAP_ASSERT_LE(std::fabs(brown(g, NoiseGen::Color::Brown)), 1.0f);
        DAP_ASSERT_LE(std::fabs(oneOverF3(g, NoiseGen::Color::OneOverF3)), 1.0f);
    }
}
