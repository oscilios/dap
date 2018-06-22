#ifndef DAP_DSP_OSCILLATOR_FUNCTIONS_H
#define DAP_DSP_OSCILLATOR_FUNCTIONS_H

#include "base/Constants.h"
#include <ostream>

namespace dap
{
    namespace dsp
    {
        class OscillatorFunctions;
    }
}

class dap::dsp::OscillatorFunctions final
{
public:
    enum class Shape
    {
        Sine,
        Square,
        Saw,
        InverseSaw,
        Triangle,
    };

    struct SineTag
    {
    };
    struct SquareTag
    {
    };
    struct SawTag
    {
    };
    struct InverseSawTag
    {
    };
    struct TriangleTag
    {
    };

    template <typename T>
    inline static auto process(T phase, SineTag&&)
    {
        return std::sin(phase);
    }
    template <typename T>
    inline static auto process(T phase, SquareTag)
    {
        return phase < M_PI ? 1.0f : -1.0f;
    }
    template <typename T>
    inline static auto process(T phase, SawTag&&)
    {
        return 1.0f - phase / M_PI;
    }
    template <typename T>
    inline static auto process(T phase, InverseSawTag&&)
    {
        return phase - 1.0f / M_PI;
    }
    template <typename T>
    inline static auto process(T phase, TriangleTag&&)
    {
        auto triangle = [](T ph) { return ph < 2.0f ? ph - 1.0f : 3.0f - ph; };
        return triangle(phase * INV_HALF_PI);
    }

    template <typename T>
    inline static T process(T phase, Shape shape)
    {
        switch (shape)
        {
            case Shape::Sine:
                return process(phase, SineTag{});
            case Shape::Square:
                return process(phase, SquareTag{});
            case Shape::Saw:
                return process(phase, SawTag{});
            case Shape::InverseSaw:
                return process(phase, InverseSawTag{});
            case Shape::Triangle:
                return process(phase, TriangleTag{});
        }
        return T(0);
    }
};
namespace dap
{
    namespace dsp
    {
        inline std::ostream& operator<<(std::ostream& out, OscillatorFunctions::Shape shape)
        {
            using Shape = OscillatorFunctions::Shape;
            switch (shape)
            {
                case Shape::Sine:
                    out << "Sine";
                    break;
                case Shape::Square:
                    out << "Square";
                    break;
                case Shape::Saw:
                    out << "Saw";
                    break;
                case Shape::InverseSaw:
                    out << "InverseSaw";
                    break;
                case Shape::Triangle:
                    out << "Triangle";
                    break;
            }
            return out;
        }
    }
}

#endif // DAP_DSP_OSCILLATOR_FUNCTIONS_H
