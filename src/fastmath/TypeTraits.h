#ifndef DAP_FASTMATH_TYPE_TRAITS_H
#define DAP_FASTMATH_TYPE_TRAITS_H

// Collection of compile time utilities

#include "base/TypeTraits.h"
#include "Variable.h"

namespace dap
{
    namespace fastmath
    {
        template <typename, typename>
        constexpr bool isArithmetic()
        {
            return false;
        }
        template <typename T, DAP_REQUIRES(dap::isArithmetic<T>())>
        constexpr bool isArithmetic()
        {
            return true;
        }
        template <typename T, DAP_REQUIRES(isSame<T, Variable<value_type<T>>>())>
        constexpr bool isArithmetic()
        {
            return true;
        }

        template <typename, typename>
        constexpr bool isFloatingPoint()
        {
            return false;
        }
        template <typename T, DAP_REQUIRES(dap::isFloatingPoint<T>())>
        constexpr bool isFloatingPoint()
        {
            return true;
        }
        template <typename T, DAP_REQUIRES(isSame<T, Variable<value_type<T>>>())>
        constexpr bool isFloatingPoint()
        {
            return dap::isFloatingPoint<typename T::value_type>();
        }

        template <typename>
        struct isBaseExpression : std::false_type
        {
        };
        template <typename...Ts>
        struct isBaseExpression<Variable<Ts...>> : std::true_type
        {
        };
        template <typename...Ts>
        struct isBaseExpression<UnaryExpression<Ts...>> : std::true_type
        {
        };
        template <typename...Ts>
        struct isBaseExpression<BinaryExpression<Ts...>> : std::true_type
        {
        };
    }
}

#endif // DAP_FASTMATH_TYPE_TRAITS_H
