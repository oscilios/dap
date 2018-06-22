#ifndef DAP_TAYLOR_IMPL_H
#define DAP_TAYLOR_IMPL_H

#include "base/TypeTraits.h"

namespace dap
{
    namespace taylor
    {
        namespace detail
        {
            // sine
            template <typename T, DAP_REQUIRES(isFloatingPoint<T>())>
            constexpr T sine_mult(T x, size_t idx)
            {
                return -x * x / T((2 * idx + 1) * 2 * idx);
            }

            template <typename T, DAP_REQUIRES(isFloatingPoint<T>())>
            constexpr T sine_impl(T x, T cur, size_t idx, size_t size)
            {
                return idx == 0 ? cur :
                                  idx < size - 1 ?
                                  cur * sine_mult(x, idx) +
                                          sine_impl(x, cur * sine_mult(x, idx), idx + 1, size) :
                                  cur;
            }

            template <typename T, DAP_REQUIRES(isFloatingPoint<T>())>
            constexpr T sine(T x, size_t size)
            {
                return sine_impl(x, x, 0, size) + sine_impl(x, x, 1, size);
            }

            template <class T, std::size_t Precision, std::size_t... Is>
            constexpr std::array<T, sizeof...(Is)>
                sine_array_impl(dap::detail::sequence<Is...> /*unused*/)
            {
                return {{sine(T{T{2.0 * M_PI * Is} / T{sizeof...(Is)}}, Precision)...}};
            }

            // cosine
            template <typename T, DAP_REQUIRES(isFloatingPoint<T>())>
            constexpr T cosine_mult(T x, size_t idx)
            {
                return -x * x / T((2 * idx - 1) * 2 * idx);
            }

            template <typename T, DAP_REQUIRES(isFloatingPoint<T>())>
            constexpr T cosine_impl(T x, T cur, size_t idx, size_t size)
            {
                return idx == 0 ? 1 :
                                  idx < size - 1 ?
                                  cur * cosine_mult(x, idx) +
                                          cosine_impl(x, cur * cosine_mult(x, idx), idx + 1, size) :
                                  cur;
            }

            template <typename T, DAP_REQUIRES(isFloatingPoint<T>())>
            constexpr T cosine(T x, size_t size)
            {
                return cosine_impl(x, x, 0, size) + cosine_impl(x, T{1}, 1, size);
            }

            template <class T, std::size_t Precision, std::size_t... Is>
            constexpr std::array<T, sizeof...(Is)>
                cosine_array_impl(dap::detail::sequence<Is...> /*unused*/)
            {
                return {{cosine(T{T{2.0 * M_PI * Is} / T{sizeof...(Is)}}, Precision)...}};
            }

            // exp for x [0, 1)
            template <typename T, DAP_REQUIRES(isFloatingPoint<T>())>
            constexpr T exp_mult(T x, size_t idx)
            {
                return x / T(idx);
            }

            template <typename T, DAP_REQUIRES(isFloatingPoint<T>())>
            constexpr T exp_impl(T x, T cur, size_t idx, size_t size)
            {
                return idx == 0 ?
                           1 :
                           idx == 1 ? x :
                                      idx < size - 1 ?
                                      cur * exp_mult(x, idx) +
                                              exp_impl(x, cur * exp_mult(x, idx), idx + 1, size) :
                                      cur;
            }

            template <typename T, DAP_REQUIRES(isFloatingPoint<T>())>
            constexpr T exp(T x, size_t size)
            {
                return exp_impl(x, x, 0, size) + exp_impl(x, T{1}, 1, size) +
                       exp_impl(x, x, 2, size);
            }

            template <class T, std::size_t Precision, std::size_t... Is>
            constexpr std::array<T, sizeof...(Is)>
            exp_array_impl(T Start, T End, dap::detail::sequence<Is...> /*unused*/)
            {
                return {
                    {exp(T{T{Start} + T{End - Start} * T{Is} / T{sizeof...(Is)}}, Precision)...}};
            }
        } // detail

        // Precision determines the number of factors of the expansion series
        template <class T,
                  std::size_t N,
                  std::size_t Precision = 31,
                  DAP_REQUIRES(isFloatingPoint<T>())>
        constexpr std::array<T, N> sine_array()
        {
            return detail::sine_array_impl<T, Precision>(dap::detail::make_sequence<N>{});
        }
        template <class T,
                  std::size_t N,
                  std::size_t Precision = 31,
                  DAP_REQUIRES(isFloatingPoint<T>())>
        constexpr std::array<T, N> cosine_array()
        {
            return detail::cosine_array_impl<T, Precision>(dap::detail::make_sequence<N>{});
        }
        template <std::size_t N,
                  std::size_t Precision = 31, // precision needs to be doubled as 31*2^n where n is
                                              // max(log10(Start), log10(End))
                  class T = float,
                  DAP_REQUIRES(isFloatingPoint<T>())>
        constexpr std::array<T, N> exp_array(T Start, T End)
        {
            return detail::exp_array_impl<T, Precision>(
                Start, End, dap::detail::make_sequence<N>{});
        }
    } // namespace taylor
} // namespace dap

#endif // DAP_TAYLOR_IMPL_H
