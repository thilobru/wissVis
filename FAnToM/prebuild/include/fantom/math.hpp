#pragma once

#include <algorithm>
#include <cmath>
#include <complex>
#include <iostream>
#include <numeric>
#include <stdexcept>

#include "exceptions.hpp"
#include "math/Quaternion.hpp"
#include "math/Range.hpp"


namespace fantom
{
    /// computes the factorial of a size_t
    size_t factorial( size_t k );

    /// Computes complex conjugate of anything.
    template < class T >
    T conj( const T& rhs )
    {
        return rhs;
    }

    /// Computes the squared norm of anything.
    template < class T >
    T norm2( const T& lhs )
    {
        return conj( lhs ) * lhs; /* argument dependent lookup always selects the fitting conj
                                     function (e.g., for std::complex) */
    }

    /// Computes the squared norm of a complex.
    template < class T >
    T norm2( const std::complex< T >& lhs )
    {
        return norm( lhs ); /* C++ library defect: norm(complex<T>) actually gives the squared norm. */
    }

    namespace fantom_private
    {
        template < class T >
        struct sum_norm2
        {
            typedef decltype( norm2( T() ) ) R;
            R operator()( const R& lhs, const T& rhs ) const
            {
                return lhs + norm2( rhs );
            }
        };

        template < class T >
        struct constant
        {
            typedef T result_type;
            const T& mT;
            constant( const T& t )
                : mT( t )
            {
            }
            const T& operator()()
            {
                return mT;
            }
        };

        template < class T >
        struct iteration
        {
            typedef T result_type;
            const T* mP;
            iteration( const T* p )
                : mP( p )
            {
            }
            const T& operator()()
            {
                return *mP++;
            }
        };

        template < class Op >
        struct positivate
        {
            typedef typename Op::result_type result_type;
            Op mOp;
            positivate( Op op )
                : mOp( op )
            {
            }
            result_type operator()()
            {
                return +mOp();
            }
        };

        template < class Op >
        struct negate
        {
            typedef typename Op::result_type result_type;
            Op mOp;
            negate( Op op )
                : mOp( op )
            {
            }
            result_type operator()()
            {
                return -mOp();
            }
        };

        template < class Op >
        struct conjugate
        {
            typedef typename Op::result_type result_type;
            Op mOp;
            conjugate( Op op )
                : mOp( op )
            {
            }
            result_type operator()()
            {
                return conj( mOp() );
            }
        };

        template < class Op1, class Op2 >
        struct plus
        {
            typedef decltype( std::declval< typename Op1::result_type >()
                              + std::declval< typename Op2::result_type >() ) result_type;
            Op1 mOp1;
            Op2 mOp2;
            plus( Op1 op1, Op2 op2 )
                : mOp1( op1 )
                , mOp2( op2 )
            {
            }
            result_type operator()()
            {
                return mOp1() + mOp2();
            }
        };

        template < class Op1, class Op2 >
        struct minus
        {
            typedef decltype( std::declval< typename Op1::result_type >()
                              - std::declval< typename Op2::result_type >() ) result_type;
            Op1 mOp1;
            Op2 mOp2;
            minus( Op1 op1, Op2 op2 )
                : mOp1( op1 )
                , mOp2( op2 )
            {
            }
            result_type operator()()
            {
                return mOp1() - mOp2();
            }
        };

        template < class Op1, class Op2 >
        struct multiplies
        {
            typedef decltype( std::declval< typename Op1::result_type >()
                              * std::declval< typename Op2::result_type >() ) result_type;
            Op1 mOp1;
            Op2 mOp2;
            multiplies( Op1 op1, Op2 op2 )
                : mOp1( op1 )
                , mOp2( op2 )
            {
            }
            result_type operator()()
            {
                return mOp1() * mOp2();
            }
        };

        template < class Op1, class Op2 >
        struct divides
        {
            typedef decltype( std::declval< typename Op1::result_type >()
                              / std::declval< typename Op2::result_type >() ) result_type;
            Op1 mOp1;
            Op2 mOp2;
            divides( Op1 op1, Op2 op2 )
                : mOp1( op1 )
                , mOp2( op2 )
            {
            }
            result_type operator()()
            {
                return mOp1() / mOp2();
            }
        };

        template < class T >
        constant< T > makeConstant( const T& t )
        {
            return constant< T >( t );
        }

        template < class T >
        iteration< T > makeIteration( const T* t )
        {
            return iteration< T >( t );
        }

        template < class Op >
        positivate< Op > makePositivate( Op op )
        {
            return positivate< Op >( op );
        }

        template < class Op >
        negate< Op > makeNegate( Op op )
        {
            return negate< Op >( op );
        }

        template < class Op >
        conjugate< Op > makeConjugate( Op op )
        {
            return conjugate< Op >( op );
        }

        template < class Op1, class Op2 >
        plus< Op1, Op2 > makePlus( Op1 op1, Op2 op2 )
        {
            return plus< Op1, Op2 >( op1, op2 );
        }

        template < class Op1, class Op2 >
        minus< Op1, Op2 > makeMinus( Op1 op1, Op2 op2 )
        {
            return minus< Op1, Op2 >( op1, op2 );
        }

        template < class Op1, class Op2 >
        multiplies< Op1, Op2 > makeMultiplies( Op1 op1, Op2 op2 )
        {
            return multiplies< Op1, Op2 >( op1, op2 );
        }

        template < class Op1, class Op2 >
        divides< Op1, Op2 > makeDivides( Op1 op1, Op2 op2 )
        {
            return divides< Op1, Op2 >( op1, op2 );
        }

        template < class T, std::size_t... N >
        struct multi_array;

        template < class T >
        struct multi_array< T >
        {
            typedef T type;
            static const std::size_t order = 0;
            static const std::size_t size = 1;
            static T& get( type& t )
            {
                return t;
            }
            static const T& get( const type& t )
            {
                return t;
            }
            static T& get( type& t, const std::size_t[] )
            {
                return t;
            }
            static const T& get( const type& t, const std::size_t /*i*/[] )
            {
                return t;
            }
        };

        template < class T, std::size_t N0, std::size_t... N >
        struct multi_array< T, N0, N... >
        {
            typedef typename multi_array< T, N... >::type type[N0];
            static const std::size_t order = 1 + multi_array< T, N... >::order;
            static const std::size_t size = N0 * multi_array< T, N... >::size;

            template < class... I >
            static T& get( type& t, std::size_t i0, I... i )
            {
#ifndef NDEBUG
                if( i0 >= N0 )
                {
                    throw fantom::logic_error( "index out of bounds." );
                }
#endif
                return multi_array< T, N... >::get( t[i0], i... );
            }

            template < class... I >
            static const T& get( const type& t, std::size_t i0, I... i )
            {
#ifndef NDEBUG
                if( i0 >= N0 )
                {
                    throw fantom::logic_error( "index out of bounds." );
                }
#endif
                return multi_array< T, N... >::get( t[i0], i... );
            }

            static T& get( type& t, const std::size_t i[] )
            {
#ifndef NDEBUG
                if( i[0] >= N0 )
                {
                    throw fantom::logic_error( "index out of bounds." );
                }
#endif
                return multi_array< T, N... >::get( t[i[0]], i + 1 );
            }

            static const T& get( const type& t, const std::size_t i[] )
            {
#ifndef NDEBUG
                if( i[0] >= N0 )
                {
                    throw fantom::logic_error( "index out of bounds." );
                }
#endif
                return multi_array< T, N... >::get( t[i[0]], i + 1 );
            }
        };
    } // namespace fantom_private

    /// tensor \ingroup math
    /**
     * Objects of this class are tensors, i.e. generalizations of
     * mathematical scalars, vectors, matrices. The class uses variadic
     * templates to encode both order and dimension of scalars, e.g.
     *
     * - Tensor<float> is a single precision scalar type,
     * - Tensor<double, 3> is a double precision 3-D vector type,
     * - Tensor<complex<float>, 2, 3> is a single precision
     *   complex 2 by 3 matrix type. The matrix has 2 rows and 3 columns.
     * - Tensor<int, 2, 3, 4> is a 2 by 3 by 4 tensor (3rd order) on integers.
     * - ...
     *
     * The memory layout of coefficients matches the layout of C++
     * multidimensional array, e.g. Tensor<int, 2, 3, 4> is laid out exactly as
     * int[2][3][4]. Accessing coefficients can be done with operator()() which
     * takes a list of indices to address coefficients. An operator[]() is
     * provided as well but works only with vectors.
     */
    template < class T, std::size_t... N >
    class Tensor
    {
    public:
        /// tensor order
        static const std::size_t order = fantom_private::multi_array< T, N... >::order;

        /// number of tensor coefficients
        static const std::size_t size = fantom_private::multi_array< T, N... >::size;

        /// Constructs a tensor and initializes all entries to zero
        Tensor()
            : m() //< ()-constructor initializes the variables as defined in the C++03 standard
        {
        }

        /// Constructs tensor using a constant.
        /**
         * @brief Constructs a homogenous tensor of arbitrary order and dimensions.
         * @param x The value that is used for all tensor coefficients
         */
        explicit Tensor( const T& x )
        {
            // NOTE: This method is different from the ones following,
            // since independent from its size everytime a valid tensor
            // is constructed. The behavior was changed to allow to construct
            // vectors with Tensor(0) as the zero vector.
            for( unsigned int i = 0; i < size; i++ )
            {
                ( *this )[i] = x;
            }
        }

        /// Convenience constructor for 2D, first-order tensor ( i.e. 2D vector ) using
        /// coefficients.
        explicit Tensor( const T& x, const T& y )
        {
            static_assert( order == 1 && size == 2, "invalid tensor" );
            m[0] = x;
            m[1] = y;
        }

        /// Convenience constructor for 3D, first-order tensor ( i.e. 3D vector ) using
        /// coefficients.
        explicit Tensor( const T& x, const T& y, const T& z )
        {
            static_assert( order == 1 && size == 3, "invalid tensor" );
            m[0] = x;
            m[1] = y;
            m[2] = z;
        }

        /// Convenience constructor for 4D, first-order tensor ( i.e. 4D vector ) using
        /// coefficients.
        explicit Tensor( const T& x, const T& y, const T& z, const T& w )
        {
            static_assert( order == 1 && size == 4, "invalid tensor" );
            m[0] = x;
            m[1] = y;
            m[2] = z;
            m[3] = w;
        }

        /// Constructs tensor and initializes its values using the entries in the array
        /**
         * The entries have to be provided in the same order as the internal storage of
         * the tensor
         */
        template < class T1 >
        explicit Tensor( T1* t )
        {
            std::copy( t, t + size, begin() );
        }

        /// Constructs tensor using an initializer list.
        /**
         * Usage example: Tensor<double, 3, 3> t = {11, 12, 13, 21, 22, 23, 31, 32, 33};
         * @param l initializer list
         */
        Tensor( std::initializer_list< T > l )
        {
            if( l.size() != size )
            {
                throw fantom::logic_error( "Wrong size of initializer list." );
            }
            std::copy( l.begin(), l.end(), begin() );
        }

        /// Constructs tensor copying another tensor.
        template < class T1 >
        explicit Tensor( const Tensor< T1, N... >& rhs )
        {
            std::copy( rhs.begin(), rhs.end(), begin() );
        }

        /// \internal Construct tensor from expression.
        template < class Op,
                   typename ignore = typename std::enable_if< !std::is_convertible< Op, const T& >::value >::type >
        explicit Tensor( Op op )
        // the enable_if-statement is used to disable this function completely types types
        // compatible with T
        // to force the usage of Tensor(const T&) in that case
        {
            std::generate_n( begin(), size, op );
        }

        /// \internal Gives an iterator to the first coefficient.
        T* begin()
        {
            return reinterpret_cast< T( & )[size] >( m );
        }

        /// \internal Gives an iterator to the first coefficient.
        const T* begin() const
        {
            return reinterpret_cast< const T( & )[size] >( m );
        }

        /// \internal Gives an iterator past the last coefficient.
        T* end()
        {
            return begin() + size;
        }

        /// \internal Gives an iterator past the last coefficient.
        const T* end() const
        {
            return begin() + size;
        }

        /// Gives the i-th tensor value.
        T& operator[]( const std::size_t i )
        {
#ifndef NDEBUG
            if( i >= size )
            {
                throw fantom::logic_error( "index out of bounds." );
            }
#endif
            return reinterpret_cast< T* >( &m )[i];
        }

        /// Gives the i-th tensor value.
        const T& operator[]( const std::size_t i ) const
        {
#ifndef NDEBUG
            if( i >= size )
            {
                throw fantom::logic_error( "index out of bounds." );
            }
#endif
            return reinterpret_cast< const T* >( &m )[i];
        }

        /// Gives the value for a list of indices.
        /**
         * \param i the list of indices.
         * \pre 0 <= i[k] < N[k] for all 0 <= k < order.
         */
        template < class... I >
        T& operator()( I... i )
        {
            return fantom_private::multi_array< T, N... >::get( m, i... );
        }

        /// Gives the value for a list of indices.
        /**
         * \param i the list of indices.
         * \pre 0 <= i[k] < N[k] for all 0 <= k < order.
         */
        template < class... I >
        const T& operator()( I... i ) const
        {
            return fantom_private::multi_array< T, N... >::get( m, i... );
        }

        /// Gives the value for a list of indices.
        /**
         * \param i the list of indices.
         * \pre 0 <= i[k] < N[k] for all 0 <= k < order.
         * \note calling this function is equivalent to calling operator()(i[0], i[1], ...,
         * i[order-1]).
         */
        T& operator()( const std::size_t i[] )
        {
            return fantom_private::multi_array< T, N... >::get( m, i );
        }

        /// Gives the value for a list of indices.
        /**
         * \param i the list of indices.
         * \pre 0 <= i[k] < N[k] for all 0 <= k < order.
         * \note calling this function is equivalent to calling operator()(i[0], i[1], ...,
         * i[order-1]).
         */
        const T& operator()( const std::size_t i[] ) const
        {
            return fantom_private::multi_array< T, N... >::get( m, i );
        }

        template < typename T2 >
        Tensor< T2, N... > toType() const
        {
            Tensor< T2, N... > result;
            for( size_t i = 0; i < size; ++i )
            {
                result[i] = static_cast< T2 >( ( *this )[i] );
            }
            return result;
        }

    private:
        typename fantom_private::multi_array< T, N... >::type m;
    };

    /// PA=LU factorization for square matrices \ingroup math
    template < class T, std::size_t N >
    class LUPFactorization
    {
        /* algorithms adapted from:
         * Golub, Loan, "Matrix Computations", 3rd ed., The John Hopkins University Press, 1996 */

        /// \internal stores both L and U matrices in one matrix
        Tensor< T, N, N > mLU;

        /// \internal stores permutation
        unsigned char mP[N];

    public:
        /// Constructs new PA=LU factorization.
        explicit LUPFactorization( const Tensor< T, N, N >& A )
            : mLU( A )
        {
            static_assert( N < 256, "LUPFactorization is implemented for matrices up to dimension 256, only" );
            using std::abs;
            using std::swap_ranges;

            for( unsigned char n = 0; n != N; ++n )
            {
                for( unsigned char m = 0; m != n; ++m )
                    for( unsigned char l = 0; l != m; ++l )
                        mLU( m, n ) -= mLU( m, l ) * mLU( l, n ); // U(m,n) -= L(m,l) * U(l,n)

                for( unsigned char m = n; m != N; ++m )
                    for( unsigned char l = 0; l != n; ++l )
                        mLU( m, n ) -= mLU( m, l ) * mLU( l, n ); // L(m,n) -= L(m,l) * U(l,n)

                mP[n] = n;
                for( unsigned char m = n + 1; m != N; ++m )
                    if( abs( mLU( m, n ) ) > abs( mLU( mP[n], n ) ) )
                        mP[n] = m;

                swap_ranges( &mLU( n, 0 ), &mLU( n, 0 ) + N, &mLU( mP[n], 0 ) );

                for( unsigned char m = n + 1; m != N; ++m )
                    mLU( m, n ) /= mLU( n, n );
            }
        }

        /// Gives lower matrix (L).
        Tensor< T, N, N > lower() const
        {
            Tensor< T, N, N > res( mLU );
            for( std::size_t n = 0; n != N; ++n )
            {
                res( n, n ) = T( 1 );
                for( std::size_t m = n + 1; m != N; ++m )
                    res( n, m ) = T( 0 );
            }
            return res;
        }

        /// Gives upper matrix (U).
        Tensor< T, N, N > upper() const
        {
            Tensor< T, N, N > res( mLU );
            for( std::size_t n = 0; n != N; ++n )
                for( std::size_t m = 0; m != n; ++m )
                    res( n, m ) = T( 0 );

            return res;
        }

        /// Gives permutation matrix (P).
        Tensor< T, N, N > perm() const
        {
            using std::swap;

            char p[N];
            for( std::size_t n = 0; n != N; ++n )
                p[n] = n;

            for( std::size_t n = 0; n != N; ++n )
                swap( p[n], p[std::size_t( mP[n] )] );

            Tensor< T, N, N > res;
            for( std::size_t n = 0; n != N; ++n )
                res( n, p[n] ) = T( 1 );

            return res;
        }

        /// Gives the determinant of A.
        T determinant() const
        {
            T det( 1 );
            for( std::size_t n = 0; n != N; ++n )
                det *= ( mP[n] == n ? mLU( n, n ) : -mLU( n, n ) );

            return det;
        }

        /// Find x which satisfies Ax = b.
        template < std::size_t... M >
        Tensor< T, N, M... > solve( Tensor< T, N, M... > b ) const
        {
            solve_inplace( b );
            return b;
        }

        /// Find x which satisfies Ax = b and write it back on b.
        template < std::size_t... M >
        void solve_inplace( Tensor< T, N, M... >& b ) const
        {
            using std::swap_ranges;

            // apply permutation
            for( std::size_t n = 0; n != N; ++n )
                swap_ranges( b.begin() + n * fantom_private::multi_array< T, M... >::size,
                             b.begin() + ( n + 1 ) * fantom_private::multi_array< T, M... >::size,
                             b.begin() + mP[n] * fantom_private::multi_array< T, M... >::size );

            // solve Ly=b by forward substitution
            for( std::size_t n = 0; n != N; ++n )
                for( std::size_t m = 0; m != n; ++m )
                    for( std::size_t l = 0; l != fantom_private::multi_array< T, M... >::size; ++l )
                        b.begin()[n * fantom_private::multi_array< T, M... >::size + l]
                            -= mLU( n, m ) * b.begin()[m * fantom_private::multi_array< T, M... >::size + l];

            // solve Ux=y by back substitution
            std::size_t n = N;
            while( n != 0 )
            {
                --n;
                for( std::size_t m = n + 1; m != N; ++m )
                    for( std::size_t l = 0; l != fantom_private::multi_array< T, M... >::size; ++l )
                        b.begin()[n * fantom_private::multi_array< T, M... >::size + l]
                            -= mLU( n, m ) * b.begin()[m * fantom_private::multi_array< T, M... >::size + l];

                for( std::size_t l = 0; l != fantom_private::multi_array< T, M... >::size; ++l )
                    b.begin()[n * fantom_private::multi_array< T, M... >::size + l] /= mLU( n, n );
            }
        }
    };

    /// Integer \ingroup math
    typedef int64_t Integer;

    /// Scalar \ingroup math
    typedef Tensor< double > Scalar;

    /// Vector \ingroup math
    template < size_t D >
    using Vector = Tensor< double, D >;

    /// float Vector \ingroup graphics
    template < size_t D >
    using VectorF = Tensor< float, D >;

    /// Point \ingroup math
    template < size_t D >
    using Point = Tensor< double, D >;

    /// float Vector \ingroup graphics
    template < size_t D >
    using PointF = Tensor< float, D >;

    /// Matrix \ingroup math
    template < size_t D >
    using Matrix = Tensor< double, D, D >;

    /// Matrix \ingroup graphics
    template < size_t D >
    using MatrixF = Tensor< float, D, D >;

    /// 1-D vector \ingroup math
    typedef Tensor< double, 1 > Vector1;

    /// 2-D vector \ingroup math
    typedef Tensor< double, 2 > Vector2;

    /// 2-D vector with float precision \ingroup GUI
    typedef Tensor< float, 2 > Vector2F;

    /// 3-D vector with float precision \ingroup GUI
    using Vector3F = Tensor< float, 3 >;

    /// 3-D vector \ingroup math
    typedef Tensor< double, 3 > Vector3;

    /// 4-D vector \ingroup math
    typedef Tensor< double, 4 > Vector4;

    /// 1-D point \ingroup math
    typedef Tensor< double, 1 > Point1;

    /// 2-D point \ingroup math
    typedef Tensor< double, 2 > Point2;

    /// 2-D point with float precision \ingroup GUI
    typedef Tensor< float, 2 > Point2F;

    /// 3-D point \ingroup math
    typedef Tensor< double, 3 > Point3;

    /// 4-D point \ingroup math
    typedef Tensor< double, 4 > Point4;

    /// 1-D matrix \ingroup math
    typedef Tensor< double, 1, 1 > Matrix1;

    /// 2-D matrix \ingroup math
    typedef Tensor< double, 2, 2 > Matrix2;

    /// 3-D matrix \ingroup math
    typedef Tensor< double, 3, 3 > Matrix3;

    /// 3-D matrix with float precision \ingroup GUI
    typedef Tensor< float, 3, 3 > Matrix3F;

    /// 4-D matrix \ingroup math
    typedef Tensor< double, 4, 4 > Matrix4;

    typedef Tensor< int, 2 > Pos2D;
    typedef Tensor< size_t, 2 > Size2D;
    typedef Tensor< int, 3 > Pos3D;
    typedef Tensor< size_t, 3 > Size3D;


    // explicit conversion operators between basic math types

    /**
     * convert a point of any dimension to a point of dimension 3 as it is useful
     * for converting point from 1D and 2D grids to points 3D points for using
     * it in graphics.
     *
     * If Dim == 3, this is the identity operation.
     * If Dim > 3, the additional coordinates are discarded and the vector is projected
     * onto the x - y plane.
     *
     * \tparam BaseType the mathematical base type of the points
     * \tparam Dim the dimension of the original space
     *
     * \param rhs the Point to convert
     * \returns a 3D representation of the point rhs
     */
    template < class BaseType, std::size_t Dim >
    inline Tensor< BaseType, 3 > toPoint3( const Tensor< BaseType, Dim >& rhs )
    {
        return Tensor< BaseType, 3 >( rhs[0], Dim > 1 ? rhs[1] : BaseType(), Dim > 2 ? rhs[2] : BaseType() );
    }

    /**
     * convert a point of any dimension to a point of dimension 3 as it is useful
     * for converting point from 1D and 2D grids to points 2D points for using
     * it in graphics.
     *
     * If Dim == 2, this is the identity operation.
     * If Dim > 2, the additional coordinates are discarded and the vector is projected
     * onto the x - y plane.
     *
     * \tparam BaseType the mathematical base type of the points
     * \tparam Dim the dimension of the original space
     *
     * \param rhs the Point to convert
     * \returns a 2D representation of the point rhs
     */
    template < class BaseType, std::size_t Dim >
    inline Tensor< BaseType, 2 > toPoint2( const Tensor< BaseType, Dim >& rhs )
    {
        return Tensor< BaseType, 2 >( rhs[0], Dim > 1 ? rhs[1] : BaseType() );
    }

    /**
     * convert a point of any dimension to a point of dimension 3 as it is useful
     * for converting point from 1D and 2D grids to points 2D points for using
     * it in graphics.
     *
     * If DimFrom == DimTo, this is the identity operation.
     * If DimFrom > DimTo, the additional coordinates are discarded and the vector is projected
     * onto the lower-dimensional subspace.
     *
     * \tparam BaseType the mathematical base type of the points
     * \tparam DimFrom the dimension of the original space
     * \tparam DimTo the dimension of the target space
     *
     * \param rhs the Point to convert
     * \returns a DimTo-D representation of the point rhs
     *
     * \sa toPoint2
     * \sa toPoint3
     */
    template < std::size_t DimTo, class BaseType, std::size_t DimFrom >
    inline Tensor< BaseType, DimTo > toPoint( const Tensor< BaseType, DimFrom >& rhs )
    {
        Tensor< BaseType, DimTo > point;
        for( size_t i = 0; i < std::min( DimTo, DimFrom ); ++i )
        {
            point[i] = rhs[i];
        }
        return point;
    }

    /**
     * convert a vector of any dimension to a vector of dimension 3 as it is useful
     * for converting vectors from 1D and 2D grids to vector 3D vectors for using
     * it in graphics.
     *
     * If Dim == 3, this is the identity operation.
     * If Dim > 3, the additional coordinates are discarded and the vector is projected
     * onto the x - y plane.
     *
     * \tparam BaseType the mathematical base type of the vectors
     * \tparam Dim the dimension of the original space
     *
     * \param rhs the vector to convert
     * \returns a 3D representation of the vector rhs
     *
     * \sa toVector2
     * \sa toPoint2
     * \sa toPoint3
     */
    template < class BaseType, std::size_t Dim >
    inline Tensor< BaseType, 3 > toVector3( const Tensor< BaseType, Dim >& rhs )
    {
        return Tensor< BaseType, 3 >( rhs[0], Dim > 1 ? rhs[1] : BaseType(), Dim > 2 ? rhs[2] : BaseType() );
    }

    /**
     * convert a vector of any dimension to a vector of dimension 3
     *
     * If Dim == 2, this is the identity operation.
     * If Dim > 2, the additional coordinates are discarded and the vector is projected
     * onto the x - y plane.
     *
     * \tparam BaseType the mathematical base type of the vector
     * \tparam Dim the dimension of the original space
     *
     * \param rhs the Point to convert
     * \returns a 2D representation of the vector rhs
     *
     * \sa toVector3
     * \sa toPoint2
     * \sa toPoint3
     */
    template < class BaseType, std::size_t Dim >
    inline Tensor< BaseType, 2 > toVector2( const Tensor< BaseType, Dim >& rhs )
    {
        return Tensor< BaseType, 2 >( rhs[0], Dim > 1 ? rhs[1] : BaseType() );
    }

    /**
     * convert a vector of any dimension to a vector of dimension 3 as it is useful
     * for converting vector from 1D and 2D grids to vector 2D vector for using
     * it in graphics.
     *
     * If DimFrom == DimTo, this is the identity operation.
     * If DimFrom > DimTo, the additional coordinates are discarded and the vector is projected
     * onto the lower-dimensional subspace.
     *
     * \tparam BaseType the mathematical base type of the vector
     * \tparam DimFrom the dimension of the original space
     * \tparam DimTo the dimension of the target space
     *
     * \param rhs the Point to convert
     * \returns a DimTo-D representation of the vector rhs
     *
     * \sa toVector2
     * \sa toVector3
     * \sa toPoint
     * \sa toPoint2
     * \sa toPoint3
     */
    template < std::size_t DimTo, class BaseType = double, std::size_t DimFrom >
    inline Tensor< BaseType, DimTo > toVector( const Tensor< BaseType, DimFrom >& rhs )
    {
        Tensor< BaseType, DimTo > point;
        for( size_t i = 0; i < std::min( DimTo, DimFrom ); ++i )
        {
            point[i] = rhs[i];
        }
        return point;
    }


    /// Compares a tensor for equality.
    template < class T1, class T2, std::size_t... N >
    bool operator==( const Tensor< T1, N... >& lhs, const Tensor< T2, N... >& rhs )
    {
        return std::equal( lhs.begin(), lhs.end(), rhs.begin() );
    }

    /// Compares a tensor for inequality.
    template < class T1, class T2, std::size_t... N >
    bool operator!=( const Tensor< T1, N... >& lhs, const Tensor< T2, N... >& rhs )
    {
        return !( lhs == rhs );
    }

    /// Compare scalars.
    template < class T1, class T2 >
    bool operator<( const Tensor< T1 >& lhs, const Tensor< T2 >& rhs )
    {
        return lhs() < rhs();
    }

    /// Compare scalars.
    template < class T1, class T2 >
    bool operator<=( const Tensor< T1 >& lhs, const Tensor< T2 >& rhs )
    {
        return lhs() <= rhs();
    }

    /// Compare scalars.
    template < class T1, class T2 >
    bool operator>( const Tensor< T1 >& lhs, const Tensor< T2 >& rhs )
    {
        return lhs() > rhs();
    }

    /// Compare scalars.
    template < class T1, class T2 >
    bool operator>=( const Tensor< T1 >& lhs, const Tensor< T2 >& rhs )
    {
        return lhs() >= rhs();
    }

    /// Adds another tensor component-wise.
    template < class T1, class T2, std::size_t... N >
    Tensor< T1, N... >& operator+=( Tensor< T1, N... >& lhs, const Tensor< T2, N... >& rhs )
    {
        for( std::size_t i = 0; i != Tensor< T1, N... >::size; ++i )
            lhs.begin()[i] += rhs.begin()[i];
        return lhs;
    }

    /// Subtracts another tensor component-wise.
    template < class T1, class T2, std::size_t... N >
    Tensor< T1, N... >& operator-=( Tensor< T1, N... >& lhs, const Tensor< T2, N... >& rhs )
    {
        for( std::size_t i = 0; i != Tensor< T1, N... >::size; ++i )
            lhs.begin()[i] -= rhs.begin()[i];
        return lhs;
    }

    /// Multiplies a scalar component-wise.
    template < class T1, class T2, std::size_t... N >
    Tensor< T1, N... >& operator*=( Tensor< T1, N... >& lhs, const Tensor< T2 >& rhs )
    {
        for( std::size_t i = 0; i != Tensor< T1, N... >::size; ++i )
            lhs.begin()[i] *= rhs();
        return lhs;
    }

    /// Divides a scalar component-wise.
    template < class T1, class T2, std::size_t... N >
    Tensor< T1, N... >& operator/=( Tensor< T1, N... >& lhs, const Tensor< T2 >& rhs )
    {
        for( std::size_t i = 0; i != Tensor< T1, N... >::size; ++i )
            lhs.begin()[i] /= rhs();
        return lhs;
    }

    /// Multiplies a constant component-wise.
    template < class T1, class T2, std::size_t... N >
    Tensor< T1, N... >& operator*=( Tensor< T1, N... >& lhs, const T2& rhs )
    {
        for( std::size_t i = 0; i != Tensor< T1, N... >::size; ++i )
            lhs.begin()[i] *= rhs;
        return lhs;
    }

    /// Divides a constant component-wise.
    template < class T1, class T2, std::size_t... N >
    Tensor< T1, N... >& operator/=( Tensor< T1, N... >& lhs, const T2& rhs )
    {
        for( std::size_t i = 0; i != Tensor< T1, N... >::size; ++i )
            lhs.begin()[i] /= rhs;
        return lhs;
    }

    /// Positivates tensor component-wise.
    template < class T, std::size_t... N >
    Tensor< T, N... > operator+( const Tensor< T, N... >& rhs )
    {
        using namespace fantom_private;
        return Tensor< T, N... >( makePositivate( makeIteration( rhs.begin() ) ) );
    }

    /// Negates tensor component-wise.
    template < class T, std::size_t... N >
    Tensor< T, N... > operator-( const Tensor< T, N... >& rhs )
    {
        using namespace fantom_private;
        return Tensor< T, N... >( makeNegate( makeIteration( rhs.begin() ) ) );
    }

    /// Adds two tensors component-wise.
    template < class T1, class T2, std::size_t... N >
    Tensor< decltype( T1() + T2() ), N... > operator+( const Tensor< T1, N... >& lhs, const Tensor< T2, N... >& rhs )
    {
        using namespace fantom_private;
        return Tensor< decltype( T1() + T2() ), N... >(
            makePlus( makeIteration( lhs.begin() ), makeIteration( rhs.begin() ) ) );
    }

    /// Subtracts two tensors component-wise.
    template < class T1, class T2, std::size_t... N >
    Tensor< decltype( T1() - T2() ), N... > operator-( const Tensor< T1, N... >& lhs, const Tensor< T2, N... >& rhs )
    {
        using namespace fantom_private;
        return Tensor< decltype( T1() - T2() ), N... >(
            makeMinus( makeIteration( lhs.begin() ), makeIteration( rhs.begin() ) ) );
    }

    /// Multiplies a scalar and a tensor component-wise.
    template < class T1, class T2, size_t N0, std::size_t... N >
    Tensor< decltype( T1() * T2() ), N0, N... > operator*( const Tensor< T1 >& lhs, const Tensor< T2, N0, N... >& rhs )
    {
        using namespace fantom_private;
        return Tensor< decltype( T1() * T2() ), N0, N... >(
            makeMultiplies( makeConstant( lhs() ), makeIteration( rhs.begin() ) ) );
    }

    /// Multiplies a tensor and a scalar component-wise.
    template < class T1, class T2, std::size_t... N >
    Tensor< decltype( T1() * T2() ), N... > operator*( const Tensor< T1, N... >& lhs, const Tensor< T2 >& rhs )
    {
        using namespace fantom_private;
        return Tensor< decltype( T1() * T2() ), N... >(
            makeMultiplies( makeIteration( lhs.begin() ), makeConstant( rhs() ) ) );
    }

    /// Divides a tensor and a scalar component-wise.
    template < class T1, class T2, std::size_t... N >
    Tensor< decltype( T1() / T2() ), N... > operator/( const Tensor< T1, N... >& lhs, const Tensor< T2 >& rhs )
    {
        using namespace fantom_private;
        return Tensor< decltype( T1() + T2() ), N... >(
            makeDivides( makeIteration( lhs.begin() ), makeConstant( rhs() ) ) );
    }

    /// Multiplies a constant and a tensor component-wise.
    template < class T1, class T2, std::size_t... N >
    Tensor< decltype( T1() * T2() ), N... > operator*( const T1& lhs, const Tensor< T2, N... >& rhs )
    {
        using namespace fantom_private;
        return Tensor< decltype( T1() * T2() ), N... >(
            makeMultiplies( makeConstant( lhs ), makeIteration( rhs.begin() ) ) );
    }

    /// Multiplies a tensor and a constant component-wise.
    template < class T1, class T2, std::size_t... N >
    Tensor< decltype( T1() * T2() ), N... > operator*( const Tensor< T1, N... >& lhs, const T2& rhs )
    {
        using namespace fantom_private;
        return Tensor< decltype( T1() * T2() ), N... >(
            makeMultiplies( makeIteration( lhs.begin() ), makeConstant( rhs ) ) );
    }

    /// Divides a tensor and a constant component-wise.
    template < class T1, class T2, std::size_t... N >
    Tensor< decltype( T1() / T2() ), N... > operator/( const Tensor< T1, N... >& lhs, const T2& rhs )
    {
        using namespace fantom_private;
        return Tensor< decltype( T1() / T2() ), N... >(
            makeDivides( makeIteration( lhs.begin() ), makeConstant( rhs ) ) );
    }

    /// Computes the inner product (dot product, scalar product) of two vectors.
    template < class T1, class T2, std::size_t N >
    decltype( T1() * T2() ) operator*( const Tensor< T1, N >& lhs, const Tensor< T2, N >& rhs )
    {
        typedef decltype( T1() * T2() ) T;
        T t = T();
        for( std::size_t i = 0; i != N; ++i )
            t += lhs( i ) * rhs( i );
        return t;
    }

    /// Multiplies two matrices.
    template < class T1, class T2, std::size_t M, std::size_t N, std::size_t L >
    Tensor< decltype( T1() * T2() ), N, M > operator*( const Tensor< T1, N, L >& lhs, const Tensor< T2, L, M >& rhs )
    {
        Tensor< decltype( T1() * T2() ), N, M > tmp;
        for( std::size_t n = 0; n != N; ++n )
            for( std::size_t m = 0; m != M; ++m )
                for( std::size_t l = 0; l != L; ++l )
                    tmp( n, m ) += lhs( n, l ) * rhs( l, m );

        return tmp;
    }

    /// Multiplies a matrix and a vector. \ingroup math
    template < class T1, class T2, std::size_t N, std::size_t M >
    Tensor< decltype( T1() * T2() ), N > operator*( const Tensor< T1, N, M >& lhs, const Tensor< T2, M >& rhs )
    {
        Tensor< decltype( T1() * T2() ), N > tmp;
        for( std::size_t n = 0; n != N; ++n )
            for( std::size_t m = 0; m != M; ++m )
                tmp( n ) += lhs( n, m ) * rhs( m );

        return tmp;
    }

    /// Multiplies a vector and a matrix. \ingroup math
    template < class T1, class T2, std::size_t N, std::size_t M >
    Tensor< decltype( T1() * T2() ), M > operator*( const Tensor< T1, N >& lhs, const Tensor< T2, N, M >& rhs )
    {
        Tensor< decltype( T1() * T2() ), M > tmp;
        for( std::size_t m = 0; m != M; ++m )
            for( std::size_t n = 0; n != N; ++n )
                tmp( m ) += lhs( n ) * rhs( n, m );

        return tmp;
    }

    /// Prints tensor. \ingroup math
    template < class T, std::size_t... N >
    std::ostream& operator<<( std::ostream& os, const Tensor< T, N... >& v )
    {
        os << "[" << v.begin()[0];
        for( std::size_t i = 1; i != Tensor< T, N... >::size; ++i )
            os << ", " << v.begin()[i];
        return os << "]";
    }

    /// Gives a PA=LU factorization of a square matrix A.
    template < class T, std::size_t N >
    LUPFactorization< T, N > makeLUPFactorization( const Tensor< T, N, N >& matrix )
    {
        return LUPFactorization< T, N >( matrix );
    }

    /// Computes the area spanned by two 2-D vectors. \ingroup math
    /**
     * \sa template <class T1, class T2> const Tensor<decltype(T1()*T2()), 3> cross(const Tensor<T1,
     * 3>& lhs, const
     * Tensor<T2, 3>& rhs)
     */
    template < class T1, class T2 >
    decltype( T1() * T2() ) area( const Tensor< T1, 2 >& lhs, const Tensor< T2, 2 >& rhs )
    {
        return lhs( 0 ) * rhs( 1 ) - lhs( 1 ) * rhs( 0 );
    }

    /// Computes complex conjugate of tensor. \ingroup math
    template < class T, std::size_t... N >
    Tensor< T, N... > conj( const Tensor< T, N... >& rhs )
    {
        using namespace fantom_private;
        return Tensor< T, N... >( makeConjugate( makeIteration( rhs.begin() ) ) );
    }

    /// Computes the cross product (outer product) of two 3-D vectors. \ingroup math
    /**
     * \sa template <class T1, class T2> const decltype(T1()*T2()) area(const Tensor<T1, 2>& lhs,
     * const Tensor<T2, 2>&
     * rhs)
     */
    template < class T1, class T2 >
    Tensor< decltype( T1() * T2() ), 3 > cross( const Tensor< T1, 3 >& lhs, const Tensor< T2, 3 >& rhs )
    {
        return Tensor< decltype( T1() * T2() ), 3 >( lhs( 1 ) * rhs( 2 ) - lhs( 2 ) * rhs( 1 ),
                                                     lhs( 2 ) * rhs( 0 ) - lhs( 0 ) * rhs( 2 ),
                                                     lhs( 0 ) * rhs( 1 ) - lhs( 1 ) * rhs( 0 ) );
    }

    /// Computes matrix determinant.
    template < class T >
    T determinant( const Tensor< T, 1, 1 >& lhs )
    {
        return lhs( 0, 0 );
    }

    /// Computes matrix determinant.
    template < class T >
    T determinant( const Tensor< T, 2, 2 >& lhs )
    {
        return lhs( 0, 0 ) * lhs( 1, 1 ) - lhs( 0, 1 ) * lhs( 1, 0 );
    }

    /// Computes matrix determinant.
    template < class T >
    T determinant( const Tensor< T, 3, 3 >& lhs )
    {
        return lhs( 0, 0 ) * lhs( 1, 1 ) * lhs( 2, 2 ) + lhs( 0, 1 ) * lhs( 1, 2 ) * lhs( 2, 0 )
               + lhs( 0, 2 ) * lhs( 1, 0 ) * lhs( 2, 1 ) - lhs( 0, 0 ) * lhs( 1, 2 ) * lhs( 2, 1 )
               - lhs( 0, 1 ) * lhs( 1, 0 ) * lhs( 2, 2 ) - lhs( 0, 2 ) * lhs( 1, 1 ) * lhs( 2, 0 );
    }

    /// Computes matrix determinant. \ingroup math
    template < class T, std::size_t N >
    T determinant( const Tensor< T, N, N >& lhs )
    {
        return makeLUPFactorization( lhs ).determinant();
    }

    /// Computes the trace of a matrix. \ingroup math
    template < class T, std::size_t N >
    T trace( const Tensor< T, N, N >& lhs )
    {
        T result = T();
        for( size_t i = 0; i < N; ++i )
        {
            result += lhs( i, i );
        }
        return result;
    }

    /// Computes matrix discriminant. \ingroup math
    template < class T >
    T discriminant( const Tensor< T, 3, 3 >& lhs )
    {
        // see Roth (Page 143) !

        double P = -trace( lhs );
        double Q = lhs( 0, 0 ) * lhs( 1, 1 ) - lhs( 0, 1 ) * lhs( 1, 0 ) + lhs( 0, 0 ) * lhs( 2, 2 )
                   - lhs( 0, 2 ) * lhs( 2, 0 ) + lhs( 1, 1 ) * lhs( 2, 2 ) - lhs( 1, 2 ) * lhs( 2, 1 );
        double R = -determinant( lhs );

        double q = Q / 3 - ( P * P ) / 9;
        double r = P * Q / 6 - R / 2 - ( P * P * P ) / 27;

        return q * q * q + r * r;
    }

    /// @copydoc discriminant( const Tensor< T, 3, 3>& )
    template < class T >
    T discriminant( const Tensor< T, 2, 2 >& lhs )
    {
        double b = lhs( 0, 0 ) + lhs( 1, 1 );
        double c = lhs( 0, 0 ) * lhs( 1, 1 ) - lhs( 0, 1 ) * lhs( 1, 0 );
        return b * b - 4 * 1 * c;
    }

    /// Computes matrix inverse.
    template < class T >
    Tensor< T, 1, 1 > inverse( const Tensor< T, 1, 1 >& lhs )
    {
        T D = T( 1 ) / lhs( 0, 0 );
        return Tensor< T, 1, 1 >( &D );
    }

    /// Computes matrix inverse.
    template < class T >
    Tensor< T, 2, 2 > inverse( const Tensor< T, 2, 2 >& lhs )
    {
        using namespace fantom_private;
        T D = determinant( lhs ), t[] = { lhs( 1, 1 ) / D, -lhs( 0, 1 ) / D, -lhs( 1, 0 ) / D, lhs( 0, 0 ) / D };
        return Tensor< T, 2, 2 >( t );
    }

    /// Computes matrix inverse.
    template < class T >
    Tensor< T, 3, 3 > inverse( const Tensor< T, 3, 3 >& lhs )
    {
        Tensor< T, 3, 3 > tmp;

        tmp( 0, 0 ) = ( lhs( 1, 1 ) * lhs( 2, 2 ) - lhs( 2, 1 ) * lhs( 1, 2 ) );
        tmp( 0, 1 ) = -( lhs( 0, 1 ) * lhs( 2, 2 ) - lhs( 2, 1 ) * lhs( 0, 2 ) );
        tmp( 0, 2 ) = ( lhs( 0, 1 ) * lhs( 1, 2 ) - lhs( 1, 1 ) * lhs( 0, 2 ) );

        double D = tmp( 0, 0 ) * lhs( 0, 0 ) + tmp( 0, 1 ) * lhs( 1, 0 ) + tmp( 0, 2 ) * lhs( 2, 0 );

        tmp( 0, 0 ) /= D;
        tmp( 0, 1 ) /= D;
        tmp( 0, 2 ) /= D;
        tmp( 1, 0 ) = -( lhs( 1, 0 ) * lhs( 2, 2 ) - lhs( 2, 0 ) * lhs( 1, 2 ) ) / D;
        tmp( 1, 1 ) = ( lhs( 0, 0 ) * lhs( 2, 2 ) - lhs( 2, 0 ) * lhs( 0, 2 ) ) / D;
        tmp( 1, 2 ) = -( lhs( 0, 0 ) * lhs( 1, 2 ) - lhs( 1, 0 ) * lhs( 0, 2 ) ) / D;
        tmp( 2, 0 ) = ( lhs( 1, 0 ) * lhs( 2, 1 ) - lhs( 2, 0 ) * lhs( 1, 1 ) ) / D;
        tmp( 2, 1 ) = -( lhs( 0, 0 ) * lhs( 2, 1 ) - lhs( 2, 0 ) * lhs( 0, 1 ) ) / D;
        tmp( 2, 2 ) = ( lhs( 0, 0 ) * lhs( 1, 1 ) - lhs( 1, 0 ) * lhs( 0, 1 ) ) / D;

        return tmp;
    }

    /// Computes matrix inverse. \ingroup math
    template < class T, std::size_t N >
    Tensor< T, N, N > inverse( const Tensor< T, N, N >& lhs )
    {
        Tensor< T, N, N > I;
        for( std::size_t n = 0; n != N; ++n )
            I( n, n ) = T( 1 );

        return makeLUPFactorization( lhs ).solve( I );
    }

    /// Computes the squared norm of a tensor. \ingroup math
    /**
     * @note this is the entry-wise norm and not the spectral norm of the matrix
     */
    template < class T, std::size_t... N >
    decltype( norm2( T() ) ) norm2( const Tensor< T, N... >& lhs )
    {
        typedef decltype( norm2( T() ) ) R;
        return std::accumulate( lhs.begin(), lhs.end(), R(), fantom_private::sum_norm2< T >() );
    }

    /// Computes the norm of a tensor. \ingroup math
    /**
     * @note this is the entry-wise norm and not the spectral norm of the matrix
     */
    template < class T, std::size_t... N >
    decltype( norm2( T() ) ) norm( const Tensor< T, N... >& lhs )
    {
        using std::sqrt;
        return sqrt( norm2( lhs ) );
    }

    /**
     * Returns
     * \f$ \| A\|_1 = \max_{0 \geq j < N} \sum_{i=0}^{M-1} |a_{ij}| \f$
     * which is the maximum absolute column sum
     */
    template < class T, std::size_t M, std::size_t N >
    decltype( norm2( T() ) ) matrix1Norm( const Tensor< T, M, N >& m )
    {
        typedef decltype( norm2( T() ) ) RT;
        RT R = RT{};
        for( size_t j = 0; j < N; ++j )
        {
            RT tmpR = RT{};
            for( size_t i = 0; i < M; ++i )
            {
                tmpR += std::abs( m( i, j ) );
            }
            if( tmpR > R )
            {
                R = tmpR;
            }
        }
        return R;
    }

    /**
     * Returns
     * \f$ \| A\|_1 = \max_{0 \geq i < M} \sum_{j=0}^{N-1} |a_{ij}| \f$
     * which is the maximum absolute row sum
     */
    template < class T, std::size_t M, std::size_t N >
    decltype( norm2( T() ) ) matrixInftyNorm( const Tensor< T, M, N >& m )
    {
        typedef decltype( norm2( T() ) ) RT;
        RT R = RT{};
        for( size_t i = 0; i < N; ++i )
        {
            RT tmpR = RT{};
            for( size_t j = 0; j < M; ++j )
            {
                tmpR += std::abs( m( i, j ) );
            }
            if( tmpR > R )
            {
                R = tmpR;
            }
        }
        return R;
    }


#ifdef __clang__
// float == 0 is considered unsafe, ignore the warning
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#endif
    /// Computes the normalized tensor. \ingroup math
    template < class T, std::size_t... N >
    Tensor< T, N... > normalized( const Tensor< T, N... >& lhs )
    {
        return norm( lhs ) == 0 ? lhs : lhs / norm( lhs );
    }
#ifdef __clang__
#pragma clang diagnostic pop
#endif

    Vector2 normalized( const Vector2& vector );

    Vector3 normalized( const Vector3& vector );

    /// Convert a 3-dimensional vector into homogeneous coordingates
    template < typename TType >
    inline Tensor< TType, 4 > homogenize( Tensor< TType, 3 > const& vec )
    {
        return Tensor< TType, 4 >( vec( 0 ), vec( 1 ), vec( 2 ), 1 );
    }

    template < typename TType >
    inline Tensor< TType, 3 > homogenize( Tensor< TType, 4 > const& vec )
    {
        return Tensor< TType, 3 >( vec( 0 ) / vec( 3 ), vec( 1 ) / vec( 3 ), vec( 2 ) / vec( 3 ) );
    }


    /// Computes the transposed matrix. \ingroup math
    template < class T, std::size_t N, std::size_t M >
    Tensor< T, M, N > transposed( const Tensor< T, N, M >& lhs )
    {
        Tensor< T, M, N > result;
        for( std::size_t m = 0; m != M; ++m )
            for( std::size_t n = 0; n != N; ++n )
                result( m, n ) = lhs( n, m );

        return result;
    }


    /// Create an identity matrix. \ingroup math
    template < class T, std::size_t N >
    Tensor< T, N, N > makeIdentity()
    {
        Tensor< T, N, N > out;
        for( size_t i = 0; i < N; ++i )
            out( i, i ) = 1.0;
        return out;
    }

    /// Create a scale matrix. \ingroup math
    template < class T >
    Tensor< T, 3, 3 > makeScale( const Tensor< T, 3 >& scale )
    {
        Tensor< T, 3, 3 > out = makeIdentity< T, 3 >();
        for( size_t i = 0; i < 3; ++i )
            out( i, i ) = scale( i );
        return out;
    }

    /// Create a scale matrix. \ingroup math
    template < class T >
    Tensor< T, 4, 4 > makeScaleH( const Tensor< T, 3 >& scale )
    {
        Tensor< T, 4, 4 > out = makeIdentity< T, 4 >();
        for( size_t i = 0; i < 3; ++i )
            out( i, i ) = scale( i );
        return out;
    }

    /// Create a scale matrix. \ingroup math
    template < class T >
    Tensor< T, 4, 4 > makeScaleH( T scale )
    {
        Tensor< T, 4, 4 > out = makeIdentity< T, 4 >();
        for( size_t i = 0; i < 3; ++i )
            out( i, i ) = scale;
        return out;
    }

    /// Create a translation matrix using homogeneous coordinates. \ingroup math
    /// It is meant for multiplication from the left.
    template < class T >
    Tensor< T, 4, 4 > makeTranslationH( const Tensor< T, 3 >& offset )
    {
        Tensor< T, 4, 4 > out = makeIdentity< T, 4 >();
        for( size_t i = 0; i < 3; ++i )
            out( i, 3 ) = offset( i );
        return out;
    }

    /// Create a rotation matrix using homogeneous coordinates. \ingroup math
    /// \pre axis is a unit vector
    template < class T >
    Tensor< T, 4, 4 > makeRotationH( const Tensor< T, 3 >& axis, T angle )
    {
        Tensor< T, 4, 4 > out;
        T cosA = std::cos( angle );
        T sinA = std::sin( angle );
        out( 0, 0 ) = cosA + axis( 0 ) * axis( 0 ) * ( 1 - cosA );
        out( 0, 1 ) = axis( 0 ) * axis( 1 ) * ( 1 - cosA ) - axis( 2 ) * sinA;
        out( 0, 2 ) = axis( 0 ) * axis( 2 ) * ( 1 - cosA ) - axis( 1 ) * sinA;
        out( 0, 3 ) = 0;
        out( 1, 0 ) = axis( 0 ) * axis( 1 ) * ( 1 - cosA ) + axis( 2 ) * sinA;
        out( 1, 1 ) = cosA + axis( 1 ) * axis( 1 ) * ( 1 - cosA );
        out( 1, 2 ) = axis( 1 ) * axis( 2 ) * ( 1 - cosA ) - axis( 0 ) * sinA;
        out( 1, 3 ) = 0;
        out( 2, 0 ) = axis( 0 ) * axis( 2 ) * ( 1 - cosA ) - axis( 1 ) * sinA;
        out( 2, 1 ) = axis( 1 ) * axis( 2 ) * ( 1 - cosA ) + axis( 0 ) * sinA;
        out( 2, 2 ) = cosA + axis( 2 ) * axis( 2 ) * ( 1 - cosA );
        out( 2, 3 ) = 0;
        out( 3, 0 ) = 0;
        out( 3, 1 ) = 0;
        out( 3, 2 ) = 0;
        out( 3, 3 ) = 1;
        return out;
    }

    /// Create a rotation matrix using homogeneous coordinates. \ingroup math
    template < typename T >
    Tensor< T, 3, 3 > makeRotation( Quaternion< T > const& quat )
    {
        Tensor< T, 3, 3 > result;

        T x2 = quat.x() * quat.x();
        T y2 = quat.y() * quat.y();
        T z2 = quat.z() * quat.z();
        T xy = quat.x() * quat.y();
        T xz = quat.x() * quat.z();
        T yz = quat.y() * quat.z();
        T wx = quat.w() * quat.x();
        T wy = quat.w() * quat.y();
        T wz = quat.w() * quat.z();

        result( 0, 0 ) = T( 1 ) - T( 2 ) * ( y2 + z2 );
        result( 0, 1 ) = T( 2 ) * ( xy - wz );
        result( 0, 2 ) = T( 2 ) * ( xz + wy );
        result( 1, 0 ) = T( 2 ) * ( xy + wz );
        result( 1, 1 ) = T( 1 ) - T( 2 ) * ( x2 + z2 );
        result( 1, 2 ) = T( 2 ) * ( yz - wx );
        result( 2, 0 ) = T( 2 ) * ( xz - wy );
        result( 2, 1 ) = T( 2 ) * ( yz + wx );
        result( 2, 2 ) = T( 1 ) - T( 2 ) * ( x2 + y2 );

        return result;
    }

    /// Create a rotation matrix using homogeneous coordinates. \ingroup math
    template < typename T >
    Tensor< T, 4, 4 > makeRotationH( Quaternion< T > const& quat )
    {
        Tensor< T, 4, 4 > result;

        T x2 = quat.x() * quat.x();
        T y2 = quat.y() * quat.y();
        T z2 = quat.z() * quat.z();
        T xy = quat.x() * quat.y();
        T xz = quat.x() * quat.z();
        T yz = quat.y() * quat.z();
        T wx = quat.w() * quat.x();
        T wy = quat.w() * quat.y();
        T wz = quat.w() * quat.z();

        result( 0, 0 ) = T( 1 ) - T( 2 ) * ( y2 + z2 );
        result( 0, 1 ) = T( 2 ) * ( xy - wz );
        result( 0, 2 ) = T( 2 ) * ( xz + wy );
        result( 0, 3 ) = T( 0 );
        result( 1, 0 ) = T( 2 ) * ( xy + wz );
        result( 1, 1 ) = T( 1 ) - T( 2 ) * ( x2 + z2 );
        result( 1, 2 ) = T( 2 ) * ( yz - wx );
        result( 1, 3 ) = T( 0 );
        result( 2, 0 ) = T( 2 ) * ( xz - wy );
        result( 2, 1 ) = T( 2 ) * ( yz + wx );
        result( 2, 2 ) = T( 1 ) - T( 2 ) * ( x2 + y2 );
        result( 2, 3 ) = T( 0 );
        result( 3, 0 ) = T( 0 );
        result( 3, 1 ) = T( 0 );
        result( 3, 2 ) = T( 0 );
        result( 3, 3 ) = T( 1 );

        return result;
    }


    /// RGBA color \ingroup math
    class Color
    {
    public:
        /// Creates black, non-transparent color
        Color()
            : mR( 0.0f )
            , mG( 0.0f )
            , mB( 0.0f )
            , mA( 1.0f )
        {
        }

        /// Creates color in RGBA space.
        /**
         * \ingroup math
         * \param r red
         * \param g green
         * \param b blue
         * \param a opacity
         * \pre \a r, \a g, \a b, \a a must lie inside [0.0, 1.0].
         */
        Color( float r, float g, float b, float a = 1.0f )
            : mR( r )
            , mG( g )
            , mB( b )
            , mA( a )
        {
            if( r < 0.0f || g < 0.0f || b < 0.0f || a < 0.0f )
            {
                throw fantom::logic_error( "Negative color values are not allowed." );
            }
            if( r > 1.0f || g > 1.0f || b > 1.0f || a > 1.0f )
            {
                throw fantom::logic_error( "Color values greater than 1.0 are not allowed." );
            }
        }

        /// Determines red component.
        float r() const
        {
            return mR;
        }

        /// Determines green component
        float g() const
        {
            return mG;
        }

        /// Determines blue component
        float b() const
        {
            return mB;
        }

        /// Determines alpha component
        float a() const
        {
            return mA;
        }

        bool operator!=( const Color& color )
        {
            return !( *this == color );
        }

        /// equals operator
        bool operator==( const Color& color ) const
        {
// floating point comparison is considered an unsafe operation. As it is supposed
// to only work on copies of the same object here, we can safely ignore the warnings
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#endif
            return mA == color.mA && mR == color.mR && mG == color.mG && mB == color.mB;
#ifdef __clang__
#pragma clang diagnostic pop
#endif
        }

        // operator* is required by the tensor field when interpolating color values
        friend Color operator*( float weight, const Color& rhs );

        // operator+= is required by the tensor field when interpolating color values
        Color& operator+=( const Color& c )
        {
            mR += c.mR;
            mG += c.mG;
            mB += c.mB;
            mA += c.mA;
            return *this;
        }

        /// Return color in HSV format.
        /// \post: h in [0, 360], s, v in [0, 1]
        void getHSV( float& h, float& s, float& v ) const;

        /// Creates a color from HSV format.
        /// \pre: h in [0, 360], s, v in [0, 1]
        static Color createFromHSV( float h, float s, float v, float a = 1.0 );

        // operator+ is required by the tensor field when interpolating color values
        friend Color operator+( const Color& c1, const Color& c2 );

    private:
        float mR, mG, mB, mA;
    };

    /// Prints color. \ingroup math
    std::ostream& operator<<( std::ostream& os, const Color& c );

    inline Color operator*( float weight, const Color& rhs )
    {
        return Color( rhs.r() * weight, rhs.g() * weight, rhs.b() * weight, rhs.a() * weight );
    }

    inline Color operator+( const Color& c1, const Color& c2 )
    {
        Color c( c1 );
        return ( c += c2 );
    }

} // namespace fantom
