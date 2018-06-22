#ifndef DAP_ARRAY_OPS_EIGEN_IMPL_H
#define DAP_ARRAY_OPS_EIGEN_IMPL_H

#include "base/SystemAnnotations.h"
#include <cassert>
#include <type_traits>
#include <Eigen/Core>

namespace dap
{
    namespace fastmath
    {
        template <typename Op, typename Rhs>
        using CwiseUnaryOp = Eigen::CwiseUnaryOp<Op, Rhs>;

        template <typename Op, typename Lhs, typename Rhs>
        using CwiseBinaryOp = Eigen::CwiseBinaryOp<Op, Lhs, Rhs>;

        template <typename Lhs, typename Rhs, int NestingFlags>
        using CoeffBasedProduct = Eigen::CoeffBasedProduct<Lhs, Rhs, NestingFlags>;

        namespace array
        {
            template <typename T>
            using ArrayType = Eigen::Array<T, Eigen::Dynamic, 1>;

            template <typename T>
            using MapType = Eigen::Map<ArrayType<T>, Eigen::Aligned>;

            template <typename T>
            using ConstMapType = Eigen::Map<const ArrayType<T>, Eigen::Aligned>;

            template <typename T>
            MapType<T> map(T* x, size_t size)
            {
                checkAlignment(x);
                return MapType<T>(x, size);
            }
            template <typename T>
            ConstMapType<T> const_map(const T* x, size_t size)
            {
                checkAlignment(x);
                return ConstMapType<T>(x, size);
            }
        }
        namespace vector
        {
            template <typename T>
            using VectorType = Eigen::Matrix<T, Eigen::Dynamic, 1>;

            template <typename T>
            using MapType = Eigen::Map<VectorType<T>, Eigen::Aligned>;

            template <typename T>
            using ConstMapType = Eigen::Map<const VectorType<T>, Eigen::Aligned>;

            template <typename T>
            MapType<T> map(T* x, size_t size)
            {
                checkAlignment(x);
                return MapType<T>(x, size);
            }
            template <typename T>
            ConstMapType<T> const_map(const T* x, size_t size)
            {
                checkAlignment(x);
                return ConstMapType<T>(x, size);
            }
        }
        namespace matrix
        {
            enum Order
            {
                ColMajor = Eigen::ColMajor,
                RowMajor = Eigen::RowMajor
            };
            // compile time size
            template <typename T, size_t Rows, size_t Cols, int Order = ColMajor>
            using MatrixType                                          = Eigen::Matrix<T, Rows, Cols, Order>;

            template <typename T, size_t Rows, size_t Cols, int Order = ColMajor>
            using MapType                                             = Eigen::Map<MatrixType<T, Rows, Cols, Order>, Eigen::Aligned>;

            template <typename T, size_t Rows, size_t Cols, int Order = ColMajor>
            using ConstMapType                                        = Eigen::Map<const MatrixType<T, Rows, Cols, Order>, Eigen::Aligned>;

            template <typename T, size_t Rows, size_t Cols, int Order = ColMajor>
            MapType<T, Rows, Cols, Order> map(T* x, size_t DAP_ATTRIBUTE_UNUSED size)
            {
                assert(size == Cols * Rows);
                checkAlignment(x);
                return MapType<T, Rows, Cols, Order>(x);
            }
            template <typename T, size_t Rows, size_t Cols, int Order = ColMajor>
            ConstMapType<T, Rows, Cols, Order> const_map(const T* x, size_t DAP_ATTRIBUTE_UNUSED size)
            {
                assert(size == Cols * Rows);
                checkAlignment(x);
                return ConstMapType<T, Rows, Cols, Order>(x);
            }

            // dynamic size
            template <typename T, int Order = ColMajor>
            using MatrixTypeDynamic         = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;

            template <typename T, int Order = ColMajor>
            using MapTypeDynamic            = Eigen::Map<MatrixTypeDynamic<T, Order>, Eigen::Aligned>;

            template <typename T, int Order = ColMajor>
            using ConstMapTypeDynamic =
                Eigen::Map<const MatrixTypeDynamic<T, Order>, Eigen::Aligned>;

            template <typename T, int Order = ColMajor>
            MapTypeDynamic<T, Order> map(T* x, size_t rows, size_t cols)
            {
                checkAlignment(x);
                return MapTypeDynamic<T, Order>(x, rows, cols);
            }
            template <typename T, int Order = ColMajor>
            ConstMapTypeDynamic<T, Order> const_map(const T* x, size_t rows, size_t cols)
            {
                checkAlignment(x);
                return ConstMapTypeDynamic<T, Order>(x, rows, cols);
            }
        }

        // constant ops
        template <typename T>
        bool all(const T* x, size_t size)
        {
            return array::const_map(x, size).all();
        }
        template <typename T>
        bool any(const T* x, size_t size)
        {
            return array::const_map(x, size).any();
        }
        template <typename T>
        bool allFinite(const T* x, size_t size)
        {
            static_assert(!std::is_integral<T>::value, "Integral types are always finite.");
            return array::const_map(x, size).allFinite();
        }
        template <typename T>
        bool hasNaN(const T* x, size_t size)
        {
            static_assert(!std::is_integral<T>::value, "NaN is floating point only.");
            return array::const_map(x, size).hasNaN();
        }

        // unary ops
        template <typename T>
        void abs(T* result, const T* x, size_t size)
        {
            array::map(result, size) = array::const_map(x, size).abs();
        }
        template <typename T>
        void abs2(T* result, const T* x, size_t size)
        {
            array::map(result, size) = array::const_map(x, size).abs2();
        }
        template <typename T>
        void normalize(T* x, size_t size)
        {
            vector::map(x, size).normalize();
        }
        template <typename T>
        void cos(T* result, const T* x, size_t size)
        {
            static_assert(!std::is_integral<T>::value, "cosine requires floating point.");
            array::map(result, size) = array::const_map(x, size).cos();
        }
        template <typename T>
        void sin(T* result, const T* x, size_t size)
        {
            static_assert(!std::is_integral<T>::value, "sine requires floating point.");
            array::map(result, size) = array::const_map(x, size).sin();
        }
        template <typename T>
        void tan(T* result, const T* x, size_t size)
        {
            static_assert(!std::is_integral<T>::value, "tangent requires floating point.");
            array::map(result, size) = array::const_map(x, size).tan();
        }
        template <typename T>
        void acos(T* result, const T* x, size_t size)
        {
            static_assert(!std::is_integral<T>::value, "arc cosine requires floating point.");
            array::map(result, size) = array::const_map(x, size).acos();
        }
        template <typename T>
        void asin(T* result, const T* x, size_t size)
        {
            static_assert(!std::is_integral<T>::value, "arc sine requires floating point.");
            array::map(result, size) = array::const_map(x, size).asin();
        }
        template <typename T>
        T max(const T* x, size_t size)
        {
            return array::const_map(x, size).maxCoeff();
        }
        template <typename T>
        T min(const T* x, size_t size)
        {
            return array::const_map(x, size).minCoeff();
        }
        template <typename T>
        T sum(const T* x, size_t size)
        {
            return array::const_map(x, size).sum();
        }
        template <typename T>
        T prod(const T* x, size_t size)
        {
            return vector::const_map(x, size).prod();
        }
        template <typename T>
        void fill(const T& value, T* x, size_t size)
        {
            return array::map(x, size).fill(value);
        }
        template <typename T>
        void linspace(const T& low, const T& high, T* x, size_t size)
        {
            array::map(x, size).setLinSpaced(low, high);
        }
        template <typename T>
        T mean(const T* x, size_t size)
        {
            return array::const_map(x, size).mean();
        }
        template <typename T, typename U>
        T mean(U* result, const T* x, size_t size)
        {
            *result = mean(x, size);
        }
        template <typename T>
        T norm(const T* x, size_t size)
        {
            return vector::const_map(x, size).norm();
        }
        template <typename T, typename U>
        void norm(U* result, const T* x, size_t size)
        {
            *result = norm(x, size);
        }
        template <typename T>
        T squaredNorm(const T* x, size_t size)
        {
            return vector::const_map(x, size).squaredNorm();
        }
        template <typename T>
        void square(T* result, const T* x, size_t size)
        {
            array::map(result, size) = array::const_map(x, size).square();
        }
        template <typename T>
        void cube(T* result, const T* x, size_t size)
        {
            array::map(result, size) = array::const_map(x, size).cube();
        }
        template <typename T>
        void pow(const T& value, T* result, const T* x, size_t size)
        {
            array::map(result, size) = array::const_map(x, size).pow(value);
        }
        template <typename T>
        void sqrt(T* result, const T* x, size_t size)
        {
            array::map(result, size) = array::const_map(x, size).sqrt();
        }
        template <typename T>
        void exp(T* result, const T* x, size_t size)
        {
            array::map(result, size) = array::const_map(x, size).exp();
        }
        template <typename T>
        void log(T* result, const T* x, size_t size)
        {
            array::map(result, size) = array::const_map(x, size).log();
        }
        template <typename T>
        void inverse(T* result, const T* x, size_t size)
        {
            array::map(result, size) = array::const_map(x, size).inverse();
        }
        template <typename T>
        void conjugate(std::complex<T>* result, const std::complex<T>* x, size_t size)
        {
            array::map(result, size) = array::const_map(x, size).conjugate();
        }

        // binary ops
        template <typename T>
        void add(T* result, const T* x, const T* y, size_t size)
        {
            array::map(result, size) = array::const_map(x, size) + array::const_map(y, size);
        }
        template <typename T>
        void sub(T* result, const T* x, const T* y, size_t size)
        {
            array::map(result, size) = array::const_map(x, size) - array::const_map(y, size);
        }
        template <typename T>
        void mul(T* result, const T* x, const T* y, size_t size)
        {
            array::map(result, size) = array::const_map(x, size) * array::const_map(y, size);
        }
        template <typename T>
        void div(T* result, const T* x, const T* y, size_t size)
        {
            array::map(result, size) = array::const_map(x, size) / array::const_map(y, size);
        }

    } // namespace fastmath
} // namespace dap
#endif // DAP_ARRAY_OPS_EIGEN_IMPL_H
