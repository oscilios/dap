#ifndef DAP_CRTP_NODES_NODE_H
#define DAP_CRTP_NODES_NODE_H

#include "Port.h"
#include "fastmath/Var.h"
#include <cassert>
#include <memory>
#include <tuple>

#define NODE_INPUT_NAMES(...) decltype(dap::crtp::make_input_names(__VA_ARGS__))

namespace dap
{
    namespace crtp
    {
        class Node;
        template <typename Derived, typename Inputs>
        class NodeExpression;
        template <typename TInput>
        class ValueNode;
        template <typename TInput>
        using ConstantNode = const dap::crtp::ValueNode<TInput>;
        template <typename Processor, typename Inputs, typename InputNames>
        class ProcessorNode;
        template <typename TInputs, typename... TInputNamesTuples>
        class SplitNode;
        template <typename TInputs, typename Op>
        class JoinNode;

        template <typename Processor, typename Inputs, typename... InputNames>
        auto make_processor_node(Processor&&, Inputs&&, InputNames&&...); // TODO: remove

        template <typename... Names>
        constexpr std::tuple<Names...> make_input_names(Names&&...)
        {
            return {};
        }
        // given a prefix and a sequence of size_t, concatenates prefix with each number
        template <char... Chars, size_t... Is>
        constexpr auto
        make_input_names(constexpr_string<Chars...> prefix, std::index_sequence<Is...>)
        {
            return make_input_names((prefix + num_constexpr_string<Is>::value)...);
        }

        namespace traits
        {
            struct Tuple2VectorOfPorts;
            template <typename T = void>
            struct IsCrtpNode
            {
                constexpr static auto value = std::is_base_of<crtp::Node, T>::value;
            };
            template <typename T = void>
            constexpr bool isCrtpNode()
            {
                return std::is_base_of<crtp::Node, T>::value;
            }
            template <typename T = void>
            constexpr bool isCrtpNode(T)
            {
                return std::is_base_of<crtp::Node, T>::value;
            }
        } // namespace traits

        // retrieves stored value depending on the type
        template <typename T, DAP_REQUIRES(!traits::IsCrtpNode<T>::value)>
        inline constexpr auto dispatch(const T& value)
        {
            return fastmath::dispatch(value);
        }
        template <typename T, DAP_REQUIRES(traits::IsCrtpNode<T>::value)>
        inline constexpr auto dispatch(T& value)
        {
            return value();
        }
        template <typename T, DAP_REQUIRES(traits::IsCrtpNode<T>::value)>
        inline constexpr auto dispatch(T&& value)
        {
            return value();
        }
        template <typename T, DAP_REQUIRES(traits::IsCrtpNode<T>::value)>
        inline constexpr auto dispatch(T* value)
        {
            return value->self()();
        }

        template <typename T>
        struct ResultOfDispatch
        {
            static constexpr auto dispatch_rvalue(T value)
            {
                return dispatch(value);
            }
            using type = decltype(ResultOfDispatch<T>::dispatch_rvalue(T{}));
        };
        template <typename T>
        using result_of_dispatch_t = typename ResultOfDispatch<T>::type;

        template <size_t N, typename TInputs>
        using inputs_element_t = tuple_element_t<N, typename TInputs::type>;
    }
}

// class Node: represents runtime (virtual) interface
class dap::crtp::Node
{
protected:
    struct IO
    {
    };
    template <typename... Is>
    struct IOPlaceholder : IO
    {
        using type = std::tuple<Is...>;
        using size = typename std::tuple_size<type>;
        template <size_t N>
        using type_of = tuple_element_t<N, type>;
    };

public:
    template <typename... Is>
    struct Inputs final : IOPlaceholder<Is...>
    {
    };

    Node()            = default;
    Node(const Node&) = default;
    Node(Node&&)      = default;
    virtual ~Node()   = default;
    Node& operator=(const Node&) = default;
    Node& operator=(Node&&)                     = default;
    virtual std::vector<Port> ports()           = 0;
    virtual size_t portCount() const            = 0;
    virtual std::unique_ptr<Node> clone() const = 0;
    void process()
    {
        compute();
    }

protected:
    virtual void compute() = 0;
};

template <typename Derived, typename TInputs>
class dap::crtp::NodeExpression : public dap::crtp::Node
{
    static_assert(std::is_base_of<Node::IO, TInputs>::value,
                  "TInputs must be of type Node::Inputs");

public:
    using base_type  = NodeExpression<Derived, TInputs>;
    using input_type = typename TInputs::type;

private:
    void compute() override
    {
        (*this)();
    }
    std::vector<Port> ports() override
    {
        std::vector<Port> result;
        traits::Tuple2VectorOfPorts(m_inputs, result, this);
        return result;
    }
    size_t portCount() const override
    {
        return inputCount();
    }
    std::unique_ptr<Node> clone() const override
    {
        return std::make_unique<Derived>(self());
    }

protected:
    input_type m_inputs;

public:
    NodeExpression()                      = default;
    NodeExpression(const NodeExpression&) = default;
    NodeExpression(NodeExpression&&)      = default;
    ~NodeExpression() override            = default;
    NodeExpression& operator=(const NodeExpression&) = default;
    NodeExpression& operator=(NodeExpression&&) = default;
    constexpr Derived& self()
    {
        return static_cast<Derived&>(*this);
    }
    constexpr const Derived& self() const
    {
        return static_cast<const Derived&>(*this);
    }
    constexpr auto& inputs()
    {
        return m_inputs;
    }
    template <size_t N>
    constexpr auto& input()
    {
        return std::get<N>(m_inputs);
    }
    template <size_t N>
    constexpr const auto& input() const
    {
        return std::get<N>(m_inputs);
    }
    static constexpr size_t inputCount()
    {
        return TInputs::size::value;
    }
    template <char... Chars>
    constexpr auto& input(constexpr_string<Chars...>)
    {
        using name_t = constexpr_string<Chars...>;
        using idx_t  = Index<name_t, decltype(self().inputNames())>;
        return input<idx_t::value>();
    }
    template <char... Chars>
    constexpr bool hasInput(constexpr_string<Chars...> name)
    {
        return tupleContainsType(name, self().inputNames());
    }
    auto operator()()
    {
        return self()();
    }
    // conversion operator (i.e. int(), float(), etc)
    template <typename T>
    operator T()
    {
        return (*this)();
    }
};

template <typename TInput>
class dap::crtp::ValueNode : public NodeExpression<ValueNode<TInput>, Node::Inputs<TInput>>
{
    using base_type = NodeExpression<ValueNode<TInput>, Node::Inputs<TInput>>;
    static_assert(base_type::inputCount() == 1, "ValueNode must have a single input");

public:
    ValueNode() = default;
    template <typename T, DAP_REQUIRES(isArithmetic<T>())>
    ValueNode(const T& t)
    {
        base_type::template input<0>() = t;
    }
    template <typename T, DAP_REQUIRES(isArithmetic<T>())>
    ValueNode(T&& t)
    {
        base_type::template input<0>() = TInput(std::forward<T>(t));
    }
    template <typename... Ts>
    ValueNode(NodeExpression<Ts...>& t)
    : ValueNode(t())
    {
    }
    template <typename... Ts>
    ValueNode(NodeExpression<Ts...>&& t)
    : ValueNode(t.self()())
    {
    }
    template <typename E>
    ValueNode(const fastmath::BaseExpression<E>& t)
    : ValueNode(t.self()())
    {
    }
    template <typename E>
    ValueNode(fastmath::BaseExpression<E>&& t)
    : ValueNode(t.self()())
    {
    }
    ~ValueNode() = default;
    template <typename T, DAP_REQUIRES(isArithmetic<T>())>
    ValueNode& operator=(const T& x)
    {
        base_type::template input<0>() = x;
        return *this;
    }
    template <typename T, DAP_REQUIRES(isArithmetic<T>())>
    ValueNode& operator=(T&& x)
    {
        base_type::template input<0>() = TInput(std::forward<T>(x));
        return *this;
    }
    template <typename... Ts>
    ValueNode& operator=(NodeExpression<Ts...>& t)
    {
        *this = t.self()();
        return *this;
    }
    template <typename... Ts>
    ValueNode& operator=(NodeExpression<Ts...>&& t)
    {
        *this = t.self()();
        return *this;
    }
    template <typename E>
    ValueNode& operator=(const fastmath::BaseExpression<E>& t)
    {
        *this = t.self()();
        return *this;
    }
    template <typename E>
    ValueNode& operator=(fastmath::BaseExpression<E>&& t)
    {
        *this = t.self()();
        return *this;
    }
    auto operator()()
    {
        return base_type::template input<0>();
    }
};

template <typename Processor, typename Inputs, typename InputNames>
class dap::crtp::ProcessorNode
: public NodeExpression<ProcessorNode<Processor, Inputs, InputNames>, Inputs>
{
    static_assert(isTuple(InputNames{}), "InputNames must be a tuple");
    using base_type = NodeExpression<ProcessorNode<Processor, Inputs, InputNames>, Inputs>;
    Processor m_processor;

    template <size_t... Is>
    inline auto callProcessor(const std::index_sequence<Is...>&)
    {
        return m_processor(dispatch(std::get<Is>(base_type::template m_inputs))...);
    }

public:
    ProcessorNode() = default;
    template <typename...Ts>
    ProcessorNode(const Ts&...inputs)
    {
        base_type::m_inputs = std::forward_as_tuple(inputs...);
    }
    template <typename...Ts>
    ProcessorNode(Ts&&...inputs)
    {
        base_type::m_inputs = std::forward_as_tuple(inputs...);
    }
    static constexpr auto inputNames()
    {
        return InputNames{};
    }
    auto operator()()
    {
        static constexpr auto indices =
            std::make_index_sequence<base_type::template inputCount()>{};
        return callProcessor(indices);
    }
};

namespace dap
{
    namespace crtp
    {
        namespace detail
        {
            template <template <class...> class F, class C1, class C2>
            struct TargetTransform;

            template <template <class...> class F, class C1, class C2>
            using targetTransform = typename TargetTransform<F, C1, C2>::type;

            template <template <class...> class F,
                      template <class...> class C1,
                      class... T1,
                      template <class...> class C2,
                      class... T2>
            struct TargetTransform<F, C1<T1...>, C2<T2...>>
            {
                static_assert(sizeof...(T1) == sizeof...(T2), "size mismatch");
                using type = C1<F<T1, T2>...>;
            };

            template <class T, class U>
            struct Target
            {
                static constexpr T sinkName()
                {
                    return {};
                }
                static constexpr U inputNames()
                {
                    return {};
                }
            };

            template <class T, class U>
            struct TargetCat
            {
                using type = Target<T, U>;
            };

            template <class T, class U>
            using targetcat = typename TargetCat<T, U>::type;
        }
    }
}

// splits input<0> into multiple nodes and multiple inputs
template <typename TInputs, typename... TInputNamesTuples>
class dap::crtp::SplitNode
: public NodeExpression<SplitNode<TInputs, TInputNamesTuples...>, TInputs>
{
    using base_type = NodeExpression<SplitNode<TInputs, TInputNamesTuples...>, TInputs>;
    static constexpr auto branchCount = typename TInputs::size{} - 1;

    constexpr static auto branchNames()
    {
        static_assert(typename TInputs::size{} > 1, "Inputs must be greater than 1");
        return make_input_names("branch_"_s, std::make_index_sequence<branchCount>{});
    }

    template <typename T, size_t... Is>
    inline auto split(T&& value, const std::index_sequence<Is...>&)
    {
        using dests_t      = std::tuple<TInputNamesTuples...>;
        using sink_names_t = decltype(SplitNode<TInputs, TInputNamesTuples...>::branchNames());
        static constexpr auto targets = detail::targetTransform<detail::targetcat, sink_names_t, dests_t>{};

        for_each(targets, [&](auto& target) {
            for_each(target.inputNames(), [&](auto name) {
                base_type::template input(target.sinkName()).input(name) = value;
            });
        });

        return std::make_tuple(dispatch(std::get<1 + Is>(base_type::template m_inputs))...);
    }

public:
    constexpr static auto inputNames()
    {
        return std::tuple_cat(make_input_names("source"_s), branchNames());
    }
    auto operator()()
    {
        constexpr static auto indices = std::make_index_sequence<branchCount>{};
        return split(dispatch(base_type::template input<0>()), indices);
    }
};

// joins SplitNodes and applies Op to each branch
template <typename TInputs, typename Op>
class dap::crtp::JoinNode: public NodeExpression<JoinNode<TInputs, Op>, TInputs>
{
    using base_type = NodeExpression<JoinNode<TInputs, Op>, TInputs>;
    Op m_op;

    template <typename Tuple, size_t... Is>
    inline auto join(Tuple&& tpl)
    {
        return apply(m_op, std::forward<Tuple>(tpl));
    }
    template <typename... Tuples>
    inline auto join(Tuples&&... tpls)
    {
        return m_op(join(std::forward<Tuples>(tpls))...);
    }
    template <size_t... Is>
    inline auto join(const std::index_sequence<Is...>&)
    {
        return join(dispatch(std::get<Is>(base_type::template m_inputs))...);
    }
    static constexpr auto indices()
    {
        return std::make_index_sequence<base_type::inputCount()>{};
    }

public:
    static constexpr auto inputNames()
    {
        return make_input_names("branch_"_s, indices());
    }
    auto operator()()
    {
        static constexpr auto is = indices();
        return join(is);
    }
};

template <typename Processor, typename Inputs, typename... InputNames>
auto dap::crtp::make_processor_node(Processor&&, Inputs&&, InputNames&&...)
{
    return ProcessorNode<Processor, Inputs, std::tuple<InputNames...>>{};
}

namespace dap
{
    namespace crtp
    {
        namespace traits
        {
            struct Tuple2VectorOfPorts
            {
                template <int index, typename... Ts>
                struct Insert
                {
                    void
                    operator()(std::tuple<Ts...>& t, std::vector<crtp::Port>& v, crtp::Node* parent)
                    {
                        using size                   = std::tuple_size<std::tuple<Ts...>>;
                        constexpr size_t reversedIdx = size::value - index - 1;
                        v.emplace_back(crtp::Port(parent, reversedIdx, &std::get<reversedIdx>(t)));
                        Insert<index - 1, Ts...>{}(t, v, parent);
                    }
                };

                template <typename... Ts>
                struct Insert<0, Ts...>
                {
                    void
                    operator()(std::tuple<Ts...>& t, std::vector<crtp::Port>& v, crtp::Node* parent)
                    {
                        using size                   = std::tuple_size<std::tuple<Ts...>>;
                        constexpr size_t reversedIdx = size::value - 1;
                        v.emplace_back(crtp::Port(parent, reversedIdx, &std::get<reversedIdx>(t)));
                    }
                };

                template <typename... Ts>
                Tuple2VectorOfPorts(std::tuple<Ts...>& t,
                                    std::vector<crtp::Port>& v,
                                    crtp::Node* parent)
                {
                    using size = std::tuple_size<std::tuple<Ts...>>;
                    Insert<size::value - 1, Ts...>{}(t, v, parent);
                }
            };
        } // namespace traits
    }     // namespace crtp
} // namespace dap

#include "private/BinaryNodeOpsImpl.hpp"
#include "private/UnaryNodeOpsImpl.hpp"

#endif // DAP_CRTP_NODE_NODE_H
