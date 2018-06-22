#ifndef DAP_DSP_UNIFORM_DISTRIBUTION_H
#define DAP_DSP_UNIFORM_DISTRIBUTION_H

#include <map>
#include <random>

namespace dap
{
    namespace dsp
    {
        class UniformDistribution;
    }
}

class dap::dsp::UniformDistribution final
{
    std::mt19937 m_generator;
    std::uniform_int_distribution<> m_distribution;

public:
    using value_type = float;
    UniformDistribution()
    : m_generator(std::random_device()())
    , m_distribution(0, RAND_MAX)
    {
        std::srand(std::time(0));
    }
    inline auto operator()()
    {
        static constexpr value_type factor(2.0f / (RAND_MAX + 1.0f));
        return m_distribution(m_generator) * factor - 1.0f;
    }
};

#endif // DAP_DSP_UNIFORM_DISTRIBUTION_H
