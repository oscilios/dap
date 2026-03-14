#ifndef DAP_DSP_UNIFORM_DISTRIBUTION_H
#define DAP_DSP_UNIFORM_DISTRIBUTION_H

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
    std::uniform_real_distribution<float> m_distribution;

public:
    using value_type = float;
    UniformDistribution()
    : m_generator(std::random_device()())
    , m_distribution(-1.0f, 1.0f)
    {
    }
    inline auto operator()()
    {
        return m_distribution(m_generator);
    }
};

#endif // DAP_DSP_UNIFORM_DISTRIBUTION_H
