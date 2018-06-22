#include "crtp/nodes/Node.h"
#include "dap_gtest.h"

using namespace testing;
using namespace dap;
using namespace dap::crtp;

class ProcessorNodeTest : public Test
{
};

class ProdProcessor
{
public:
    auto operator()(float x, float y)
    {
        return x * y;
    }
};

template <typename T>
class TProdProcessor
{
public:
    auto operator()(T x, T y)
    {
        return x * y;
    }
};

DAP_TEST_F(ProcessorNodeTest, non_templated_processor)
{
    // ProcessorNode<ProdProcessor, Node::Inputs<float, float>, decltype(Node::make_params("lhs"_s,
    // "rhs"_s))>
    //    prod;
    auto prod = make_processor_node(ProdProcessor{}, Node::Inputs<float, float>{}, "lhs"_s, "rhs"_s);

    prod.input("lhs"_s) = 2.0f;
    prod.input("rhs"_s) = 3.0f;
    DAP_ASSERT_EQ(6.0f, prod);
}
DAP_TEST_F(ProcessorNodeTest, templated_processor)
{
    auto prod = make_processor_node(TProdProcessor<float>{}, Node::Inputs<float, float>{}, "lhs"_s, "rhs"_s);

    prod.input("lhs"_s) = 2.0f;
    prod.input("rhs"_s) = 3.0f;
    DAP_ASSERT_EQ(6.0f, prod);
}
