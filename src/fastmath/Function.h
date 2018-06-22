#ifndef DAP_FASTMATH_FUNCTION_H
#define DAP_FASTMATH_FUNCTION_H

#include <tuple>
namespace dap
{
    namespace fastmath
    {
        template <class... Ops>
        struct Function
        {
            // primary template, never used
        };

        template <class Op, class... Ops>
        struct Function<Op, Ops...> : Function<Ops...>
        {
            template <typename... Xs>
            auto operator()(Xs... x) const noexcept
            {
                Function<Ops...> f;
                return f(m_op(x...));
            }
        private:
            Op m_op;
        };
        template <class Op>
        struct Function<Op>
        {
            template <typename... Xs>
            auto operator()(Xs... x) const noexcept
            {
                return m_op(x...);
            }
        private:
            Op m_op;
        };
    } // fastmath
} // dap
#endif // DAP_FASTMATH_FUNCTION_H
