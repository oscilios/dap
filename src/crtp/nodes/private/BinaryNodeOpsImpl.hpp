#define DAP_CRTP_STD_BINARY_NODE(func, name_struct) \
    struct name_struct                              \
    {                                               \
        template <typename T, typename U>           \
        auto operator()(T t, U u) const noexcept    \
        {                                           \
            return func##_impl(t, u);               \
        }                                           \
    };                                              \
    template <typename I1, typename I2>             \
    using name_struct##Node = BinaryNode<name_struct, I1, I2>;

#define DAP_CRTP_STD_BINARY_NODE_OPERATOR(op, opnode)                                \
    template <typename D1, typename I1, typename D2, typename I2>                    \
    inline constexpr auto operator op(NodeExpression<D1, I1> n1,                     \
                                      NodeExpression<D2, I2> n2) noexcept            \
    {                                                                                \
        return opnode<D1, D2>{n1.self(), n2.self()};                                 \
    }                                                                                \
    template <typename D1, typename I1, typename D2, typename I2>                    \
    inline constexpr auto operator op(NodeExpression<D1, I1>* n1,                    \
                                      NodeExpression<D2, I2>* n2) noexcept           \
    {                                                                                \
        return opnode<D1*, D2*>{n1->self(), n2->self()};                             \
    }                                                                                \
    template <typename T, typename D, typename I, DAP_REQUIRES(isArithmetic<T>())>   \
    inline constexpr auto operator op(T const& t, NodeExpression<D, I> n) noexcept   \
    {                                                                                \
        return opnode<T, D>{t, n.self()};                                            \
    }                                                                                \
    template <typename D, typename I, typename T, DAP_REQUIRES(isArithmetic<T>())>   \
    inline constexpr auto operator op(NodeExpression<D, I> n, T const& t) noexcept   \
    {                                                                                \
        return opnode<D, T>{n.self(), t};                                            \
    }                                                                                \
    template <typename E, typename D, typename I>                                    \
    inline constexpr auto operator op(fastmath::BaseExpression<E> const& e,          \
                                      NodeExpression<D, I> n) noexcept               \
    {                                                                                \
        return opnode<E, D>{e.self(), n.self()};                                     \
    }                                                                                \
    template <typename D, typename I, typename E>                                    \
    inline constexpr auto operator op(NodeExpression<D, I> n,                        \
                                      fastmath::BaseExpression<E> const& e) noexcept \
    {                                                                                \
        return opnode<D, E>{n.self(), e.self()};                                     \
    }

#define DAP_CRTP_STD_BINARY_NODE_FUNCTOR(func, opnode)                                          \
    template <typename D1, typename I1, typename D2, typename I2>                               \
    inline constexpr auto func(NodeExpression<D1, I1> n1, NodeExpression<D2, I2> n2) noexcept   \
    {                                                                                           \
        return opnode<D1, D2>{n1.self(), n2.self()};                                            \
    }                                                                                           \
    template <typename D1, typename I1, typename D2, typename I2>                               \
    inline constexpr auto func(NodeExpression<D1, I1>* n1, NodeExpression<D2, I2>* n2) noexcept \
    {                                                                                           \
        return opnode<D1*, D2*>{n1->self(), n2->self()};                                        \
    }                                                                                           \
    template <typename T, typename D, typename I, DAP_REQUIRES(isArithmetic<T>())>              \
    inline constexpr auto func(T const& t, NodeExpression<D, I> n) noexcept                     \
    {                                                                                           \
        return opnode<T, D>{t, n.self()};                                                       \
    }                                                                                           \
    template <typename D, typename I, typename T, DAP_REQUIRES(isArithmetic<T>())>              \
    inline constexpr auto func(NodeExpression<D, I> n, T const& t) noexcept                     \
    {                                                                                           \
        return opnode<D, T>{n.self(), t};                                                       \
    }                                                                                           \
    template <typename E, typename D, typename I>                                               \
    inline constexpr auto func(fastmath::isBaseExpression<E> const& e,                          \
                               NodeExpression<D, I> n) noexcept                                 \
    {                                                                                           \
        return opnode<E, D>{e.self(), n.self()};                                                \
    }                                                                                           \
    template <typename D, typename I, typename E>                                               \
    inline constexpr auto func(NodeExpression<D, I> n,                                          \
                               fastmath::BaseExpression<E> const& e) noexcept                   \
    {                                                                                           \
        return opnode<D, E>{n.self(), e.self()};                                                \
    }

#define DAP_CRTP_STD_BINARY_NODE_COMPARE_OPERATOR(op, func)                                        \
    template <typename... Ts, typename... Us>                                                      \
    inline constexpr auto operator op(NodeExpression<Ts...> n1, NodeExpression<Us...> n2) noexcept \
    {                                                                                              \
        return func(n1(), n2());                                                                   \
    }                                                                                              \
    template <typename T, typename... Ts>                                                          \
    inline constexpr auto operator op(T const& n1, NodeExpression<Ts...> n2) noexcept              \
    {                                                                                              \
        return func(n1, n2());                                                                     \
    }                                                                                              \
    template <typename... Ts, typename T>                                                          \
    inline constexpr auto operator op(NodeExpression<Ts...> n1, T const& n2) noexcept              \
    {                                                                                              \
        return func(n1(), n2);                                                                     \
    }

namespace dap
{
    namespace crtp
    {
        template <typename Fn, typename TInput1, typename TInput2>
        using BinaryNode = ProcessorNode<Fn,
                                         Node::Inputs<TInput1, TInput2>,
                                         decltype(make_input_names("x"_s, "y"_s))>;

        template <typename T, typename U>
        inline auto add_impl(T t, U u) noexcept
        {
            return t + u;
        }
        template <typename T, typename U>
        inline auto subtract_impl(T t, U u) noexcept
        {
            return t - u;
        }

        template <typename T, typename U>
        inline auto multiply_impl(T t, U u) noexcept
        {
            return t * u;
        }

        template <typename T, typename U>
        inline auto divide_impl(T t, U u) noexcept
        {
            return t / u;
        }

        template <typename T, typename U>
        inline auto power_impl(T t, U u) noexcept
        {
            return std::pow(t, u);
        }

        template <typename T, typename U>
        inline auto hypot_impl(T t, U u) noexcept
        {
            return std::hypot(t, u);
        }
        template <typename T, typename U>
        inline auto equal_impl(T t, U u) noexcept
        {
            return t == u;
        }
        template <typename T, typename U>
        inline auto not_equal_impl(T t, U u) noexcept
        {
            return t != u;
        }

        template <typename T, typename U>
        inline auto less_impl(T t, U u) noexcept
        {
            return t < u;
        }

        template <typename T, typename U>
        inline auto greater_impl(T t, U u) noexcept
        {
            return t > u;
        }

        template <typename T, typename U>
        inline auto less_or_equal_impl(T t, U u) noexcept
        {
            return t <= u;
        }

        template <typename T, typename U>
        inline auto greater_or_equal_impl(T t, U u) noexcept
        {
            return t >= u;
        }

        template <typename T>
        inline auto max_impl(T x, T y) noexcept
        {
            return std::max<T>(x, y);
        }

        template <typename T>
        inline auto min_impl(T x, T y) noexcept
        {
            return std::min<T>(x, y);
        }

        // create Nodes and overloaded functions
        DAP_CRTP_STD_BINARY_NODE(add, Add)
        DAP_CRTP_STD_BINARY_NODE(subtract, Subtract)
        DAP_CRTP_STD_BINARY_NODE(multiply, Multiply)
        DAP_CRTP_STD_BINARY_NODE(divide, Divide)
        DAP_CRTP_STD_BINARY_NODE(power, Power)
        DAP_CRTP_STD_BINARY_NODE(hypot, Hypot)
        DAP_CRTP_STD_BINARY_NODE(equal, Equal)
        DAP_CRTP_STD_BINARY_NODE(not_equal, NotEqual)
        DAP_CRTP_STD_BINARY_NODE(less, Less)
        DAP_CRTP_STD_BINARY_NODE(greater, Greater)
        DAP_CRTP_STD_BINARY_NODE(less_or_equal, LessOrEqual)
        DAP_CRTP_STD_BINARY_NODE(greater_or_equal, GreaterOrEqual)
        DAP_CRTP_STD_BINARY_NODE(max, Max)
        DAP_CRTP_STD_BINARY_NODE(min, Min)

        // BINARY OPERATORS
        DAP_CRTP_STD_BINARY_NODE_OPERATOR(+, AddNode)
        DAP_CRTP_STD_BINARY_NODE_OPERATOR(-, SubtractNode)
        DAP_CRTP_STD_BINARY_NODE_OPERATOR(*, MultiplyNode)
        DAP_CRTP_STD_BINARY_NODE_OPERATOR(/, DivideNode)

        DAP_CRTP_STD_BINARY_NODE_FUNCTOR(pow, PowerNode)
        DAP_CRTP_STD_BINARY_NODE_FUNCTOR(hypot, HypotNode)
        DAP_CRTP_STD_BINARY_NODE_FUNCTOR(max, MaxNode)
        DAP_CRTP_STD_BINARY_NODE_FUNCTOR(min, MinNode)

        DAP_CRTP_STD_BINARY_NODE_COMPARE_OPERATOR(==, equal_impl)
        DAP_CRTP_STD_BINARY_NODE_COMPARE_OPERATOR(!=, not_equal_impl)
        DAP_CRTP_STD_BINARY_NODE_COMPARE_OPERATOR(<, less_impl)
        DAP_CRTP_STD_BINARY_NODE_COMPARE_OPERATOR(>, greater_impl)
        DAP_CRTP_STD_BINARY_NODE_COMPARE_OPERATOR(<=, less_or_equal_impl)
        DAP_CRTP_STD_BINARY_NODE_COMPARE_OPERATOR(>=, greater_or_equal_impl)

    } // namespace crtp
} // namespace dap

#undef DAP_CRTP_STD_BINARY_NODE
#undef DAP_CRTP_STD_BINARY_NODE_OPERATOR
#undef DAP_CRTP_STD_BINARY_NODE_FUNCTOR
#undef DAP_CRTP_STD_BINARY_NODE_COMPARE_OPERATOR
