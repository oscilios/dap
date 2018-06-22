#ifndef DAP_BASE_KEY_VALUE_TUPLE_H
#define DAP_BASE_KEY_VALUE_TUPLE_H

#include "TypeTraits.h"

namespace dap
{
    template <typename First, typename Second>
    constexpr auto first(std::tuple<First, Second> t)
    {
        return std::get<0>(t);
    }
    template <typename First, typename Second>
    constexpr auto second(std::tuple<First, Second> t)
    {
        return std::get<1>(t);
    }

    // check whether a tuple has unique types
    template <typename T, typename Tuple>
    struct IsUnique;
    template <typename T>
    struct IsUnique<T, std::tuple<>> : std::true_type
    {
    };
    template <typename T, typename U, typename... Ts>
    struct IsUnique<T, std::tuple<U, Ts...>> : IsUnique<T, std::tuple<Ts...>>
    {
    };
    template <typename T, typename... Ts>
    struct IsUnique<T, std::tuple<T, Ts...>> : std::false_type
    {
    };
    template <typename T>
    constexpr bool isUnique(T)
    {
        return true;
    }
    template <typename T, typename U>
    constexpr bool isUnique(T, U)
    {
        return !std::is_same<T,U>::value;
    }
    template <typename T, typename U, typename... Ts>
    constexpr bool isUnique(T, U, Ts...)
    {
        return IsUnique<T, std::tuple<U,Ts...>>::value && IsUnique<U, std::tuple<Ts...>>::value;
    }

    // (naive) associative type list based on two tuples: one for keys, one for values
    template <typename KeysTuple, typename ValuesTuple>
    struct KeyValueTuple
    {
        static_assert(IsTuple<KeysTuple>::value, "keys must be a tuple");
        static_assert(IsTuple<ValuesTuple>::value, "values must be a tuple");

        using keys = KeysTuple;
        ValuesTuple values;

        constexpr KeyValueTuple(ValuesTuple&& vs)
        : values(vs)
        {
        }

        template <typename Key>
        constexpr auto& operator[](Key)
        {
            return std::get<Index<Key, KeysTuple>::value>(values);
        }
        template <typename Key>
        constexpr const auto& operator[](Key) const
        {
            return std::get<Index<Key, KeysTuple>::value>(values);
        }
    };

    template <typename... Tuples>
    constexpr auto make_key_value_tuple(Tuples&&... tuples)
    {
        static_assert(all(isTuple(Tuples{})...), "types must be tuples");
        static_assert(all(tupleSizeIs<2>(Tuples{})...), "tuples must be key/value pairs");
        static_assert(isUnique(first(Tuples{})...), "keys must be unique");
        return KeyValueTuple<decltype(std::make_tuple(first(Tuples{})...)),
                             decltype(std::make_tuple(second(Tuples{})...))>{
            std::make_tuple(second(std::forward<Tuples>(tuples))...)};
    }
}
#endif // DAP_BASE_KEY_VALUE_TUPLE_H
