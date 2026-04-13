#ifndef DAP_DSP_DISTORTION_H
#define DAP_DSP_DISTORTION_H

#include <algorithm>
#include <cmath>

namespace dap
{
    namespace dsp
    {
        class Distortion;
    }
}

class dap::dsp::Distortion final
{
public:
    enum class Type
    {
        HardClip,
        SoftClip,
        Wavefold,
        Exponential,
        Bitcrush
    };

    // drive:  pre-gain, typically 1..50
    // mix:    dry/wet, 0 = dry, 1 = full wet
    // param:  type-specific parameter:
    //           HardClip:     threshold (0..1, default ~0.8)
    //           SoftClip:     saturation curve steepness (1..20)
    //           Wavefold:     fold threshold (0.1..1.0)
    //           Exponential:  exponent shape (0.1..10)
    //           Bitcrush:     bit depth (1..16)
    template <typename T>
    inline auto operator()(T input, T drive, T mix, T param, Type type) const
    {
        const T driven = input * drive;
        T wet;

        switch (type)
        {
            case Type::HardClip:
                wet = hardClip(driven, param);
                break;
            case Type::SoftClip:
                wet = softClip(driven, param);
                break;
            case Type::Wavefold:
                wet = wavefold(driven, param);
                break;
            case Type::Exponential:
                wet = exponential(driven, param);
                break;
            case Type::Bitcrush:
                wet = bitcrush(driven, param);
                break;
        }

        return (T(1) - mix) * input + mix * wet;
    }

private:
    // Hard clip at +/- threshold
    template <typename T>
    static inline T hardClip(T x, T threshold)
    {
        return std::clamp(x, -threshold, threshold);
    }

    // tanh-style soft saturation; steepness controls how sharp the knee is
    template <typename T>
    static inline T softClip(T x, T steepness)
    {
        return std::tanh(steepness * x) / std::tanh(steepness);
    }

    // Triangular wavefold: signal folds back at +/- threshold
    template <typename T>
    static inline T wavefold(T x, T threshold)
    {
        // Map into [-threshold, threshold] range via triangle wave folding
        const T period = T(4) * threshold;
        // Shift so fold is symmetric around zero
        T phase = std::fmod(x + threshold, period);
        if (phase < T(0))
            phase += period;
        // Triangle: rise for first half-period, fall for second
        if (phase < threshold * T(2))
            return phase - threshold;
        else
            return T(3) * threshold - phase;
    }

    // Asymptotic exponential shaping: sign(x) * (1 - e^(-|x|*exponent))
    template <typename T>
    static inline T exponential(T x, T exponent)
    {
        const T absX = std::abs(x);
        return std::copysign(T(1) - std::exp(-absX * exponent), x);
    }

    // Bit-depth reduction (quantize amplitude)
    template <typename T>
    static inline T bitcrush(T x, T bitDepth)
    {
        const T steps = std::exp2(std::max(T(1), std::round(bitDepth))) - T(1);
        return std::round(x * steps) / steps;
    }
};

#endif // DAP_DSP_DISTORTION_H
