#ifndef DAP_BASE_VARIADIC_OPS_H
#define DAP_BASE_VARIADIC_OPS_H

// TODO: maybe move to fastmath var

#include <functional>
#include <cmath>

namespace dap
{
    template <typename Fun>
    struct VariadicOp
    {
        template <typename T, typename U>
        constexpr auto operator()(T t, U u)
        {
            return Fun()(t, u);
        }
        template <typename T, typename... Ts>
        constexpr auto operator()(T t, Ts... ts)
        {
            return Fun()(t, VariadicOp<Fun>()(ts...));
        }
    };
    struct VariadicSum
    {
        template <typename... Ts>
        constexpr auto operator()(Ts... ts)
        {
            return (... + ts);
        }
    };
    struct VariadicProd
    {
        template <typename... Ts>
        constexpr auto operator()(Ts... ts)
        {
            return (... * ts);
        }
    };
    template <typename... Ts>
    constexpr auto sum(Ts... ts)
    {
        return VariadicSum()(ts...);
    }
    template <typename... Ts>
    constexpr auto prod(Ts... ts)
    {
        return VariadicProd()(ts...);
    }
    template <typename... Ts>
    constexpr auto rms(Ts... ts)
    {
        using T = std::common_type_t<Ts...>;
        auto sqr = [](T t) { return t * t; };
        return std::sqrt(sum(sqr(ts)...));
    }
}
#endif // DAP_BASE_VARIADIC_OPS_H
