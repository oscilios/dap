namespace dap
{
    namespace fastmath
    {
        ///////////////////////////////////////////////////////////////////////
        // Binary Expression
        ///////////////////////////////////////////////////////////////////////
        template <typename Op, typename Lhs, typename Rhs>
        struct BinaryExpression : public BaseExpression<BinaryExpression<Op, Lhs, Rhs> >
        {
            BinaryExpression(Lhs lhs, Rhs rhs) noexcept : m_lhs(lhs), m_rhs(rhs)
            {
            }

            auto operator()() const noexcept
            {
                return m_op(m_lhs(), m_rhs());
            }

            template <typename E, DAP_REQUIRES(!isArithmetic<E>())>
            auto operator()(E e) const noexcept
            {
                return m_op(m_lhs(e()), m_rhs(e()));
            }

            template <typename T, DAP_REQUIRES(isArithmetic<T>())>
            auto operator()(T n) const noexcept
            {
                return m_op(m_lhs(n), m_rhs(n));
            }
            // conversion operator
            template <typename T, DAP_REQUIRES(isArithmetic<T>())>
            operator T() const noexcept
            {
                return static_cast<T>((*this)());
            }

        private:
            Lhs m_lhs;
            Rhs m_rhs;
            Op m_op;
        };

        ///////////////////////////////////////////////////////////////////////
        // Operators
        ///////////////////////////////////////////////////////////////////////

        struct AddOp
        {
            template <typename T, typename U>
            auto operator()(T t, U u) const noexcept
            {
                return t + u;
            }
        };
        struct SubOp
        {
            template <typename T, typename U>
            auto operator()(T t, U u) const noexcept
            {
                return t - u;
            }
        };
        struct MulOp
        {
            template <typename T, typename U>
            auto operator()(T t, U u) const noexcept
            {
                return t * u;
            }
        };
        struct DivOp
        {
            template <typename T, typename U>
            auto operator()(T t, U u) const noexcept
            {
                return t / u;
            }
        };
        struct PowOp
        {
            template <typename T, typename U>
            auto operator()(T t, U u) const noexcept
            {
                return std::pow(t, u);
            }
        };
        struct HypotOp
        {
            template <typename T, typename U>
            auto operator()(T t, U u) const noexcept
            {
                return std::hypot(t, u);
            }
        };
        struct EqualOp
        {
            template <typename T, typename U>
            auto operator()(T t, U u) const noexcept
            {
                return t == u;
            }
        };
        struct NotEqualOp
        {
            template <typename T, typename U>
            auto operator()(T t, U u) const noexcept
            {
                return t != u;
            }
        };
        struct LessOp
        {
            template <typename T, typename U>
            auto operator()(T t, U u) const noexcept
            {
                return t < u;
            }
        };
        struct GreaterOp
        {
            template <typename T, typename U>
            auto operator()(T t, U u) const noexcept
            {
                return t > u;
            }
        };
        struct LessOrEqualOp
        {
            template <typename T, typename U>
            auto operator()(T t, U u) const noexcept
            {
                return t <= u;
            }
        };
        struct GreaterOrEqualOp
        {
            template <typename T, typename U>
            auto operator()(T t, U u) const noexcept
            {
                return t >= u;
            }
        };
        struct MaxOp
        {
            template <typename T>
            auto operator()(T x, T y) const noexcept
            {
                return std::max(x, y);
            }
        };
        struct MinOp
        {
            template <typename T>
            auto operator()(T x, T y) const noexcept
            {
                return std::min(x, y);
            }
        };

        // addition
        template <typename E1, typename E2>
        inline constexpr auto operator+(BaseExpression<E1> const& e1,
                                        BaseExpression<E2> const& e2) noexcept
        {
            return BinaryExpression<AddOp, E1, E2>{e1.self(), e2.self()};
        }
        template <typename E, typename Literal, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto operator+(BaseExpression<E> const& e1, Literal const& e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<AddOp, E, Var>{e1.self(), Var(e2)};
        }
        template <typename Literal, typename E, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto operator+(Literal e1, BaseExpression<E> const& e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<AddOp, Var, E>{Var(e1), e2.self()};
        }

        // subtraction
        template <typename E1, typename E2>
        inline constexpr auto operator-(BaseExpression<E1> const& e1,
                                        BaseExpression<E2> const& e2) noexcept
        {
            return BinaryExpression<SubOp, E1, E2>{e1.self(), e2.self()};
        }
        template <typename E, typename Literal, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto operator-(BaseExpression<E> const& e1, Literal e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<SubOp, E, Var>{e1.self(), Var(e2)};
        }
        template <typename Literal, typename E, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto operator-(Literal e1, BaseExpression<E> const& e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<SubOp, Var, E>{Var(e1), e2.self()};
        }

        // product
        template <typename E1, typename E2>
        inline constexpr auto operator*(BaseExpression<E1> const& e1,
                                        BaseExpression<E2> const& e2) noexcept
        {
            return BinaryExpression<MulOp, E1, E2>{e1.self(), e2.self()};
        }
        template <typename E, typename Literal, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto operator*(BaseExpression<E> const& e1, Literal e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<MulOp, E, Var>{e1.self(), Var(e2)};
        }
        template <typename Literal, typename E, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto operator*(Literal e1, BaseExpression<E> const& e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<MulOp, Var, E>{Var(e1), e2.self()};
        }

        // division
        template <typename E1, typename E2>
        inline constexpr auto operator/(BaseExpression<E1> const& e1,
                                        BaseExpression<E2> const& e2) noexcept
        {
            return BinaryExpression<DivOp, E1, E2>{e1.self(), e2.self()};
        }
        template <typename E, typename Literal, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto operator/(BaseExpression<E> const& e1, Literal e2)
        {
            using Var = Variable<Literal>;
            return BinaryExpression<DivOp, E, Var>{e1.self(), Var(e2)};
        }
        template <typename Literal, typename E, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto operator/(Literal e1, BaseExpression<E> const& e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<DivOp, Var, E>{Var(e1), e2.self()};
        }

        // power
        template <typename E1, typename E2>
        inline constexpr auto pow(BaseExpression<E1> const& e1,
                                  BaseExpression<E2> const& e2) noexcept
        {
            return BinaryExpression<PowOp, E1, E2>{e1.self(), e2.self()};
        }
        template <typename E, typename Literal, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto pow(BaseExpression<E> const& e1, Literal e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<PowOp, E, Var>{e1.self(), Var(e2)};
        }
        template <typename Literal, typename E, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto pow(Literal e1, BaseExpression<E> const& e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<PowOp, Var, E>{Var(e1), e2.self()};
        }

        // hypot
        template <typename E1, typename E2>
        inline constexpr auto hypot(BaseExpression<E1> const& e1,
                                    BaseExpression<E2> const& e2) noexcept
        {
            return BinaryExpression<HypotOp, E1, E2>{e1.self(), e2.self()};
        }
        template <typename E, typename Literal, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto hypot(BaseExpression<E> const& e1, Literal e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<HypotOp, E, Var>{e1.self(), Var(e2)};
        }
        template <typename Literal, typename E, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto hypot(Literal e1, BaseExpression<E> const& e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<HypotOp, Var, E>{Var(e1), e2.self()};
        }

        // equal
        template <typename E1, typename E2>
        inline constexpr auto operator==(BaseExpression<E1> const& e1,
                                         BaseExpression<E2> const& e2) noexcept
        {
            return BinaryExpression<EqualOp, E1, E2>{e1.self(), e2.self()}();
        }
        template <typename E, typename Literal, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto operator==(BaseExpression<E> const& e1, Literal e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<EqualOp, E, Var>{e1.self(), Var(e2)}();
        }
        template <typename Literal, typename E, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto operator==(Literal e1, BaseExpression<E> const& e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<EqualOp, Var, E>{Var(e1), e2.self()}();
        }
        // not equal
        template <typename E1, typename E2>
        inline constexpr auto operator!=(BaseExpression<E1> const& e1,
                                         BaseExpression<E2> const& e2) noexcept
        {
            return BinaryExpression<NotEqualOp, E1, E2>{e1.self(), e2.self()}();
        }
        template <typename E, typename Literal, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto operator!=(BaseExpression<E> const& e1, Literal e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<NotEqualOp, E, Var>{e1.self(), Var(e2)}();
        }
        template <typename Literal, typename E, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto operator!=(Literal e1, BaseExpression<E> const& e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<NotEqualOp, Var, E>{Var(e1), e2.self()}();
        }
        // Less than
        template <typename E1, typename E2>
        inline constexpr auto operator<(BaseExpression<E1> const& e1,
                                        BaseExpression<E2> const& e2) noexcept
        {
            return BinaryExpression<LessOp, E1, E2>{e1.self(), e2.self()}();
        }
        template <typename E, typename Literal, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto operator<(BaseExpression<E> const& e1, Literal e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<LessOp, E, Var>{e1.self(), Var(e2)}();
        }
        template <typename Literal, typename E, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto operator<(Literal e1, BaseExpression<E> const& e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<LessOp, Var, E>{Var(e1), e2.self()}();
        }
        // greater than
        template <typename E1, typename E2>
        inline constexpr auto operator>(BaseExpression<E1> const& e1,
                                        BaseExpression<E2> const& e2) noexcept
        {
            return BinaryExpression<GreaterOp, E1, E2>{e1.self(), e2.self()}();
        }
        template <typename E, typename Literal, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto operator>(BaseExpression<E> const& e1, Literal e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<GreaterOp, E, Var>{e1.self(), Var(e2)}();
        }
        template <typename Literal, typename E, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto operator>(Literal e1, BaseExpression<E> const& e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<GreaterOp, Var, E>{Var(e1), e2.self()}();
        }
        // Less than or equal
        template <typename E1, typename E2>
        inline constexpr auto operator<=(BaseExpression<E1> const& e1,
                                         BaseExpression<E2> const& e2) noexcept
        {
            return BinaryExpression<LessOrEqualOp, E1, E2>{e1.self(), e2.self()}();
        }
        template <typename E, typename Literal, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto operator<=(BaseExpression<E> const& e1, Literal e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<LessOrEqualOp, E, Var>{e1.self(), Var(e2)}();
        }
        template <typename Literal, typename E, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto operator<=(Literal e1, BaseExpression<E> const& e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<LessOrEqualOp, Var, E>{Var(e1), e2.self()}();
        }
        // greater than or equal
        template <typename E1, typename E2>
        inline constexpr auto operator>=(BaseExpression<E1> const& e1,
                                         BaseExpression<E2> const& e2) noexcept
        {
            return BinaryExpression<GreaterOrEqualOp, E1, E2>{e1.self(), e2.self()}();
        }
        template <typename E, typename Literal, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto operator>=(BaseExpression<E> const& e1, Literal e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<GreaterOrEqualOp, E, Var>{e1.self(), Var(e2)}();
        }
        template <typename Literal, typename E, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto operator>=(Literal e1, BaseExpression<E> const& e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<GreaterOrEqualOp, Var, E>{Var(e1), e2.self()}();
        }
        // max
        template <typename E1, typename E2>
        inline constexpr auto max(BaseExpression<E1> const& e1,
                                  BaseExpression<E2> const& e2) noexcept
        {
            return BinaryExpression<MaxOp, E1, E2>{e1.self(), e2.self()}();
        }
        template <typename E, typename Literal, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto max(BaseExpression<E> const& e1, Literal e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<MaxOp, E, Var>{e1.self(), Var(e2)}();
        }
        template <typename Literal, typename E, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto max(Literal e1, BaseExpression<E> const& e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<MaxOp, Var, E>{Var(e1), e2.self()}();
        }
        // min
        template <typename E1, typename E2>
        inline constexpr auto min(BaseExpression<E1> const& e1,
                                  BaseExpression<E2> const& e2) noexcept
        {
            return BinaryExpression<MinOp, E1, E2>{e1.self(), e2.self()}();
        }
        template <typename E, typename Literal, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto min(BaseExpression<E> const& e1, Literal e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<MinOp, E, Var>{e1.self(), Var(e2)}();
        }
        template <typename Literal, typename E, DAP_REQUIRES(isArithmetic<Literal>())>
        inline constexpr auto min(Literal e1, BaseExpression<E> const& e2) noexcept
        {
            using Var = Variable<Literal>;
            return BinaryExpression<MinOp, Var, E>{Var(e1), e2.self()}();
        }

        // TODO (eaylon): add bit operators (&, |, ^...)
    } // namespace fastmath
} // namespace dap
