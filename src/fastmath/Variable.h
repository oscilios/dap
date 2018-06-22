#ifndef DAP_FASTMATH_VARIABLE_H
#define DAP_FASTMATH_VARIABLE_H

#include "base/Streamable.h"
#include "base/SystemAnnotations.h"
#include "base/TypeTraits.h"
#include <cmath>
#include <limits>

namespace dap
{
    namespace fastmath
    {
        template <typename Derived>
        struct BaseExpression
        {
            Derived const& self() const
            {
                return static_cast<const Derived&>(*this);
            }
            auto operator()() const noexcept
            {
                return self()();
            }
        };

        template <typename BaseType>
        struct Variable : public BaseExpression<Variable<BaseType> >
        {
            static_assert(isArithmeticOrComplex<BaseType>(),
                          "Variable<T>  does not support non arithmetic types");
            using value_type = BaseType;

            Variable() = default;
            template <typename T, DAP_REQUIRES(isArithmetic<T>())>
            DAP_NO_EXPLICIT_CTOR Variable(const T& value) noexcept
            : m_value(value)
            {
            }
            template <typename T, DAP_REQUIRES(isArithmetic<T>())>
            DAP_NO_EXPLICIT_CTOR Variable(T&& value) noexcept  // NOLINT [misc-forwarding-reference-overload]
            : m_value(value)
            {
            }
            template <typename E>
            DAP_NO_EXPLICIT_CTOR Variable(BaseExpression<E>&& expr) noexcept
            : m_value(std::move(expr()))
            {
            }
            template <typename T, DAP_REQUIRES(isArithmetic<T>())>
            Variable operator=(const T& value) noexcept // NOLINT intentionally return copy, not ref
            {
                m_value = value;
                return *this;
            }
            template <typename T, DAP_REQUIRES(isArithmetic<T>())>
            Variable operator=(T&& value) noexcept // NOLINT, intentionally return copy, not ref
            {
                m_value = std::forward<T>(value);
                return *this;
            }
            template <typename E>
            Variable operator=(BaseExpression<E> const& expr) noexcept // NOLINT, intentionally
                                                                       // return copy, not ref
            {
                m_value = expr();
                return *this;
            }
            auto operator()() const noexcept
            {
                return m_value;
            }
            auto operator()(const Variable& n) const noexcept
            {
                return n.m_value;
            }
            template <typename T, DAP_REQUIRES(isArithmetic<T>())>
            operator T() const noexcept
            {
                return T(m_value);
            }

            // math
            template <typename E>
            Variable& operator+=(BaseExpression<E> const& expr) noexcept
            {
                m_value += expr();
                return *this;
            }
            template <typename T, DAP_REQUIRES(isArithmetic<T>())>
            Variable& operator+=(T x) noexcept
            {
                m_value += x;
                return *this;
            }
            template <typename E>
            Variable& operator-=(BaseExpression<E> const& expr) noexcept
            {
                m_value -= expr();
                return *this;
            }
            template <typename T, DAP_REQUIRES(isArithmetic<T>())>
            Variable& operator-=(T x) noexcept
            {
                m_value -= x;
                return *this;
            }
            template <typename E>
            Variable& operator*=(BaseExpression<E> const& expr) noexcept
            {
                m_value *= expr();
                return *this;
            }
            template <typename T, DAP_REQUIRES(isArithmetic<T>())>
            Variable& operator*=(T x) noexcept
            {
                m_value *= x;
                return *this;
            }
            template <typename E>
            Variable& operator/=(BaseExpression<E> const& expr) noexcept
            {
                m_value /= expr();
                return *this;
            }
            template <typename T, DAP_REQUIRES(isArithmetic<T>())>
            Variable& operator/=(T x) noexcept
            {
                m_value /= x;
                return *this;
            }

        private:
            value_type m_value;
        };

        template <typename T>
        std::ostream& operator<<(std::ostream& out, const Variable<T>& n)
        {
            out << n();
            return out;
        }
        template <typename E>
        std::ostream& operator<<(std::ostream& out, const BaseExpression<E>& e)
        {
            out << (e)();
            return out;
        }

        template <typename E>
        std::string exprTypeStr(const BaseExpression<E>& e)
        {
            return demangle(std::type_index(typeid(e)).name());
        }
    } // fastmath

} // namespace dap

#include "VariableBinaryExpressions_impl.h"
#include "VariableUnaryExpressions_impl.h"

#endif // DAP_FASTMATH_VARIABLE_H
