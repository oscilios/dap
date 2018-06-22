#ifndef DAP_DSP_MIXER_H
#define DAP_DSP_MIXER_H

#include "base/VariadicOps.h" // sum
#include "base/KeyValueTuple.h" // first
#include <limits>

namespace dap
{
    namespace dsp
    {
        class Mixer;
        class NormalizedMixer;
    }
}

class dap::dsp::Mixer final
{
public:
    struct Bus
    {
        template <typename T, typename U>
        inline auto operator()(T gain, U signal)
        {
            return gain * signal;
        }
    };
    template <typename... Ts>
    inline auto operator()(Ts&&... values)
    {
        static constexpr float eps = std::numeric_limits<float>::epsilon();
        return sum(values...)/(sqrt(sizeof...(Ts)) + eps);
    }
};

class dap::dsp::NormalizedMixer final
{
    template <typename T>
    constexpr auto sqr(T&& t)
    {
        return t * t;
    }
    template <typename...Ts>
    constexpr auto gainRms(Ts&&...t)
    {
        return sqrt(sum(sqr(first(t))...));
    }
    template <typename...Ts>
    constexpr auto sumBuses(Ts&&... t)
    {
        return sum((std::get<0>(t)*std::get<1>(t))...);
    }

public:
    struct Bus
    {
        template <typename T, typename U>
        inline auto operator()(T gain, U signal)
        {
            return std::make_tuple(gain, signal);
        }
    };

    template <typename... Ts, DAP_REQUIRES(all(IsTuple<Ts>::value...))>
    inline auto operator()(Ts&&... values)
    {
        static constexpr float eps = std::numeric_limits<float>::epsilon();
        return sumBuses(values...) / (gainRms(values...) + eps);
    }
};
#endif // DAP_DSP_MIXER_H
