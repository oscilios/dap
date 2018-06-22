#ifndef DAP_FASTMATH_ARRAY_OPS_H
#define DAP_FASTMATH_ARRAY_OPS_H

#include <complex>
#include <stdexcept>

namespace dap
{
    namespace fastmath
    {
        template <typename T>
        constexpr bool aligned(const T* ptr, size_t bytes)
        {
            return (size_t(ptr) & (bytes - 1)) == 0;
        }
        template <typename T>
        constexpr bool sameAlignment(const T* ptr1, const T* ptr2, size_t bytes)
        {
            return (size_t(ptr1) & (bytes - 1)) == (size_t(ptr2) & (bytes - 1));
        }
        template <typename T>
        void checkAlignment(const T* ptr)
        {
            if (!aligned(ptr, 16))
            {
                throw std::runtime_error("Pointer not correctly aligned");
            }
        }

        // constant ops
        template <typename T>
        bool all(const T* x, size_t size); // true if all coefficients are true
        template <typename T>
        bool any(const T* x, size_t size); // true if at least one coefficient is true
        template <typename T>
        bool allFinite(const T* x, size_t size); // true if contains only finite numbers, i.e., no
                                                 // NaN and no +/-INF values.
        template <typename T>
        bool hasNaN(const T* x, size_t size); // true if contains one NaN coefficient

        // unary ops
        template <typename T>
        void abs(T* result, const T* x, size_t size); // coefficient-wise absolute value
        template <typename T>
        void abs2(T* result, const T* x, size_t size); // coefficient-wise squqre absolute value
        template <typename T>
        void normalize(T* x, size_t size); // coefficient-wise normalize
        template <typename T>
        void cos(T* result, const T* x, size_t size); // coefficient-wise cosine
        template <typename T>
        void sin(T* result, const T* x, size_t size); // coefficient-wise sine
        template <typename T>
        void tan(T* result, const T* x, size_t size); // coefficient-wise tan
        template <typename T>
        void acos(T* result, const T* x, size_t size); // coefficient-wise arc cosine
        template <typename T>
        void asin(T* result, const T* x, size_t size); // coefficient-wise arc sine

        template <typename T>
        T max(const T* x,
              size_t size); // the maximum of all coefficients (undefined if there is a NaN)
        template <typename T>
        T min(const T* x,
              size_t size); // the minimum of all coefficients (undefined if there is a NaN)
        template <typename T>
        T sum(const T* x, size_t size); // the sum of all coefficients
        template <typename T>
        T prod(const T* x, size_t size); // the product of all coefficients
        template <typename T>
        void fill(const T& value, T* x, size_t size); // set all coefficients to value
        template <typename T>
        void linspace(const T& low, const T& high, T* x, size_t size); // linearly spaced vector
        template <typename T>
        T mean(const T* x, size_t size); // the mean of all coefficients
        template <typename T, typename U>
        void mean(U* result, const T* x, size_t size); // the mean of all coefficients
        template <typename T>
        T norm(const T* x, size_t size); // norm of the vector
        template <typename T, typename U>
        void norm(U* result, const T* x, size_t size); // norm of the vector
        template <typename T>
        T squaredNorm(const T* x, size_t size); // sum of squares of the vector coefficients
        template <typename T>
        void square(T* result, const T* x, size_t size); // coefficient-wise square
        template <typename T>
        void cube(T* result, const T* x, size_t size); // coefficient-wise cube
        template <typename T>
        void pow(const T& value, T* result, const T* x, size_t size); // coefficient-wise pow(value)
        template <typename T>
        void sqrt(T* result, const T* x, size_t size); // coefficient-wise sqrt
        template <typename T>
        void exp(T* result, const T* x, size_t size); // coefficient-wise exp
        template <typename T>
        void log(T* result, const T* x, size_t size); // coefficient-wise log
        template <typename T>
        void inverse(T* result, const T* x, size_t size); // coefficient-wise inverse
        template <typename T>
        void conjugate(std::complex<T>* result,
                       const std::complex<T>* x,
                       size_t size); // complex conjugate

        // binary ops
        template <typename T>
        void add(T* result, const T* x, const T* y, size_t size); // x+y
        template <typename T>
        void sub(T* result, const T* x, const T* y, size_t size); // x-y
        template <typename T>
        void mul(T* result, const T* x, const T* y, size_t size); // x*y
        template <typename T>
        void div(T* result, const T* x, const T* y, size_t size); // x/y

    } // namespace fastmath
} // namespace dap

#endif // DAP_FASTMATH_ARRAY_OPS_H

#include "array_ops_eigen_impl.h"
