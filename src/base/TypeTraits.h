#ifndef DAP_BASE_TYPE_TRAITS_H
#define DAP_BASE_TYPE_TRAITS_H

// Collection of compile time utilities

#include <array>
#include <complex>
#include <iostream>
#include <limits>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <vector>

#define DAP_REQUIRES_T(...) typename std::enable_if<(__VA_ARGS__), int>::type
#define DAP_REQUIRES(...) DAP_REQUIRES_T(__VA_ARGS__) = 0

namespace dap
{
    template <typename... Types>
    void get_parameter_pack_type_ids_impl(std::vector<std::type_index>& v)
    {
        v.insert(v.end(), {typeid(Types)...});
    }

    template <typename... Types>
    std::vector<std::type_index> get_parameter_pack_type_ids()
    {
        std::vector<std::type_index> v;
        get_parameter_pack_type_ids_impl<Types...>(v);
        return v;
    }

    template <bool B, class T = void>
    using enable_if_t = typename std::enable_if<B, T>::type;

    template <class T = void>
    using value_type = typename T::value_type;

    template <typename... Args>
    using common_type_t = typename std::common_type<Args...>::type;

    template <typename T>
    struct Default
    {
        static const T& value() noexcept
        {
            static const T x{};
            return x;
        }
    };
    template <typename... Args>
    constexpr bool all(Args... args)
    {
        return (... && args);
    }
    template <typename... Args>
    constexpr bool any(Args... args)
    {
        return (... || args);
    }
    template <class T, std::size_t size>
    constexpr size_t array_size(T (&/*unused*/)[size])
    {
        return size;
    }
    template <class T = void, class U = void>
    constexpr bool isBaseOf()
    {
        return std::is_base_of<T, U>::value;
    }
    template <class T = void, class U = void>
    constexpr bool isSame()
    {
        return std::is_same<T, U>::value;
    }
    template <class T>
    constexpr bool isDefaultConstructible()
    {
        return std::is_default_constructible<T>::value;
    }
    template <class T>
    constexpr bool isTriviallyConstructible()
    {
        return std::is_trivially_constructible<T>::value;
    }
    template <class T = void>
    constexpr bool isArithmetic()
    {
        return std::is_arithmetic<typename std::decay<T>::type>::value;
    }
    template <class T = void>
    constexpr bool isIntegral()
    {
        return std::is_integral<T>::value;
    }
    template <class T = void>
    constexpr bool isFloatingPoint()
    {
        return std::is_floating_point<T>::value;
    }
    template <typename T>
    constexpr bool isComplex()
    {
        return false;
    }
    template <typename T, DAP_REQUIRES(isSame<T, std::complex<value_type<T>>>())>
    constexpr bool isComplex()
    {
        return true;
    }
    template <typename, typename>
    constexpr bool isArithmeticOrComplex()
    {
        return false;
    }
    template <typename T, DAP_REQUIRES(isArithmetic<T>())>
    constexpr bool isArithmeticOrComplex()
    {
        return true;
    }
    template <typename T, DAP_REQUIRES(isSame<T, std::complex<value_type<T>>>())>
    constexpr bool isArithmeticOrComplex()
    {
        return true;
    }
    template <class T = void>
    constexpr bool isStringConvertible()
    {
        return std::is_convertible<T, std::string>::value ||
               std::is_convertible<T, const char*>::value;
    }
    template <typename T>
    constexpr auto neverZero(T&& t)
    {
        return std::forward<T>(t) + std::numeric_limits<T>::epsilon();
    }

    template <size_t N>
    struct IsPowerOfTwo
    {
        enum
        {
            value = (N > 0) && (N & (N - 1)) == 0
        };
    };

    template <typename T, typename U>
    struct isAssignable
    {
        static void test(T& t, U& u)
        {
            U x = t;
            u   = t;
        }
        isAssignable()
        {
            void (*ptr)(T&, U&) = test;
        }
    };

    template <typename T, DAP_REQUIRES(isIntegral<T>())>
    constexpr bool isOdd(T n)
    {
        return n & 1;
    }

    template <typename T1,
              typename T2,
              typename T3 = common_type_t<T1, T2>,
              DAP_REQUIRES(isArithmetic<T1>() && isArithmetic<T2>() && isArithmetic<T3>())>
    constexpr bool
    isNearEqual(const T1 x, const T2 y, const T3 eps = std::numeric_limits<T3>::epsilon())
    {
        return std::fabs(x - y) <= eps;
    }
    template <typename T1,
              typename T2,
              typename T3,
              DAP_REQUIRES(isArithmetic<T1>() && isArithmetic<T2>() && isArithmetic<T3>())>
    constexpr T1 clip(T1 x, T2 low, T3 high)
    {
        return std::min(std::max(x, low), high);
    }

    template <typename T, DAP_REQUIRES(isIntegral<T>())>
    constexpr T factorial(T n)
    {
        return n > 0 ? n * factorial(n - 1) : 1;
    }

    template <class T>
    constexpr T power(const T& x, int N)
    {
        return N > 1 ? x * power(x, (N - 1) * (N > 1))
                     : N < 0 ? T(1) / power(x, (-N) * (N < 0)) : N == 1 ? x : T(1);
    }

    // sequence generation
    namespace detail
    {
        template <std::size_t... Is>
        struct sequence
        {
            static constexpr std::size_t size() noexcept
            {
                return sizeof...(Is);
            }
        };

        template <std::size_t N, std::size_t... Is>
        struct make_sequence : make_sequence<N - 1, N - 1, Is...>
        {
        };

        template <std::size_t... Is>
        struct make_sequence<0, Is...> : sequence<Is...>
        {
        };

        template <class Function, std::size_t... Is>
        constexpr auto make_array_impl(Function func, sequence<Is...> /*unused*/)
            -> std::array<decltype(func(std::size_t{}, sizeof...(Is))), sizeof...(Is)>
        {
            return {{func(Is, sizeof...(Is))...}};
        }

        constexpr int range_generator(std::size_t start, std::size_t cur, std::size_t /* size */)
        {
            return start + cur;
        }

        template <class T, std::size_t Start, std::size_t... Is>
        constexpr std::array<T, sizeof...(Is)> range_impl(detail::sequence<Is...> /*unused*/)
        {
            return {{T{Start + Is}...}};
        }

        template <class T, std::size_t... Is>
        constexpr std::array<T, sizeof...(Is)> norm_range_impl(detail::sequence<Is...> /*unused*/)
        {
            return {{T{T{Is} / sizeof...(Is)}...}};
        }

        template <class T, std::size_t... Is>
        constexpr std::array<T, sizeof...(Is)>
        linspace_impl(T Start, T End, detail::sequence<Is...> /*unused*/)
        {
            return {{T{T{Start} + T{End - Start} * T{Is} / T{sizeof...(Is) - 1}}...}};
        }
    } // detail

    // sequence generator given a function that generates values
    template <std::size_t TSize, class Function>
    constexpr auto make_array(Function func)
        -> decltype(detail::make_array_impl(func, detail::make_sequence<TSize>{}))
    {
        return detail::make_array_impl(func, detail::make_sequence<TSize>{});
    }

    // range [Start,End)
    template <typename TType = std::size_t,
              std::size_t Start,
              std::size_t End,
              DAP_REQUIRES(End >= Start)>
    constexpr std::array<TType, End - Start> irange()
    {
        return detail::range_impl<TType, Start>(detail::make_sequence<End - Start>{});
    }

    // normalized range sequence [0, 1)
    template <typename TType = float, std::size_t TSize, DAP_REQUIRES(isFloatingPoint<TType>())>
    constexpr std::array<TType, TSize> frange()
    {
        return detail::norm_range_impl<TType>(detail::make_sequence<TSize>{});
    }

    // linspace, [Start, End]
    template <std::size_t TSize, typename T>
    constexpr std::array<T, TSize> linspace(T Start, T End)
    {
        return detail::linspace_impl<T>(Start, End, detail::make_sequence<TSize>{});
    }

    // apply (will be available in c++17)
    namespace detail
    {
        template <class F, class Tuple, std::size_t... I>
        constexpr decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...> /*unused*/)
        {
            return std::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))...);
        }
    } // namespace detail

    template <class F, class... Ts>
    constexpr decltype(auto) apply(F&& f, std::tuple<Ts...>&& t)
    {
        using Tuple = std::tuple<Ts...>;
        using size  = std::tuple_size<std::remove_reference_t<Tuple>>;
        return detail::apply_impl(
            std::forward<F>(f), std::forward<Tuple>(t), std::make_index_sequence<size::value>{});
    }

    // tuple for_each
    template <std::size_t I = 0, typename Fn, typename... Tp>
    inline typename std::enable_if<I == sizeof...(Tp), void>::type for_each(std::tuple<Tp...>&, Fn)
    {
    }

    template <std::size_t I = 0, typename Fn, typename... Tp>
        inline typename std::enable_if < I<sizeof...(Tp), void>::type
                                         for_each(std::tuple<Tp...>& t, Fn f)
    {
        f(std::get<I>(t));
        for_each<I + 1, Fn, Tp...>(t, f);
    }

    // tuple for_each
    template <std::size_t I = 0, typename Fn, typename... Tp>
    inline typename std::enable_if<I == sizeof...(Tp), void>::type
    for_each(const std::tuple<Tp...>&, Fn)
    {
    }

    template <std::size_t I = 0, typename Fn, typename... Tp>
        inline typename std::enable_if < I<sizeof...(Tp), void>::type
                                         for_each(const std::tuple<Tp...>& t, Fn f)
    {
        f(std::get<I>(t));
        for_each<I + 1, Fn, Tp...>(t, f);
    }

    // remove/take first type from tuple
    template <typename T>
    struct RemoveFirst
    {
    };
    template <typename T, typename... Ts>
    struct RemoveFirst<std::tuple<T, Ts...>>
    {
        using type = std::tuple<Ts...>;
    };
    template <typename... Ts>
    constexpr auto removeFirst(std::tuple<Ts...>)
    {
        return typename RemoveFirst<std::tuple<Ts...>>::type{};
    }

    template <size_t N, typename Tuple>
    using tuple_element_t = typename std::tuple_element<N, Tuple>::type;

    template <typename... Ts>
    struct IsTuple : std::false_type
    {
    };
    template <typename... Ts>
    struct IsTuple<std::tuple<Ts...>> : std::true_type
    {
    };
    template <typename T>
    constexpr bool isTuple(T)
    {
        return IsTuple<T>::value;
    }

    template <typename... Ts>
    constexpr bool tupleSize(std::tuple<Ts...>)
    {
        return std::tuple_size<std::tuple<Ts...>>::value;
    }
    template <size_t size, typename... Ts>
    constexpr bool tupleSizeIs(std::tuple<Ts...>)
    {
        return std::tuple_size<std::tuple<Ts...>>::value == size;
    }

    // return index of the first occurrence of a type in a tuple
    // taken from: https://stackoverflow.com/questions/18063451/get-index-of-a-tuple-elements-type
    template <class T, class Tuple>
    struct Index;

    template <class T, class... Ts>
    struct Index<T, std::tuple<T, Ts...>>
    {
        static const std::size_t value = 0;
    };

    template <class T, class U, class... Ts>
    struct Index<T, std::tuple<U, Ts...>>
    {
        static const std::size_t value = 1 + Index<T, std::tuple<Ts...>>::value;
    };

    // return true if tuple has type T
    namespace detail
    {
        template <typename T, typename Tuple>
        struct HasType;
        template <typename T>
        struct HasType<T, std::tuple<>> : std::false_type
        {
        };
        template <typename T, typename U, typename... Ts>
        struct HasType<T, std::tuple<U, Ts...>> : HasType<T, std::tuple<Ts...>>
        {
        };
        template <typename T, typename... Ts>
        struct HasType<T, std::tuple<T, Ts...>> : std::true_type
        {
        };
    }
    template <typename T, typename Tuple>
    using tuple_contains_type_t = typename detail::HasType<T, Tuple>::type;

    template <typename T, typename... Ts>
    constexpr bool tupleContainsType(T, std::tuple<Ts...>)
    {
        return tuple_contains_type_t<T, std::tuple<Ts...>>{};
    }

    // compile time string
    template <char... Chars>
    struct constexpr_string
    {
    };

    // convert constexpr_string to runtime string
    template <char... Chars>
    std::string to_string(constexpr_string<Chars...>)
    {
        return {Chars...};
    }

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"
#endif // __clang__
    // literal operator for constructing a constexpr string (warns for being a GCC extension)
    template <typename TChar, TChar... c>
    constexpr constexpr_string<c...> operator"" _s()
    {
        return {};
    }
#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__

    template <char... Chars1, char... Chars2>
    constexpr auto operator==(constexpr_string<Chars1...>, constexpr_string<Chars2...>)
    {
        return std::is_same<constexpr_string<Chars1...>, constexpr_string<Chars2...>>{};
    }
    template <char... Chars1, char... Chars2>
    constexpr auto operator+(constexpr_string<Chars1...>, constexpr_string<Chars2...>)
    {
        return constexpr_string<Chars1..., Chars2...>{};
    }

    // convert unsigned into constexpr_string
    namespace detail
    {
        template <unsigned... digits>
        struct NumToChars
        {
            static constexpr auto value = constexpr_string<(digits)...>{};
        };
        template <>
        struct NumToChars<0>
        {
            static constexpr auto value = "0"_s;
        };

        template <unsigned rem, unsigned... digits>
        struct ExpandNumToChars : ExpandNumToChars<rem / 10, rem % 10, digits...>
        {
        };

        template <unsigned... digits>
        struct ExpandNumToChars<0, digits...> : NumToChars<('0' + digits)...>
        {
        };
        template <>
        struct ExpandNumToChars<0> : NumToChars<0>
        {
        };
    }

    template <unsigned value>
    struct num_constexpr_string : detail::ExpandNumToChars<value>
    {
    };
}

#endif // DAP_BASE_TYPE_TRAITS_H
