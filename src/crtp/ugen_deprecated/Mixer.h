#ifndef DAP_CRTP_UGEN_MIXER_H
#define DAP_CRTP_UGEN_MIXER_H

#include "base/KeyValueTuple.h" // first
#include "base/VariadicOps.h"   // sum
#include "crtp/nodes/Node.h"
#include <limits>

namespace dap
{
    namespace crtp
    {
        template <typename Ins>
        class MixerBus;
        template <typename Ins>
        class Mixer;
    }
}

template <typename Ins>
class dap::crtp::MixerBus final : public NodeExpression<MixerBus<Ins>, Ins>
{
public:
    constexpr auto inputNames()
    {
        return make_input_names("gain"_s, "signal"_s);
    }
    auto operator()()
    {
        using base  = NodeExpression<MixerBus<Ins>, Ins>;
        auto& gain  = base::template input("gain"_s);
        auto& input = base::template input("signal"_s);
        return dispatch(input) * dispatch(gain);
    }
};
template <typename Ins>
class dap::crtp::Mixer final : public NodeExpression<Mixer<Ins>, Ins>
{
    // Expects inputs to be of type MixerBus<T,U> // TODO: add static_assert

    using base_type = NodeExpression<Mixer<Ins>, Ins>;
    static constexpr auto indices = std::make_index_sequence<base_type::inputCount()>{};

    template <typename T>
    constexpr auto sqr(T t)
    {
        return t * t;
    }
    template <typename T, size_t... Is>
    constexpr auto gainRms(T& t, std::index_sequence<Is...>)
    {
        return std::sqrt(sum(sqr(dispatch(first(std::get<Is>(t).inputs())))...));
    }
    template <typename T, size_t... Is>
    constexpr auto sumBuses(T& t, std::index_sequence<Is...>)
    {
        return sum(dispatch(std::get<Is>(t))...);
    }
    template <typename T>
    constexpr auto neverZero(T&& t)
    {
        return std::forward<T>(t) + std::numeric_limits<T>::epsilon();
    }
    template <class Tuple>
    constexpr auto normalizedSum(Tuple& t)
    {
        return sumBuses(t, indices) / neverZero(gainRms(t, indices));
    }

public:
    constexpr auto inputNames()
    {
        return make_input_names("bus_"_s, indices);
    }
    auto operator()()
    {
        return normalizedSum(base_type::m_inputs);
    }
};
#endif // DAP_CRTP_UGEN_MIXER_H
