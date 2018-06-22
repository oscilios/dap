#ifndef DAP_BASE_CONSTANTS_H
#define DAP_BASE_CONSTANTS_H

#include <cmath>

#define UNUSED_ARG(arg) (void)(arg);

namespace dap
{
    constexpr float TWO_PI      = 2.0f * float(M_PI);
    constexpr float INV_TWO_PI  = 1.0f / TWO_PI;
    constexpr float HALF_PI     = float(M_PI) / 2.0f;
    constexpr float INV_HALF_PI = 1.0f / HALF_PI;

}

#endif // DAP_BASE_CONSTANTS_H
