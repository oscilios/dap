#ifndef DAP_FASTMATH_VAR_H
#define DAP_FASTMATH_VAR_H

#include "Variable.h"
#include "VarArray.h"
#include "TypeTraits.h"

namespace dap
{
    template <typename T>
    using var_t = fastmath::Variable<T>;

    template <typename T>
    using var_array_t = fastmath::VarArray<var_t<T>>;

    using var  = var_t<float>;
    using uvar = var_t<size_t>;
    using ivar = var_t<int>;
    using fvar = var_t<float>;

    using var_array  = var_array_t<float>;
    using uvar_array = var_array_t<size_t>;
    using ivar_array = var_array_t<int>;
    using fvar_array = var_array_t<float>;

    namespace fastmath
    {
        template <typename T, DAP_REQUIRES(!isBaseExpression<T>())>
        inline constexpr auto dispatch(T&& value)
        {
            return value;
        }
        template <typename E>
        inline constexpr auto dispatch(BaseExpression<E>& value)
        {
            return value();
        }
        template <typename E>
        inline constexpr auto dispatch(const BaseExpression<E>& value)
        {
            return value();
        }
        template <typename E>
        inline constexpr auto dispatch(BaseExpression<E>* value)
        {
            return value->self()();
        }
        template <typename E>
        inline constexpr auto dispatch(const BaseExpression<E>* value)
        {
            return value()->self();
        }
    }

} // namespace dap

#endif // DAP_FASTMATH_VAR_H
