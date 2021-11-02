#pragma once

#include "../../../cells.hpp"
#include "../../ValueArray.hpp"
#include "CellDefinitions.hpp"

#include <cassert>
#include <type_traits>

namespace fantom
{
    namespace cells
    {
        // ================================================================================
        // Test for cell type capabilities
        // ================================================================================

        namespace detail
        {
            /// Create a delegate template that simply absorbs the template type.
            template < typename T >
            struct sfinae_true : std::true_type
            {
            };

            /// Try to call the contains method on a dummy object. If that succeeds, this function is existent.
            template < typename CellType, size_t Dim >
            static auto test_contains( int ) -> sfinae_true< decltype(
                std::declval< CellType >().contains( std::declval< double[] >(),
                                                     Tensor< double, Dim >(),
                                                     std::declval< ValueArray< Tensor< double, Dim > > >(),
                                                     std::declval< size_t[] >() ) ) >;

            /// If the call does not succeed, the return type is std::false_type. If the above function is existent,
            /// this function is not selected because of the parameter type.
            template < typename CellType, size_t Dim >
            static auto test_contains( long ) -> std::false_type;

            /// Try to call the interpolate method on a dummy object. If that succeeds, this function is existent.
            template < typename CellType >
            static auto test_interpolate( int ) -> sfinae_true< decltype( std::declval< CellType >().interpolate(
                std::declval< double[] >(), std::declval< const double[] >() ) ) >;

            /// If the call does not succeed, the return type is std::false_type. If the above function is existent,
            /// this function is not selected because of the parameter type.
            template < typename CellType >
            static auto test_interpolate( long ) -> std::false_type;
        }

        /// template helper that determines whether the contains function exists
        /**
         * hasContains has a static const bool member variable called "value" that indicates
         * whether the CellType::contains function exists for the given dimension.
         */
        template < typename CellType, size_t Dim >
        struct hasContains
        {
            static const bool value
                = std::is_base_of< std::true_type, decltype( detail::test_contains< CellType, Dim >( 0 ) ) >::value;
        };

        /// template helper that determines whether the interpolate function exists
        /**
         * hasInterpolate has a static const bool member variable called "value" that indicates
         * whether the CellType::interpolate function exists for the given dimension.
         */
        template < typename CellType, size_t Dim >
        struct hasInterpolate
        {
            static const bool value
                = std::is_base_of< std::true_type, decltype( detail::test_interpolate< CellType >( 0 ) ) >::value;
        };


        // ================================================================================
        // Cell Strategies
        // ================================================================================


        /**
         * \brief
         * The strategy encapsulates the behavior of cells and provides the functionality around
         * the individual cell types.
         */
        struct Strategy : Cell::Strategy
        {
            Strategy( const Cell::Stats& stats );

            /**
             * \return the topological dimension of the cell defined by this strategy
             */
            virtual size_t dimension() const = 0;

            /**
             * the strategy provides means of interpolating in cells
             * This basically is the interface of the abstract cell complex to an embedding of the
             * cell complex in the Euclidean space.
             *
             * Therefore, two functions are provided:
             * contains(...) checks whether a point in \[R^d\] is contained in the cell and returns the internal
             *coordinates
             *of that point.
             * interpolate(...) takes the base index of the cell, computes the cell's vertex indices and
             *     the interpolation parameters (weights), based on the internal coordinates of a position in the cell.
             */
            virtual bool contains( double coordinates[],
                                   size_t base,
                                   const ValueArray< Vector1 >& points,
                                   const Vector1& pos ) const;
            virtual bool contains( double coordinates[],
                                   size_t base,
                                   const ValueArray< Vector2 >& points,
                                   const Vector2& pos ) const;
            virtual bool contains( double coordinates[],
                                   size_t base,
                                   const ValueArray< Vector3 >& points,
                                   const Vector3& pos ) const;
            virtual void
            interpolate( size_t indices[], double coefficients[], size_t base, const double coordinates[] ) const;

            /**
             * convert the index of the cell to a "base" value, which identifies the cell and can be the
             * index in the vertex index array or a reference vertex, if the vertex ids are computed in
             * structured cell complexes
             */
            virtual size_t makeBase( size_t index ) const = 0;

            /**
             * convert a unique base index as obtained by makeBase to a unique cell id as it it is used by
             * cell( size_t index ) in the grid.
             */
            virtual size_t makeIndex( size_t base ) const = 0;
        };


        // ================================================================================
        // Strategies for structured cell complexes
        // ================================================================================


        // extern const Cell::Stats pointStats;

        extern const size_t hexahedronFaces[6 * 5];

        /// A D-Dimensional tensor product cell with D-linear interpolation.
        /**
         * A HyperCell is a synonym for a D-Dimensional cell with D-Linear
         * interpolation. It is used for automatic mapping of structured
         * grids with line, quad, hexahedron, etc. cells to the basic and
         * named cell types.
         * Thus, a HyperCell<1>::type is a LinearLine etc.
         */
        template < size_t >
        struct HyperCell;
        template <>
        struct HyperCell< 1 >
        {
            typedef LinearLine type;
        };
        template <>
        struct HyperCell< 2 >
        {
            typedef LinearQuad type;
        };
        template <>
        struct HyperCell< 3 >
        {
            typedef LinearHexahedron type;
        };


        /// A d-dimensional strategy for hyper cells.
        /**
         * CellStrategy defines and implements a strategy for HyperCell<D> types, which means that it takes care of
         * building the proper interface to cell indices and the cell's faces for this special case of cell complexes
         */
        // D in {0,1,2,3}, where 0 is implemented separately below and aborts the recursion of cell faces
        template < size_t D >
        struct CellStrategy : Cell::Strategy
        {
            CellStrategy()
                : Cell::Strategy( HyperCell< D >::type::stats )
            {
            }
            void init( const size_t strides[], const size_t face[] )
            {
                for( size_t i = 0; i != 2 * D; ++i )
                    mFaceStrategies[i].init( strides, face + i );
            }
            virtual size_t index( size_t base, size_t i ) const
            {
                return mFaceStrategies[i].index( base, 0 );
            }
            virtual const Strategy* face( size_t i ) const
            {
                return &mFaceStrategies[i];
            }
            CellStrategy< D - 1 > mFaceStrategies[2 * D];
        };

        template <>
        struct CellStrategy< 0 > : Cell::Strategy
        {
            CellStrategy()
                : Cell::Strategy( PointCell::stats )
            {
            }
            void init( const size_t strides[], const size_t face[] )
            {
                mBase = strides[face[0]];
            }
            virtual size_t index( size_t base, size_t ) const
            {
                return mBase + base;
            }
            virtual const Strategy* face( size_t ) const
            {
                throw fantom::logic_error( "Error: getting face of 0-simplex." );
                return 0;
            }
            size_t mBase;
        };

        /**
         * Primary Hyper Cell Strategy is a specialized Strategy, which is used for structured grids, where access to
         * cells can be simplified
         * and the grids consist only of one cell type.
         * HyperCell<D> is the basic cell type used.
         *
         * implemented for D in [1...3]. For dimensions 4 and more, hexahedronFaces has to be
         * defined for higher dimensions.
         */
        template < size_t D >
        struct PrimaryHyperCellStrategy : Strategy
        {
            static_assert( 1 <= D && D <= 3,
                           "PrimaryHyperCellStrategy has not been defined for dimensions other than 1, 2, 3." );

            typedef Tensor< double, D > Point;

            /**
             * Constructs a Strategy for Structured grids consisting only of tensor product cells in the respective
             *dimension.
             *
             * The exact cell types are determined by HyperCell<D>::type
             *
             * \param extent the number of points in each direction of the grid
             */
            PrimaryHyperCellStrategy( const size_t extent[D] )
                : Strategy( HyperCell< D >::type::stats )
            {
                std::copy( extent, extent + D, mExtent );

                mStrides[0] = 0;
                size_t size = 1;
                for( size_t d = 0; d != D; ++d )
                {
                    for( size_t i = 0; i != 1u << d; ++i )
                        mStrides[( 2 << d ) - 1 - i] = size + mStrides[i];

                    size *= extent[d];
                }

                for( size_t i = 0; i != 2 * D; ++i )
                    mFaceStrategies[i].init( &mStrides[0], hexahedronFaces + ( D == 3 ? 5 : 1 ) * i );
            }

            size_t dimension() const override
            {
                return D;
            }

            virtual size_t index( size_t base, size_t i ) const override
            {
                return base + mStrides[i];
            }

            virtual const Cell::Strategy* face( size_t i ) const override
            {
                return &mFaceStrategies[i];
            }

            using Strategy::contains;

            virtual bool contains( double coordinates[],
                                   size_t base,
                                   const ValueArray< Point >& points,
                                   const Point& p ) const override
            {
                size_t indices[1 << D];
                for( size_t i = 0; i != 1 << D; ++i )
                    indices[i] = base + mStrides[i];

                return HyperCell< D >::type::contains( coordinates, p, points, indices );
            }

            virtual void
            interpolate( size_t indices[], double coefficients[], size_t base, const double coords[] ) const override
            {
                for( size_t i = 0; i != 1 << D; ++i )
                    indices[i] = base + mStrides[i];

                HyperCell< D >::type::interpolate( coefficients, coords );
            }

            /**
             * For this hypercell strategy, the base is the vertex with the lowest index in
             * the grid.
             * \param index the index of the cell
             * \returns the lowest index in the grid
             */
            virtual size_t makeBase( size_t index ) const override
            {
                size_t base = 0;
                size_t size = 1;
                for( size_t d = 0; d != D; ++d )
                {
                    base += size * ( index % ( mExtent[d] - 1 ) );
                    index /= mExtent[d] - 1;
                    size *= mExtent[d];
                }

                return base;
            }

            /**
             * The inverse mapping of makeBase
             * \sa makeBase
             */
            virtual size_t makeIndex( size_t base ) const override
            {
                size_t index = 0;
                size_t size = 1;
                for( size_t d = 0; d != D; ++d )
                {
                    index += size * ( base % mExtent[d] );
                    base /= mExtent[d];
                    size *= mExtent[d] - 1;
                }

                return index;
            }

            size_t mExtent[D];
            size_t mStrides[1 << D];
            CellStrategy< D - 1 > mFaceStrategies[2 * D];
        };


        // ================================================================================
        // Cell strategies for unstructured cell complexes
        // ================================================================================


        struct PointStrategy : Strategy
        {
            PointStrategy();
            virtual size_t index( size_t base, size_t ) const override;
            virtual const Cell::Strategy* face( size_t ) const override;

            virtual size_t dimension() const override;
            virtual size_t makeBase( size_t index ) const override;
            virtual size_t makeIndex( size_t base ) const override;

            const ValueArray< size_t >* mIndices;
            size_t mBase;
        };

        struct LinearLineStrategy : Cell::Strategy
        {
            LinearLineStrategy();
            virtual size_t index( size_t base, size_t i ) const override;
            virtual const Strategy* face( size_t i ) const override;

            PointStrategy mPointStrategies[2];
        };

        struct QuadraticLineStrategy : Cell::Strategy
        {
            QuadraticLineStrategy();
            virtual size_t index( size_t base, size_t i ) const override;
            virtual const Strategy* face( size_t i ) const override;

            PointStrategy mPointStrategies[3];
        };

        struct SubparametricQuadraticLineStrategy : Cell::Strategy
        { // FIXME( mario ): don't know whether we need two or three PointStrategies here: we have 3 points but 2 values
            SubparametricQuadraticLineStrategy();
            virtual size_t index( size_t base, size_t i ) const override;
            virtual const Strategy* face( size_t i ) const override;

            PointStrategy mPointStrategies[3];
        };

        template < class CellType >
        struct LinearFaceStrategy : Cell::Strategy
        {
            LinearFaceStrategy( const ValueArray< size_t >& indices, const size_t face[] )
                : Cell::Strategy( CellType::stats )
            {
                for( size_t i = 0; i != CellType::numFaces; ++i )
                {
                    mLineStrategies[i].mPointStrategies[0].mIndices = &indices;
                    mLineStrategies[i].mPointStrategies[0].mBase = face[i];
                    mLineStrategies[i].mPointStrategies[1].mIndices = &indices;
                    mLineStrategies[i].mPointStrategies[1].mBase = face[( i + 1 ) % CellType::numPoints];
                }
            }
            virtual size_t index( size_t base, size_t i ) const override
            {
                return mLineStrategies[i].index( base, 0 );
            }
            virtual const Strategy* face( size_t i ) const override
            {
                return &mLineStrategies[i];
            }
            LinearLineStrategy mLineStrategies[CellType::numFaces];
        };

        template < class CellType >
        struct QuadraticFaceStrategy : Cell::Strategy
        {
            QuadraticFaceStrategy( const ValueArray< size_t >& indices, const size_t face[] )
                : Cell::Strategy( CellType::stats )
            {
                for( size_t i = 0; i != CellType::numFaces; ++i )
                {
                    mLineStrategies[i].mPointStrategies[0].mIndices = &indices;
                    mLineStrategies[i].mPointStrategies[0].mBase = face[i];
                    mLineStrategies[i].mPointStrategies[1].mIndices = &indices;
                    mLineStrategies[i].mPointStrategies[1].mBase = face[( i + 1 ) % ( CellType::numPoints / 2 )];
                    mLineStrategies[i].mPointStrategies[2].mIndices = &indices;
                    mLineStrategies[i].mPointStrategies[2].mBase = face[i + CellType::numPoints / 2];
                }
            }
            virtual size_t index( size_t base, size_t i ) const override
            {
                return mLineStrategies[i].index( base, 0 );
            }
            virtual const Strategy* face( size_t i ) const override
            {
                return &mLineStrategies[i];
            }
            QuadraticLineStrategy mLineStrategies[CellType::numFaces];
        };

        template < class CellType >
        struct PrimaryCellStrategy : Strategy
        {
            enum
            {
                D = CellType::dimension
            };
            typedef Tensor< double, D > Point;

            PrimaryCellStrategy( const ValueArray< size_t >& indices )
                : Strategy( CellType::stats )
                , mIndices( indices )
            {
                if( typeid( CellType ) == typeid( LinearLine ) || typeid( CellType ) == typeid( QuadraticLine ) )
                {
                    for( size_t i = 0; i != CellType::numFaces; ++i )
                    {
                        std::unique_ptr< PointStrategy > p( new PointStrategy() );
                        p->mIndices = &mIndices;
                        p->mBase = i;
                        mFaceStrategies[i] = move( p );
                    }
                }
                else if( typeid( CellType ) == typeid( LinearTriangle ) || typeid( CellType ) == typeid( LinearQuad ) )
                {
                    for( size_t i = 0; i != CellType::numFaces; ++i )
                    {
                        std::unique_ptr< LinearLineStrategy > p( new LinearLineStrategy() );
                        p->mPointStrategies[0].mIndices = &mIndices;
                        p->mPointStrategies[0].mBase = i;
                        p->mPointStrategies[1].mIndices = &mIndices;
                        p->mPointStrategies[1].mBase = ( ( i + 1 ) % CellType::numPoints );
                        mFaceStrategies[i] = move( p );
                    }
                }
                else if( typeid( CellType ) == typeid( LinearTetrahedron ) )
                {
                    for( size_t i = 0; i != CellType::numFaces; ++i )
                        mFaceStrategies[i].reset(
                            new LinearFaceStrategy< LinearTriangle >( mIndices, LinearTetrahedron::faces[i] ) );
                }
                else if( typeid( CellType ) == typeid( LinearPyramid ) )
                {
                    mFaceStrategies[0].reset(
                        new LinearFaceStrategy< LinearQuad >( mIndices, LinearPyramid::faces[0] ) );
                    mFaceStrategies[1].reset(
                        new LinearFaceStrategy< LinearTriangle >( mIndices, LinearPyramid::faces[1] ) );
                    mFaceStrategies[2].reset(
                        new LinearFaceStrategy< LinearTriangle >( mIndices, LinearPyramid::faces[2] ) );
                    mFaceStrategies[3].reset(
                        new LinearFaceStrategy< LinearTriangle >( mIndices, LinearPyramid::faces[3] ) );
                    mFaceStrategies[4].reset(
                        new LinearFaceStrategy< LinearTriangle >( mIndices, LinearPyramid::faces[4] ) );
                }
                else if( typeid( CellType ) == typeid( LinearPrism ) )
                {
                    mFaceStrategies[0].reset(
                        new LinearFaceStrategy< LinearTriangle >( mIndices, LinearPrism::faces[0] ) );
                    mFaceStrategies[1].reset(
                        new LinearFaceStrategy< LinearTriangle >( mIndices, LinearPrism::faces[1] ) );
                    mFaceStrategies[2].reset( new LinearFaceStrategy< LinearQuad >( mIndices, LinearPrism::faces[2] ) );
                    mFaceStrategies[3].reset( new LinearFaceStrategy< LinearQuad >( mIndices, LinearPrism::faces[3] ) );
                    mFaceStrategies[4].reset( new LinearFaceStrategy< LinearQuad >( mIndices, LinearPrism::faces[4] ) );
                }
                else if( typeid( CellType ) == typeid( LinearHexahedron ) )
                {
                    for( size_t i = 0; i != CellType::numFaces; ++i )
                        mFaceStrategies[i].reset(
                            new LinearFaceStrategy< LinearQuad >( mIndices, LinearHexahedron::faces[i] ) );
                }
                // else if (typeid(CellType) == typeid(QuadraticHexahedron))
                // {
                //   for (size_t i = 0; i != CellType::numFaces; ++i)
                //     mFaceStrategies[i].reset(new QuadraticFaceStrategy<QuadraticQuad>(mIndices,
                //     QuadraticHexahedron::faces[i]));
                // }
                else if( typeid( CellType ) == typeid( QuadraticTetrahedron ) )
                {
                    for( size_t i = 0; i != CellType::numFaces; ++i )
                        mFaceStrategies[i].reset( new QuadraticFaceStrategy< QuadraticTriangle >(
                            mIndices, QuadraticTetrahedron::faces[i] ) );
                }
                else if( typeid( CellType ) == typeid( QuadraticTriangle )
                         || typeid( CellType ) == typeid( QuadraticQuad ) )
                {
                    std::cout << "here: " << typeid( CellType ).name() << typeid( CellType ).name() << " "
                              << CellType::numFaces << std::endl;
                    for( size_t i = 0; i != CellType::numFaces; ++i )
                    {
                        std::unique_ptr< QuadraticLineStrategy > p( new QuadraticLineStrategy() );
                        p->mPointStrategies[0].mIndices = &mIndices;
                        p->mPointStrategies[0].mBase = i;
                        p->mPointStrategies[2].mIndices = &mIndices;
                        p->mPointStrategies[2].mBase = ( i + CellType::numPoints / 2 );
                        p->mPointStrategies[1].mIndices = &mIndices;
                        p->mPointStrategies[1].mBase = ( i + 1 ) % ( CellType::numPoints / 2 );
                        mFaceStrategies[i] = move( p );
                    }
                }
                else if( typeid( CellType ) == typeid( SubparametricQuadraticTriangle )
                         || typeid( CellType ) == typeid( SubparametricQuadraticQuad ) )
                {
                    std::cout << "here: " << typeid( CellType ).name() << typeid( CellType ).name() << " "
                              << CellType::numFaces << std::endl;
                    for( size_t i = 0; i != CellType::numFaces; ++i )
                    {
                        std::unique_ptr< SubparametricQuadraticLineStrategy > p(
                            new SubparametricQuadraticLineStrategy() );
                        p->mPointStrategies[0].mIndices = &mIndices;
                        p->mPointStrategies[0].mBase = i;
                        p->mPointStrategies[2].mIndices = &mIndices;
                        p->mPointStrategies[2].mBase = ( i + CellType::numValues / 2 );
                        p->mPointStrategies[1].mIndices = &mIndices;
                        p->mPointStrategies[1].mBase = ( i + 1 ) % ( CellType::numValues / 2 );
                        mFaceStrategies[i] = move( p );
                    }
                }
                // FIXME: implement remaining quadratic cell types
                // TODO: find a way to add implementations of other cell types without the need to modify this code
                else
                {
                    assert( false );
                }
            }

            size_t index( size_t base, size_t i ) const override
            {
                return mIndices[base + i];
            }
            const Cell::Strategy* face( size_t i ) const override
            {
                return mFaceStrategies[i].get();
            }

            /// Implementation of the contains function in case the cell type supports the contains test in this
            /// dimension.
            template < size_t Dim >
            typename std::enable_if< hasContains< CellType, Dim >::value, bool >::type
            contains_impl( double coords[],
                           size_t base,
                           const ValueArray< Tensor< double, Dim > >& points,
                           const Tensor< double, Dim >& p ) const
            {
                size_t indices[CellType::numPoints];
                for( size_t i = 0; i != CellType::numPoints; ++i )
                    indices[i] = mIndices[base + i];

                return CellType::contains( coords, p, points, indices );
            }
            /// Implementation of the contains function in case the cell type does not support the contains test in this
            /// dimension. Defaults to the implementation of Strategy.
            template < size_t Dim >
            typename std::enable_if< !hasContains< CellType, Dim >::value, bool >::type
            contains_impl( double coords[],
                           size_t base,
                           const ValueArray< Tensor< double, Dim > >& points,
                           const Tensor< double, Dim >& p ) const
            {
                return Strategy::contains( coords, base, points, p );
            }

            /// Override of Strategy::dimension.
            size_t dimension() const override
            {
                return D;
            }

            /// Override of Strategy::contains for 1D grids. Delegates to the correct implementation function.
            bool
            contains( double coords[], size_t base, const ValueArray< Point1 >& points, const Point1& p ) const override
            {
                return contains_impl( coords, base, points, p );
            }
            /// Override of Strategy::contains for 2D grids. Delegates to the correct implementation function.
            bool
            contains( double coords[], size_t base, const ValueArray< Point2 >& points, const Point2& p ) const override
            {
                return contains_impl( coords, base, points, p );
            }
            /// Override of Strategy::contains for 3D grids. Delegates to the correct implementation function.
            bool
            contains( double coords[], size_t base, const ValueArray< Point3 >& points, const Point3& p ) const override
            {
                return contains_impl( coords, base, points, p );
            }

            /// Implementation of the interpolate function in case the cell type supports interpolation in this
            /// dimension.
            template < size_t Dim >
            typename std::enable_if< hasInterpolate< CellType, Dim >::value, void >::type
            interpolate_impl( size_t indices[], double coefficients[], size_t base, const double coords[] ) const
            {
                for( size_t i = 0; i != CellType::numPoints; ++i )
                    indices[i] = mIndices[base + i];

                CellType::interpolate( coefficients, coords );
            }
            /// Implementation of the interpolate function in case the cell type does not support interpolation in this
            /// dimension. Defaults to the implementation of Strategy.
            template < size_t Dim >
            typename std::enable_if< !hasInterpolate< CellType, Dim >::value, void >::type
            interpolate_impl( size_t indices[], double coefficients[], size_t base, const double coords[] ) const
            {
                Strategy::interpolate( indices, coefficients, base, coords );
            }

            /// Override of Strategy::interpolate for 3D grids. Delegates to the correct implementation function.
            void
            interpolate( size_t indices[], double coefficients[], size_t base, const double coords[] ) const override
            {
                interpolate_impl< CellType::dimension >( indices, coefficients, base, coords );
            }

            virtual size_t makeBase( size_t index ) const override
            {
                return index * mStats.mNumPoints;
            }

            virtual size_t makeIndex( size_t base ) const override
            {
                return base / mStats.mNumPoints;
            }

            const ValueArray< size_t >& mIndices;
            std::unique_ptr< Cell::Strategy > mFaceStrategies[CellType::numFaces];
        };
    }
}
