#include "crtp/nodes/Node.h"
#include "base/VariadicOps.h"
#include <gtest/gtest.h>
#include "dsp/Pwm.h"
#include "fastmath/TypeTraits.h"
#include "fastmath/Var.h"

#include <iostream>
#include <sndfile.hh>
#include <vector>

using namespace testing;
using namespace dap;
using namespace dap::crtp;

TEST(NodeTest, Sum)
{
    AddNode<int, int> s(2, 3);

    ASSERT_EQ(2u, s.inputCount());

    ASSERT_EQ(5, s);

    s.input("x"_s) = 5;
    s.input("y"_s) = 7;

    ASSERT_EQ(12, s);
}

TEST(NodeTest, AbstractInterface)
{
    std::vector<Node*> nodes;

    AddNode<int, float> s(2, 3.5f);
    nodes.push_back(&s);

    for (auto& node : nodes)
        node->process();

    // using concrete interface
    ASSERT_EQ(5.5, s());

    // using abstract interface
    auto& node = *nodes[0];
    auto ports = node.ports();
    ASSERT_EQ(2u, node.portCount());
    ASSERT_EQ(2u, ports.size());
    ASSERT_EQ(2, ports[0].get<int>());
    ASSERT_EQ(3.5f, ports[1].get<float>());

    ASSERT_EQ(&s, ports[0].getParent());
    ASSERT_EQ(&s, ports[1].getParent());
    ASSERT_EQ(0u, ports[0].getId());
    ASSERT_EQ(1u, ports[1].getId());

    float x = 7.5;
    node.ports()[1].set(x);
    ASSERT_EQ(7.5, ports[1].get<float>());
    node.process();
}
TEST(NodeTest, UnaryNode)
{
    ValueNode<float> x(2.0);
    ValueNode<float> y(5.0);
    ASSERT_EQ(4.0, x * x);
    ASSERT_EQ(25.0, y * y);
    ASSERT_EQ(29.0, x * x + y * y);

    ValueNode<float> s = sqr(2.0);
    ASSERT_EQ(4.0, sqr(x));

    float pi = M_PI;
    float w1 = pi / 4.0f;
    float w2 = w1 / 2.0f;
    CosNode<float> c1(w1);
    CosNode<float> c2(w2);
    ASSERT_EQ(std::cos(w1), c1);
    ASSERT_EQ(std::cos(w2), c2);
    ASSERT_EQ(std::cos(w1) + std::cos(w2), c1 + c2);

    SqrNode<float> s0(2.0f);
    SqrNode<float> s1(5.0f);
    auto s2 = s0 + s1;
    ASSERT_EQ(29.0f, s2);

    ASSERT_EQ(-2.0f, -ValueNode<float>(2.0));
    ASSERT_EQ(std::abs(-2.0f), abs(ValueNode<float>(-2.0f)));
    ASSERT_EQ(std::fabs(-2.0f), fabs(ValueNode<float>(-2.0f)));
    ASSERT_EQ(std::ceil(2.7f), ceil(ValueNode<float>(2.7f)));
    ASSERT_EQ(std::floor(2.7f), floor(ValueNode<float>(2.7f)));
    ASSERT_EQ(std::trunc(2.7f), trunc(ValueNode<float>(2.7f)));
    ASSERT_EQ(std::log(2.0f), log(ValueNode<float>(2.0f)));
    ASSERT_EQ(std::log10(2.0f), log10(ValueNode<float>(2.0f)));
    ASSERT_EQ(std::log1p(2.0f), log1p(ValueNode<float>(2.0f)));
    ASSERT_EQ(std::sqrt(2.0f), sqrt(ValueNode<float>(2.0f)));
    ASSERT_EQ(std::sin(pi / 3.0f), sin(ValueNode<float>(pi / 3.0f)));
    ASSERT_EQ(std::cos(pi / 3.0f), cos(ValueNode<float>(pi / 3.0f)));
    ASSERT_EQ(std::tan(pi / 3.0f), tan(ValueNode<float>(pi / 3.0f)));
}

TEST(NodeTest, chain)
{
    ValueNode<int> v1(1), v2(2), v3(3), v4(4);

    auto s0  = v1 + v2;
    auto s1  = v3 + v4;
    auto sum = s0 + s1;
    ASSERT_EQ(10, sum);

    v1 = v1 * 10;
    v2 = v2 * 10;
    v3 = v3 * 10;
    v4 = v4 * 10;

    s0  = v1 + v2;
    s1  = v3 + v4;
    sum = s0 + s1;
    ASSERT_EQ(100, sum);
}
TEST(NodeTest, ops)
{
    ValueNode<float> v1(1.0f), v2(2.0f), v3(3.0f), v4(4.0f);

    auto op = v1 + v2 * v3 + v1 / v4;
    ASSERT_EQ(7.25, op);
}
TEST(NodeTest, value_node)
{
    ValueNode<int> x(42);
    ASSERT_EQ(42, x);
    x = 42 * 2;
    ASSERT_EQ(84, x);

    ValueNode<int> y(6);
    auto s = x + y;
    ASSERT_EQ(90, s);

    using Sum = AddNode<ValueNode<int>*, ValueNode<int>*>;
    Sum sum(&x, &y);
    ASSERT_EQ(90, sum);

    y = 7;
    x = 5;
    ASSERT_EQ(12, sum);
}

TEST(NodeTest, BinaryOps)
{
    using Sum = AddNode<int, int>;

    ValueNode<int> x;
    int y;
    y = 7;
    x = 5;
    Sum sum(x, y);

    ASSERT_EQ(12, sum);

    ASSERT_EQ(36, 3 * sum);
    ASSERT_EQ(36, sum * 3);
    ASSERT_EQ(4, sum / 3);
    ASSERT_EQ(0.25f, 3.0f / sum);
    ASSERT_EQ(15, sum + 3);
    ASSERT_EQ(63, 3 + sum * 5);
    ASSERT_EQ(9, sum - 3);
    ASSERT_EQ(-9, 3 - sum);
    ASSERT_EQ(-57, 3 - sum * 5);
    ASSERT_EQ(144, pow(sum, 2));
    ASSERT_EQ(42, max(42, sum));
    ASSERT_EQ(42, min(42.0, pow(sum, 2)));

    // var-node ops
    ASSERT_EQ(36, var(3) * sum);
    ASSERT_EQ(36, sum * var(3));
    ASSERT_EQ(4, sum / var(3));
    ASSERT_EQ(0.25f, var(3.0f) / sum);
    ASSERT_EQ(15, sum + var(3));
    ASSERT_EQ(63, 3 + sum * var(5));
    ASSERT_EQ(9, sum - var(3));
    ASSERT_EQ(-9, var(3) - sum);
    ASSERT_EQ(-57, var(3) - sum * var(5));
    ASSERT_EQ(144, pow(float(sum), var(2)));

    // node to node
    ValueNode<float> xf(5);
    ASSERT_EQ(60, x * sum);
    ASSERT_EQ(60, sum * x);
    ASSERT_EQ(12.0f / 5.0f, sum / xf);
    ASSERT_FLOAT_EQ(5.0f / 12.0f, xf / sum);
    ASSERT_EQ(17, sum + x);
    ASSERT_EQ(3 + 12 * 5, 3 + sum * x);
    ASSERT_EQ(12 - 5, sum - x);
    ASSERT_EQ(5 - 12, x - sum);
    ASSERT_EQ(5 - 12 * 5, x - sum * x);
    ASSERT_EQ(std::pow(12, 5), pow(sum, x));

    // op to op
    ASSERT_EQ(12 * 12, sum * sum);
    ASSERT_EQ(1, sum / sum);
    ASSERT_EQ(12 + 12, sum + sum);
    ASSERT_EQ(0, sum - sum);
}
TEST(NodeTest, split_node_multiple_inputs_same_node)
{
    using pwm_t = dap::crtp::ProcessorNode<
        dap::dsp::Pwm<float>,
        Node::Inputs<float, float, float, float, float>,
        NODE_INPUT_NAMES("gain"_s, "frequency"_s, "phase"_s, "samplerate"_s, "dutycycle"_s)>;

    SplitNode<Node::Inputs<ValueNode<float>, pwm_t, pwm_t>,
              NODE_INPUT_NAMES("gain"_s, "frequency"_s),
              NODE_INPUT_NAMES("phase"_s, "dutycycle"_s)>
        splitter;

    splitter.input("source"_s).input<0>() = 42.0;
    splitter();
    ASSERT_FLOAT_EQ(42.0f, splitter.input("branch_0"_s).input("gain"_s));
    ASSERT_FLOAT_EQ(42.0f, splitter.input("branch_0"_s).input("frequency"_s));
    ASSERT_FLOAT_EQ(0.0f, splitter.input("branch_0"_s).input("phase"_s));
    ASSERT_FLOAT_EQ(0.0f, splitter.input("branch_0"_s).input("dutycycle"_s));
    ASSERT_FLOAT_EQ(0.0f, splitter.input("branch_0"_s).input("samplerate"_s));

    ASSERT_FLOAT_EQ(0.0f, splitter.input("branch_1"_s).input("gain"_s));
    ASSERT_FLOAT_EQ(0.0f, splitter.input("branch_1"_s).input("frequency"_s));
    ASSERT_FLOAT_EQ(42.0f, splitter.input("branch_1"_s).input("phase"_s));
    ASSERT_FLOAT_EQ(42.0f, splitter.input("branch_1"_s).input("dutycycle"_s));
    ASSERT_FLOAT_EQ(0.0f, splitter.input("branch_1"_s).input("samplerate"_s));
}
TEST(NodeTest, split_node_check_call_operator)
{
    SplitNode<Node::Inputs<ValueNode<float>, AddNode<float, float>, MultiplyNode<float, float>>,
              NODE_INPUT_NAMES("x"_s, "y"_s),
              NODE_INPUT_NAMES("x"_s, "y"_s)>
        splitter;

    splitter.input("source"_s).input<0>() = 42.0;

    float sum           = 0;
    float prod          = 0;
    std::tie(sum, prod) = splitter();
    ASSERT_FLOAT_EQ(42.0f + 42.0f, sum);
    ASSERT_FLOAT_EQ(42.0f * 42.0f, prod);
}
TEST(NodeTest, join_node_single_branch)
{
    JoinNode<Node::Inputs<
                 SplitNode<Node::Inputs<float, AddNode<float, float>, MultiplyNode<float, float>>,
                           NODE_INPUT_NAMES("x"_s, "y"_s),
                           NODE_INPUT_NAMES("x"_s, "y"_s)>>,
             std::plus<float>>
        joinSum;

    joinSum.input("branch_0"_s).input("source"_s) = 42.0;

    ASSERT_FLOAT_EQ(42.0f + 42.0f + 42.0f * 42.0f, joinSum());
}
TEST(NodeTest, join_node_several_branches)
{
    JoinNode<
        Node::Inputs<
            SplitNode<
                Node::Inputs<ValueNode<float>, AddNode<float, float>, MultiplyNode<float, float>>,
                NODE_INPUT_NAMES("x"_s, "y"_s),
                NODE_INPUT_NAMES("x"_s, "y"_s)>,
            SplitNode<Node::Inputs<ValueNode<float>,
                                   AddNode<float, float>,
                                   MultiplyNode<float, float>,
                                   AddNode<float, float>>,
                      NODE_INPUT_NAMES("x"_s, "y"_s),
                      NODE_INPUT_NAMES("x"_s, "y"_s),
                      NODE_INPUT_NAMES("y"_s)>,
            SplitNode<Node::Inputs<ValueNode<float>,
                                   AddNode<float, float>,
                                   MultiplyNode<float, float>,
                                   MultiplyNode<float, float>>,
                      NODE_INPUT_NAMES("x"_s, "y"_s),
                      NODE_INPUT_NAMES("x"_s, "y"_s),
                      NODE_INPUT_NAMES("x"_s)>>,
        VariadicSum>
        joinSum;

    joinSum.input("branch_0"_s).input("source"_s).input<0>() = 42.0;
    joinSum.input("branch_1"_s).input("source"_s).input<0>() = 6.0;
    joinSum.input("branch_2"_s).input("source"_s).input<0>() = 5.0;

    joinSum.input("branch_1"_s).input("branch_2"_s).input("x"_s) = 19.0;
    joinSum.input("branch_2"_s).input("branch_2"_s).input("y"_s) = 7.0;

    auto expected = (42.0f + 42.0f + 42.0f * 42.0f) + (6.0f + 6.0f + 6.0f * 6.0f + 19.0f + 6.0f) +
                    (5.0f + 5.0f + 5.0f * 5.0f + 5.0f * 7.0f);

    ASSERT_FLOAT_EQ(expected, joinSum());
}
