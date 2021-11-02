#pragma once

#include "../math.hpp"
#include "../utils.hpp"

namespace fantom
{
    /// precision of data types, e.g., of values in a ValueArray
    enum class Precision : unsigned char
    {
        INT8,
        INT16,
        INT32,
        INT64,
        UINT8,
        UINT16,
        UINT32,
        UINT64,
        FLOAT32,
        FLOAT64
    };

    /// classification of the structuring of the point set
    enum class StructuringType : unsigned char
    {
        UNSTRUCTURED = 0, ///< unstructured
        CURVILINEAR = 1, ///< curvilinear, i.e., same number of points in each topological dimension which may be curved
        RECTILINEAR
        = 2,        ///< rectilinear, i.e., like curvilinear with dimensions aligned to the Cartesian coordinate system
        UNIFORM = 3 ///< uniform, i.e., like rectilinear with equally distributed points
    };

    /// classification of the topology of a grid
    enum class TopologyType : unsigned char
    {
        UNSTRUCTURED = 0, ///< unstructured grid (for arbitrary point sets)
        STRUCTURED = 1    ///< structured grid on the points (for CURVILINEAR, RECTILINEAR and UNIFORM point sets)
    };

    /// type of the time behavior of a tensor field
    enum class TimeBehavior : unsigned char
    {
        steady,  ///< the field does not change over time
        unsteady ///< the field changes over time
                 // TODO: cyclic
    };

    //=========================== string representations of various types =============================================

    const std::string structuringTypeNames[] = { "unstructured", "curvilinear", "rectilinear", "uniform" };
    const std::string topologyTypeNames[] = { "unstructured", "structured" };
    const std::string precisionNames[]
        = { "int8", "int16", "int32", "int64", "uint8", "uint16", "uint32", "uint64", "float32", "float64" };
    const std::string domainTypeNames[] = { "point-based", "cell-based" };
    const std::string timeBehaviorNames[] = { "steady", "unsteady" };

    //=========================== templates for generation of string representations for various C++ types
    //==========================

    namespace types
    {
        template < typename T >
        inline std::string TensorName()
        {
            return "unknown";
        }
        template <>
        inline std::string TensorName< Color >()
        {
            return "Color";
        }
        template <>
        inline std::string TensorName< int64_t >()
        {
            return "Integer";
        }
        template <>
        inline std::string TensorName< size_t >()
        {
            return "Index";
        }
        template <>
        inline std::string TensorName< Scalar >()
        {
            return "Scalar";
        }
        template <>
        inline std::string TensorName< Vector1 >()
        {
            return "1-D Vector";
        }
        template <>
        inline std::string TensorName< Vector2 >()
        {
            return "2-D Vector";
        }
        template <>
        inline std::string TensorName< Vector3 >()
        {
            return "3-D Vector";
        }
        template <>
        inline std::string TensorName< Vector4 >()
        {
            return "4-D Vector";
        }
        template <>
        inline std::string TensorName< Matrix2 >()
        {
            return "2x2 Matrix";
        }
        template <>
        inline std::string TensorName< Matrix3 >()
        {
            return "3x3 Matrix";
        }
        template <>
        inline std::string TensorName< Matrix4 >()
        {
            return "4x4 Matrix";
        }

        template < typename TensorType, size_t Dim >
        struct TensorDimensionMatches
        {
            static const bool value = false;
        };
        template < typename T, size_t Dim >
        struct TensorDimensionMatches< Tensor< T >, Dim >
        {
            static const bool value = true;
        };
        template < typename T, size_t Dim >
        struct TensorDimensionMatches< Tensor< T, Dim >, Dim >
        {
            static const bool value = true;
        };
        template < typename T, size_t Dim >
        struct TensorDimensionMatches< Tensor< T, Dim, Dim >, Dim >
        {
            static const bool value = true;
        };

        template < size_t Dim >
        inline std::string DimensionName()
        {
            return "unknown";
        }
        template <>
        inline std::string DimensionName< 1 >()
        {
            return "1-D";
        }
        template <>
        inline std::string DimensionName< 2 >()
        {
            return "2-D";
        }
        template <>
        inline std::string DimensionName< 3 >()
        {
            return "3-D";
        }
        template <>
        inline std::string DimensionName< 4 >()
        {
            return "4-D";
        }
    } // namespace types

    namespace detail
    {
        /// Try to perform a linear combination with T. If that succeeds, we return a true_type
        template < typename T >
        static auto testForLinearCombination( int ) -> sfinae_true< decltype( T() + 1.0 * T() ) >;

        /// If the call does not succeed, the return type is std::false_type. If the above function is existent,
        /// this function is not selected because of the parameter type.
        template < typename T >
        static auto testForLinearCombination( long ) -> std::false_type;
    } // namespace detail

    /// Functor struct for linear combination of arbitrary types.
    template < typename T, bool canLinearCombine = decltype( detail::testForLinearCombination< T >( 0 ) )::value >
    struct LinearCombination
    {
        template < typename Values >
        void
        operator()( T& result, size_t size, const Values& values, const size_t index[], const double weight[] ) const
        {
            for( size_t i = 0; i < size; ++i )
            {
                result += weight[i] * values[index[i]];
            }
        }
    };

    /// Partial specialization for types that cannot handle a normal linear combination. We use nearest neighbor
    /// instead.
    template < typename T >
    struct LinearCombination< T, false >
    {
        template < typename Values >
        void
        operator()( T& result, size_t size, const Values& values, const size_t index[], const double weight[] ) const
        {
            if( size == 0 )
                return;

            size_t best = 0;
            for( size_t i = 1; i < size; ++i )
            {
                if( weight[i] > weight[best] )
                    best = i;
            }
            result = values[index[best]];
        }
    };
} // namespace fantom
