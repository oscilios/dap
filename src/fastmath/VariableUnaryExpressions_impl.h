namespace dap
{
    namespace fastmath
    {
        ///////////////////////////////////////////////////////////////////////
        // Unary Expression
        ///////////////////////////////////////////////////////////////////////

        template <typename Op, typename Rhs>
        struct UnaryExpression : public BaseExpression<UnaryExpression<Op, Rhs> >
        {
            explicit UnaryExpression(Rhs rhs)
            : m_rhs(rhs)
            {
            }

            auto operator()() const
            {
                return m_op(m_rhs());
            }

        private:
            Rhs m_rhs;
            Op m_op;
        };

        // Basic operations
        struct NegOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return -t;
            }
        };
        struct AbsOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::abs(t);
            }
        };
        struct FabsOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::fabs(t);
            }
        };
        struct IsnanOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::isnan(t);
            }
        };
        struct IsinfOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::isinf(t);
            }
        };
        struct IsfiniteOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::isfinite(t);
            }
        };
        struct IsnormalOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::isnormal(t);
            }
        };
        struct SignbitOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::signbit(t);
            }
        };
        struct CeilOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::ceil(t);
            }
        };
        struct FloorOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::floor(t);
            }
        };
        struct TruncOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::trunc(t);
            }
        };
        // Exponential operations
        struct ExpOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::exp(t);
            }
        };
        struct Exp2Op
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::exp2(t);
            }
        };
        struct Expm1Op
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::expm1(t);
            }
        };
        struct LnOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::log(t);
            }
        };
        struct LogOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::log10(t);
            }
        };
        struct Log2Op
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::log2(t);
            }
        };
        struct Log1pOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::log1p(t);
            }
        };

        // Power Functions
        struct SqrtOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::sqrt(t);
            }
        };
        struct CbrtOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::cbrt(t);
            }
        };

        // Trigonometric Functions
        struct SinOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::sin(t);
            }
        };
        struct CosOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::cos(t);
            }
        };
        struct TanOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::tan(t);
            }
        };
        struct AsinOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::asin(t);
            }
        };
        struct AcosOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::acos(t);
            }
        };
        struct AtanOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::atan(t);
            }
        };
        struct Atan2Op
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::atan2(t);
            }
        };

        // Hyperbolic functions
        struct SinhOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::sinh(t);
            }
        };
        struct CoshOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::cosh(t);
            }
        };
        struct TanhOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::tanh(t);
            }
        };
        struct AsinhOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::asinh(t);
            }
        };
        struct AcoshOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::acosh(t);
            }
        };
        struct AtanhOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::atanh(t);
            }
        };

        // complex functions
        struct RealOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::real(t);
            }
        };
        struct ImagOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::imag(t);
            }
        };
        struct ConjOp
        {
            template <typename T>
            auto operator()(T t) const
            {
                return std::conj(t);
            }
        };
        struct Abs2Op
        {
            template <typename T>
            auto operator()(T t) const
            {
                return t * t;
            }
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // specialization for arithmethic types
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto operator-(const E& e) noexcept
        {
            return NegOp()(e);
        }
        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto abs(const E& e) noexcept
        {
            return AbsOp()(e);
        }
        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto fabs(const E& e) noexcept
        {
            return FabsOp()(e);
        }
        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto isnan(const E& e) noexcept
        {
            return IsnanOp()(e);
        }
        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto isinf(const E& e) noexcept
        {
            return IsinfOp()(e);
        }
        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto isfinite(const E& e) noexcept
        {
            return IsfiniteOp()(e);
        }
        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto isnormal(const E& e) noexcept
        {
            return IsnormalOp()(e);
        }
        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto signbit(const E& e) noexcept
        {
            return SignbitOp()(e);
        }
        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto ceil(const E& e) noexcept
        {
            return CeilOp()(e);
        }
        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto floor(const E& e) noexcept
        {
            return FloorOp()(e);
        }
        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto trunc(const E& e) noexcept
        {
            return TruncOp()(e);
        }
        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto exp(const E& e) noexcept
        {
            return ExpOp()(e);
        }
        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto exp2(const E& e) noexcept
        {
            return Exp2Op()(e);
        }
        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto expm1(const E& e) noexcept
        {
            return Expm1Op()(e);
        }

        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto log(const E& e) noexcept
        {
            return LnOp()(e);
        }

        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto log10(const E& e) noexcept
        {
            return LogOp()(e);
        }

        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto log2(const E& e) noexcept
        {
            return Log2Op()(e);
        }

        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto log1p(const E& e) noexcept
        {
            return Log1pOp()(e);
        }

        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto sqrt(const E& e) noexcept
        {
            return SqrtOp()(e);
        }
        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto cbrt(const E& e) noexcept
        {
            return CbrtOp()(e);
        }

        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto sin(const E& e) noexcept
        {
            return SinOp()(e);
        }

        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto cos(const E& e) noexcept
        {
            return CosOp()(e);
        }

        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto tan(const E& e) noexcept
        {
            return TanOp()(e);
        }

        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto asin(const E& e) noexcept
        {
            return AsinOp()(e);
        }

        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto acos(const E& e) noexcept
        {
            return AcosOp()(e);
        }

        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto atan(const E& e) noexcept
        {
            return AtanOp()(e);
        }

        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto atan2(const E& e) noexcept
        {
            return Atan2Op()(e);
        }

        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto sinh(const E& e) noexcept
        {
            return SinhOp()(e);
        }

        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto cosh(const E& e) noexcept
        {
            return CoshOp()(e);
        }

        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto tanh(const E& e) noexcept
        {
            return TanhOp()(e);
        }

        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto asinh(const E& e) noexcept
        {
            return AsinhOp()(e);
        }

        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto acosh(const E& e) noexcept
        {
            return AcoshOp()(e);
        }

        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto atanh(const E& e) noexcept
        {
            return AtanhOp()(e);
        }
        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto real(const E& e) noexcept
        {
            return RealOp()(e);
        }
        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto imag(const E& e) noexcept
        {
            return ImagOp()(e);
        }
        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto conj(const E& e) noexcept
        {
            return ConjOp()(e);
        }
        template <typename E, DAP_REQUIRES(dap::isArithmetic<E>())>
        inline constexpr auto abs2(const E& e) noexcept
        {
            return Abs2Op()(e);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Basic functions
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename E>
        inline constexpr auto operator-(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<NegOp, E>{e.self()};
        }
        template <typename E>
        inline constexpr auto abs(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<AbsOp, E>{e.self()};
        }
        template <typename E>
        inline constexpr auto fabs(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<FabsOp, E>{e.self()};
        }
        template <typename E>
        inline constexpr auto isnan(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<IsnanOp, E>{e.self()};
        }
        template <typename E>
        inline constexpr auto isinf(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<IsinfOp, E>{e.self()};
        }
        template <typename E>
        inline constexpr auto isfinite(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<IsfiniteOp, E>{e.self()};
        }
        template <typename E>
        inline constexpr auto isnormal(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<IsnormalOp, E>{e.self()};
        }
        template <typename E>
        inline constexpr auto signbit(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<SignbitOp, E>{e.self()};
        }
        template <typename E>
        inline constexpr auto ceil(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<CeilOp, E>{e.self()};
        }
        template <typename E>
        inline constexpr auto floor(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<FloorOp, E>{e.self()};
        }
        template <typename E>
        inline constexpr auto trunc(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<TruncOp, E>{e.self()};
        }
        // Exponential functions
        template <typename E>
        inline constexpr auto exp(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<ExpOp, E>{e.self()};
        }
        template <typename E>
        inline constexpr auto exp2(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<Exp2Op, E>{e.self()};
        }
        template <typename E>
        inline constexpr auto expm1(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<Expm1Op, E>{e.self()};
        }

        template <typename E>
        inline constexpr auto log(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<LnOp, E>{e.self()};
        }

        template <typename E>
        inline constexpr auto log10(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<LogOp, E>{e.self()};
        }

        template <typename E>
        inline constexpr auto log2(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<Log2Op, E>{e.self()};
        }

        template <typename E>
        inline constexpr auto log1p(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<Log1pOp, E>{e.self()};
        }

        // Power Functions
        template <typename E>
        inline constexpr auto sqrt(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<SqrtOp, E>{e.self()};
        }
        template <typename E>
        inline constexpr auto cbrt(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<CbrtOp, E>{e.self()};
        }

        // Trigonometric functions
        template <typename E>
        inline constexpr auto sin(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<SinOp, E>{e.self()};
        }

        template <typename E>
        inline constexpr auto cos(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<CosOp, E>{e.self()};
        }

        template <typename E>
        inline constexpr auto tan(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<TanOp, E>{e.self()};
        }

        template <typename E>
        inline constexpr auto asin(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<AsinOp, E>{e.self()};
        }

        template <typename E>
        inline constexpr auto acos(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<AcosOp, E>{e.self()};
        }

        template <typename E>
        inline constexpr auto atan(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<AtanOp, E>{e.self()};
        }

        template <typename E>
        inline constexpr auto atan2(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<Atan2Op, E>{e.self()};
        }

        // Hyperbolic functions
        template <typename E>
        inline constexpr auto sinh(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<SinhOp, E>{e.self()};
        }

        template <typename E>
        inline constexpr auto cosh(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<CoshOp, E>{e.self()};
        }

        template <typename E>
        inline constexpr auto tanh(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<TanhOp, E>{e.self()};
        }

        template <typename E>
        inline constexpr auto asinh(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<AsinhOp, E>{e.self()};
        }

        template <typename E>
        inline constexpr auto acosh(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<AcoshOp, E>{e.self()};
        }

        template <typename E>
        inline constexpr auto atanh(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<AtanhOp, E>{e.self()};
        }
        // complex functions
        template <typename E>
        inline constexpr auto real(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<RealOp, E>{e.self()};
        }
        template <typename E>
        inline constexpr auto imag(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<ImagOp, E>{e.self()};
        }
        template <typename E>
        inline constexpr auto conj(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<ConjOp, E>{e.self()};
        }
        template <typename E>
        inline constexpr auto abs2(const BaseExpression<E>& e) noexcept
        {
            return UnaryExpression<Abs2Op, E>{e.self()};
        }
    } // namespace fastmath
} // namespace dap
