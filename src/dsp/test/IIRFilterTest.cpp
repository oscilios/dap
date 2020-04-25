#include "dsp/IIRFilter.h"
#include "fastmath/Var.h"
#include "dap_gtest.h"

using namespace testing;
using namespace dap;
using namespace dap::dsp;

class IIRFilterTest : public Test
{
};

DAP_TEST_F(IIRFilterTest, pink)
{
    std::vector<var> a     = {1, -2.494956002, 2.017265875, -0.522189400};
    std::vector<var> b     = {0.049922035, -0.095993537, 0.050612699, -0.004408786};
    std::vector<var> input = {1, 2, 3, 4, 5, 6};
    IIRFilter<var, 4> filter;
    filter.set(b.data(), a.data());
    std::vector<var> expected = {0.049922, 0.128404, 0.228048, 0.344537, 0.475280, 0.618652};
    for (size_t i = 0; i < input.size(); i++)
    {
        DAP_ASSERT_NEAR(expected[i], filter(input[i]), 1e-4f);
    }
}

DAP_TEST_F(IIRFilterTest, oneOverF_pulse)
{
    std::vector<var> a = {1, -2.494956002, 2.017265875, -0.522189400};
    std::vector<var> b = {0.049922035, -0.095993537, 0.050612699, -0.004408786};
    IIRFilter<var, 4> filter;
    filter.set(b.data(), a.data());
    std::vector<var> input    = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<var> expected = {0.0499220,
                                 0.0285597,
                                 0.0211620,
                                 0.0168456,
                                 0.0142533,
                                 0.0126298,
                                 0.0115547,
                                 0.0107938,
                                 0.0102163,
                                 0.0097489,
                                 0.0093506};
    for (size_t i = 0; i < input.size(); i++)
    {
        DAP_ASSERT_NEAR(expected[i], filter(input[i]), 1e-4f);
    }
}
DAP_TEST_F(IIRFilterTest, oneOverF2_step)
{
    std::vector<var> a = {1, -2.494956002, 2.017265875, -0.522189400};
    std::vector<var> b = {0.049922035, -0.095993537, 0.050612699, -0.004408786};
    IIRFilter<var, 4> f1;
    IIRFilter<var, 4> f2;
    f1.set(b.data(), a.data());
    f2.set(b.data(), a.data());
    std::vector<var> input    = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    std::vector<var> expected = {0.0024922,
                                 0.0053437,
                                 0.0082723,
                                 0.0111630,
                                 0.0139961,
                                 0.0167843,
                                 0.0195464,
                                 0.0222988,
                                 0.0250531,
                                 0.0278162,
                                 0.0305916};
    for (size_t i = 0; i < input.size(); i++)
    {
        DAP_ASSERT_NEAR(expected[i], f2(f1(input[i])), 1e-4f);
    }
}
