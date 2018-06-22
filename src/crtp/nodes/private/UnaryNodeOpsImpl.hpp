#define DAP_CRTP_STD_UNARY_NODE(func, name_struct)                     \
    struct name_struct                                                 \
    {                                                                  \
        template <typename T>                                          \
        auto operator()(T t) const noexcept                            \
        {                                                              \
            return std::func(t);                                       \
        }                                                              \
    };                                                                 \
    template <typename TInput>                                         \
    using name_struct##Node = UnaryNode<name_struct, TInput>;          \
                                                                       \
    template <typename D, typename I>                                  \
    inline constexpr auto func(NodeExpression<D, I> n) noexcept        \
    {                                                                  \
        return name_struct##Node<D>{n.self()};                         \
    }                                                                  \
    template <typename D, typename I>                                  \
    inline constexpr auto func(NodeExpression<D, I>* n) noexcept       \
    {                                                                  \
        return name_struct##Node<D*>{n->self()};                       \
    }                                                                  \
    template <typename E>                                              \
    inline constexpr auto func(fastmath::BaseExpression<E> e) noexcept \
    {                                                                  \
        return name_struct##Node<E>{e.self()};                         \
    }                                                                  \
    template <typename T, DAP_REQUIRES(isArithmetic<T>())>             \
    inline constexpr auto func(T t) noexcept                           \
    {                                                                  \
        return name_struct##Node<T>{t};                                \
    }

namespace dap
{
    namespace crtp
    {
        template <typename Fn, typename TInput>
        using UnaryNode =
            ProcessorNode<Fn, Node::Inputs<TInput>, decltype(make_input_names("x"_s))>;

        struct Sqr
        {
            template <typename T>
            auto operator()(T t) const noexcept
            {
                return t*t;
            }
        };
        struct Neg
        {
            template <typename T>
            auto operator()(T t) const noexcept
            {
                return -t;
            }
        };

        template <typename TInput>
        using SqrNode = UnaryNode<Sqr, TInput>;

        template <typename TInput>
        using NegNode = UnaryNode<Neg, TInput>;

        template <typename D, typename I>
        inline constexpr auto sqr(NodeExpression<D, I> n) noexcept
        {
            return SqrNode<D>{n.self()};
        }
        template <typename E>
        inline constexpr auto sqr(fastmath::BaseExpression<E> e) noexcept
        {
            return SqrNode<E>{e.self()};
        }
        template <typename T, DAP_REQUIRES(isArithmetic<T>())>
        inline constexpr auto sqr(T t) noexcept
        {
            return SqrNode<T>{t};
        }
        template <typename D, typename I>
        inline constexpr auto operator-(NodeExpression<D, I> n) noexcept
        {
            return NegNode<D>{n.self()};
        }

        DAP_CRTP_STD_UNARY_NODE(abs, Abs)
        DAP_CRTP_STD_UNARY_NODE(fabs, Fabs)
        DAP_CRTP_STD_UNARY_NODE(isnan, Isnan)
        DAP_CRTP_STD_UNARY_NODE(isinf, Isinf)
        DAP_CRTP_STD_UNARY_NODE(isfinite, Isfinite)
        DAP_CRTP_STD_UNARY_NODE(isnormal, Isnormal)
        DAP_CRTP_STD_UNARY_NODE(signbit, Signbit)
        DAP_CRTP_STD_UNARY_NODE(ceil, Ceil)
        DAP_CRTP_STD_UNARY_NODE(floor, Floor)
        DAP_CRTP_STD_UNARY_NODE(trunc, Trunc)
        DAP_CRTP_STD_UNARY_NODE(exp, Exp)
        DAP_CRTP_STD_UNARY_NODE(exp2, Exp2)
        DAP_CRTP_STD_UNARY_NODE(expm1, Expm1)
        DAP_CRTP_STD_UNARY_NODE(log, Log)
        DAP_CRTP_STD_UNARY_NODE(log10, Log10)
        DAP_CRTP_STD_UNARY_NODE(log2, Log2)
        DAP_CRTP_STD_UNARY_NODE(log1p, Log1p)
        DAP_CRTP_STD_UNARY_NODE(sqrt, Sqrt)
        DAP_CRTP_STD_UNARY_NODE(cbrt, Cbrt)
        DAP_CRTP_STD_UNARY_NODE(sin, Sin)
        DAP_CRTP_STD_UNARY_NODE(cos, Cos)
        DAP_CRTP_STD_UNARY_NODE(tan, Tan)
        DAP_CRTP_STD_UNARY_NODE(asin, Asin)
        DAP_CRTP_STD_UNARY_NODE(acos, Acos)
        DAP_CRTP_STD_UNARY_NODE(atan, Atan)
        DAP_CRTP_STD_UNARY_NODE(atan2, Atan2)
        DAP_CRTP_STD_UNARY_NODE(sinh, Sinh)
        DAP_CRTP_STD_UNARY_NODE(cosh, Cosh)
        DAP_CRTP_STD_UNARY_NODE(tanh, Tanh)
        DAP_CRTP_STD_UNARY_NODE(asinh, Asinh)
        DAP_CRTP_STD_UNARY_NODE(acosh, Acosh)
        DAP_CRTP_STD_UNARY_NODE(atanh, Atanh)
        DAP_CRTP_STD_UNARY_NODE(real, Real)
        DAP_CRTP_STD_UNARY_NODE(imag, Imag)
        DAP_CRTP_STD_UNARY_NODE(conj, Conj)
    } // namespace crtp
} // namespace dap

#undef DAP_CRTP_STD_UNARY_NODE
