#pragma once

#include "ValueArray.hpp"
#include "domains/Grid.hpp"
#include "domains/LineSet.hpp"

#include "../cells.hpp"
#include "../exceptions.hpp"
#include "../math.hpp"
#include "../outputs.hpp"
#include "../utils.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fantom
{
    //======================================= DomainFactory ==========================================

    /// Factory for different types of domains, mostly with spacial embedding. \ingroup commit
    /**
     * \throw fantom::logic_error on invalid inputs ( only in debug build )
     * \note All members are thread-safe.
     */
    class DomainFactory
    {

    public:
        /// Helper method that checks if points of a ValueArray contain INFINITE as a coordinate value.
        /**
         * \tparam D the geometric dimension of the points
         * \param points the ValueArray to check
         */
        template < size_t D >
        static void checkPointsForInfinite( const ValueArray< Point< D > >& points )
        {
            for( size_t i = 0; i != points.size(); i++ )
            {
                std::string typeOfPoint = typeid( points[i][0] ).name();

                if( typeOfPoint == "float" )
                {
                    for( size_t j = 0; j < D; j++ )
                    {
                        if( points[i][j] == std::numeric_limits< float >::infinity()
                            || points[i][j] == -std::numeric_limits< float >::infinity() )
                        {
                            throw fantom::logic_error(
                                "The domain contains points with INFINITE as coordinate values." );
                        }
                    }
                }
                else
                {
                    for( size_t j = 0; j < D; j++ )
                    {
                        if( points[i][j] == std::numeric_limits< double >::infinity()
                            || points[i][j] == -std::numeric_limits< double >::infinity() )
                        {
                            throw fantom::logic_error(
                                "The domain contains points with INFINITE as coordinate values." );
                        }
                    }
                }
            }
        }

        /// Helper method that checks if the sum of the indices needed for the cells of each type equal the amount of
        /// given indices.
        /**
         * \param cellCounts array of pairs containing the cell types and their number
         * \param numCellTypes the numberof
         * \param numGivenIndices the number of given indices
         */
        static void checkCellCountsAndIndices( const std::pair< Cell::Type, size_t > cellCounts[],
                                               size_t numCellTypes,
                                               size_t numGivenIndices )
        {
            size_t numberOfIndices = 0;
            for( size_t i = 0; i != numCellTypes; i++ )
            {
                size_t verticesPerCell = 0;

                if( cellCounts[i].first == Cell::Type::POINT )
                {
                    verticesPerCell = 1;
                }
                if( cellCounts[i].first == Cell::Type::LINE )
                {
                    verticesPerCell = 2;
                }
                if( cellCounts[i].first == Cell::Type::TRIANGLE )
                {
                    verticesPerCell = 3;
                }
                if( cellCounts[i].first == Cell::Type::QUAD )
                {
                    verticesPerCell = 4;
                }
                if( cellCounts[i].first == Cell::Type::TETRAHEDRON )
                {
                    verticesPerCell = 4;
                }
                if( cellCounts[i].first == Cell::Type::PYRAMID )
                {
                    verticesPerCell = 5;
                }
                if( cellCounts[i].first == Cell::Type::PRISM )
                {
                    verticesPerCell = 6;
                }
                if( cellCounts[i].first == Cell::Type::HEXAHEDRON )
                {
                    verticesPerCell = 8;
                }
                if( cellCounts[i].first == Cell::Type::SUBPARAMETRIC_QUADRATIC_LINE )
                {
                    verticesPerCell = 3;
                }
                if( cellCounts[i].first == Cell::Type::SUBPARAMETRIC_QUADRATIC_TRIANGLE )
                {
                    verticesPerCell = 6;
                }
                if( cellCounts[i].first == Cell::Type::SUBPARAMETRIC_QUADRATIC_QUAD )
                {
                    verticesPerCell = 8;
                }
                if( cellCounts[i].first == Cell::Type::SUBPARAMETRIC_QUADRATIC_TETRAHEDRON )
                {
                    verticesPerCell = 10;
                }
                if( cellCounts[i].first == Cell::Type::SUBPARAMETRIC_QUADRATIC_HEXAHEDRON )
                {
                    verticesPerCell = 20;
                }
                if( cellCounts[i].first == Cell::Type::QUADRATIC_LINE )
                {
                    verticesPerCell = 3;
                }
                if( cellCounts[i].first == Cell::Type::QUADRATIC_TRIANGLE )
                {
                    verticesPerCell = 6;
                }
                if( cellCounts[i].first == Cell::Type::QUADRATIC_QUAD )
                {
                    verticesPerCell = 8;
                }
                if( cellCounts[i].first == Cell::Type::QUADRATIC_TETRAHEDRON )
                {
                    verticesPerCell = 10;
                }
                if( cellCounts[i].first == Cell::Type::QUADRATIC_HEXAHEDRON )
                {
                    verticesPerCell = 20;
                }
                numberOfIndices += verticesPerCell * cellCounts[i].second;
            }

            if( numberOfIndices != numGivenIndices )
            {
                throw fantom::logic_error(
                    "Number of indices provided does not match the number of indices needed to create the cells." );
            }
        }


        /// Creates a point set domain from an arbitrary point set.
        /**
         * \tparam D the geometric dimension of the domain, either 1, 2, or 3.
         * \param points the points of the domain.
         */
        template < size_t D >
        static std::shared_ptr< const PointSet< D > > makePointSet( const ValueArray< Point< D > >& points )
        {
            static_assert( 1 <= D && D <= 3, "D must be either 1, 2 or 3" );
#ifndef NDEBUG
            checkPointsForInfinite( points );
#endif

            return std::make_shared< PointSet< D > >( points );
        }

        /// Creates a point set domain on an arbitrary point set.
        /**
         * \tparam D the geometric dimension of the domain, either 1, 2 or 3.
         * \param points the points of the domain. ( use std::move to prevent copying. Otherwise a copy is made. )
         */
        template < size_t D >
        static std::shared_ptr< const PointSet< D > > makePointSet( std::vector< Point< D > > points )
        {
            auto values = makeValueArray( move( points ), Precision::FLOAT64 );
            return makePointSet( *values );
        }

        template < size_t D >
        static std::shared_ptr< const LineSet< D > > makeLineSet( const ValueArray< Point< D > >& points,
                                                                  std::vector< std::vector< size_t > > indices )
        {
            static_assert( 1 <= D && D <= 3, "D must be either 1, 2 or 3" );
#ifndef NDEBUG
            checkPointsForInfinite( points );
#endif

            return std::make_shared< LineSet< D > >( points, move( indices ) );
        }

        template < size_t D >
        static std::shared_ptr< const LineSet< D > > makeLineSet( std::vector< Point< D > > points,
                                                                  std::vector< std::vector< size_t > > indices )
        {
            auto values = makeValueArray( move( points ), Precision::FLOAT64 );
            return makeLineSet( *values, move( indices ) );
        }

        /// Creates a new unstructured grid.
        /**
         * \tparam D the dimension of the domain, either 2 or 3.
         * \param points the point set of this dataset.
         * \param numCellTypes the number of cell types.
         * \param cellCounts a list of cell type/count pairs.
         * \param indices cell indices list.
         * \pre no cell count may be empty. For empty grids set numCellTypes = 0.
         * \pre all indices must be lower than the point set's size.
         *
         * indices contains the cell indices for one cell after the other.
         * The cells are grouped by their cell type in the same order as the cell types appear in cellCounts.
         * With respect to the domain's points' positions, all cells must be convex and oriented counter-clockwise.
         * Indices within each cell are specified according to the documentation in fantom::Cell.
         */
        template < size_t D >
        static std::shared_ptr< const Grid< D > > makeGrid( const ValueArray< Point< D > >& points,
                                                            size_t numCellTypes,
                                                            const std::pair< Cell::Type, size_t > cellCounts[],
                                                            std::unique_ptr< ValueArray< size_t > > indices )
        {
            if( !indices )
            {
                throw fantom::logic_error( "Indices array must be specified." );
            }
#ifndef NDEBUG
            for( size_t i = 0; i != indices->size(); ++i )
            {
                if( ( *indices )[i] >= points.size() )
                {
                    throw fantom::logic_error( "Cell index out of bounds." );
                }
            }

            for( size_t i = 0; i != numCellTypes; ++i )
            {
                if( cellCounts[i].second == 0 )
                {
                    throw fantom::logic_error( "Invalid cell count." );
                }
            }

            checkPointsForInfinite( points );
            checkCellCountsAndIndices( cellCounts, numCellTypes, indices->size() );
#endif

            return std::make_shared< detail::GridUnstructured< D > >(
                points, std::make_shared< CellComplexUnstructured >( numCellTypes, cellCounts, move( indices ) ) );
        } // namespace fantom

        /// Creates a new unstructured grid.
        template < size_t D >
        static std::shared_ptr< const Grid< D > > makeGrid( std::vector< Point< D > > points,
                                                            size_t numCellTypes,
                                                            const std::pair< Cell::Type, size_t > cellCounts[],
                                                            std::vector< size_t > indices )
        {
            auto p = makeValueArray( move( points ), Precision::FLOAT64 );
            return makeGrid( *p,
                             numCellTypes,
                             cellCounts,
                             std::make_unique< DefaultValueArray< size_t > >( move( indices ), Precision::UINT64 ) );
        }

        /// Creates a grid by reusing a cell complex.
        template < size_t D >
        static std::shared_ptr< const Grid< D > > makeGrid( const ValueArray< Point< D > >& points,
                                                            const CellComplex& cells )
        {
#ifndef NDEBUG
            for( size_t c = 0; c != cells.numCells(); ++c )
            {
                auto cell = cells.cell( c );
                for( size_t i = 0; i < cell.numVertices(); ++i )
                {
                    if( cell.index( i ) >= points.size() )
                    {
                        throw fantom::logic_error( "Cell index out of bounds." );
                    }
                }
            }

            checkPointsForInfinite( points );
#endif

            return std::make_shared< detail::GridUnstructured< D > >( points, cells.mComplex );
        }


        /// Creates a point set domain with structured topology on an arbitrary point set.
        /**
         * \tparam TD the topological dimension of the domain, either 1, 2, or 3.
         * \tparam GD the geometric dimension of the domain, either 2 or 3.
         * \param extent the extent of the domain in each topological dimension.
         * \param points the points of the domain.
         * \pre \a TD <= \a GD
         * \pre The positions in \a points are ordered by topological dimensions.
         *      The first dimension is incremented first, then the second, then the third and so on.
         */
        template < size_t TD, size_t GD >
        static std::shared_ptr< const PointSet< GD > >
        makeCurvilinearPointSet( const size_t ( &extent )[TD], const ValueArray< Point< GD > >& points )
        {
#ifndef NDEBUG
            checkPointsForInfinite( points );
#endif

            static_assert( 1 <= TD && TD <= 3, "TD must be either 1, 2 or 3" );
            static_assert( 2 <= GD && GD <= 3, "GD must be either 2 or 3" );
            static_assert( TD <= GD, "TD must be less than or equal to GD" );

            auto curvilinear = std::make_shared< PointSetCurvilinear< GD > >( points, TD, extent );

            return std::make_shared< PointSet< GD > >( *curvilinear );
        }

        /// Creates a point set domain with structured topology on an arbitrary points set.
        /**
         * \tparam TD the topological dimension of the domain, either 1, 2, or 3.
         * \tparam GD the geometric dimension of the domain, either 2 or 3.
         * \param extent the extent of the domain in each topological dimension.
         * \param points the points of the domain. ( use std::move to prevent copying. Otherwise a copy is made. )
         * \pre \a TD <= \a GD
         */
        template < size_t TD, size_t GD >
        static std::shared_ptr< const PointSet< GD > > makeCurvilinearPointSet( const size_t ( &extent )[TD],
                                                                                std::vector< Point< GD > > points )
        {
            auto values = makeValueArray( move( points ), Precision::FLOAT64 );
            return makeCurvilinearPointSet( extent, *values );
        }

        /// Creates a grid with structured topology on an arbitrary point set.
        /**
         * \copydoc makeCurvilinearPointSet( const size_t (&)[TD], const ValueArray< Point< GD > >& )
         */
        template < size_t TD, size_t GD >
        static std::shared_ptr< const Grid< GD > > makeCurvilinearGrid( const size_t ( &extent )[TD],
                                                                        const ValueArray< Point< GD > >& points )
        {
#ifndef NDEBUG
            checkPointsForInfinite( points );
#endif
            static_assert( 1 <= TD && TD <= 3, "TD must be either 1, 2 or 3" );
            static_assert( 2 <= GD && GD <= 3, "GD must be either 2 or 3" );
            static_assert( TD <= GD, "TD must be less than or equal to GD" );

            auto curvilinear = std::make_shared< PointSetCurvilinear< GD > >( points, TD, extent );

            return std::make_shared< detail::GridUnstructured< GD > >(
                *curvilinear, std::make_shared< CellComplexStructured< TD > >( extent ) );
        }

        /// Creates a grid with structured topology on an arbitrary points set.
        /**
         * \copydoc makeCurvilinearPointSet( const size_t (&)[TD], std::vector< Point< GD > > )
         */
        template < size_t TD, size_t GD >
        static std::shared_ptr< const Grid< GD > > makeCurvilinearGrid( const size_t ( &extent )[TD],
                                                                        std::vector< Point< GD > > points )
        {
            auto values = makeValueArray( move( points ), Precision::FLOAT64 );
            return makeCurvilinearGrid( extent, *values );
        }


        /// Creates a rectilinear point set domain.
        /**
         * \tparam D the dimension of the domain, either 2 or 3.
         * \param points the points of the domain.
         * \pre \a points is strictly monotonic increasing
         *         and has at least two entries for each dimension.
         */
        template < size_t D >
        static std::shared_ptr< const PointSet< D > > makeRectilinearPointSet( std::vector< double > ( &points )[D] )
        {
            static_assert( 2 <= D && D <= 3, "D must be either 2 or 3" );
#ifndef NDEBUG
            for( size_t d = 0; d != D; ++d )
            {
                if( points[d].size() < 2 )
                {
                    throw fantom::logic_error( "There must be at least two points in each dimension." );
                }

                if( points[d][0] == std::numeric_limits< double >::infinity()
                    || points[d][0] == -std::numeric_limits< double >::infinity() )
                {
                    throw fantom::logic_error( "Points contain coordinates with INFINITE as their value." );
                }

                for( size_t i = 1; i < points[d].size(); ++i )
                {
                    if( points[d][i - 1] >= points[d][i] )
                    {
                        throw fantom::logic_error( "Points not strictly monotonically increasing." );
                    }

                    if( points[d][i] == std::numeric_limits< double >::infinity()
                        || points[d][i] == -std::numeric_limits< double >::infinity() )
                    {
                        throw fantom::logic_error( "Points contain coordinates with INFINITE as their value." );
                    }
                }
            }

#endif

            auto pointSet = std::make_shared< PointSetRectilinear< D > >( points );
            return std::make_shared< PointSet< D > >( *pointSet );
        }

        /// Creates a rectilinear grid.
        /**
         * \copydoc makeRectilinearPointSet
         */
        template < size_t D >
        static std::shared_ptr< const Grid< D > > makeRectilinearGrid( std::vector< double > ( &points )[D] )
        {
            static_assert( 2 <= D && D <= 3, "D must be either 2 or 3" );
#ifndef NDEBUG
            for( size_t d = 0; d != D; ++d )
            {
                if( points[d].size() < 2 )
                {
                    throw fantom::logic_error( "There must be at least two points in each dimension." );
                }
                if( points[d][0] == std::numeric_limits< double >::infinity()
                    || points[d][0] == -std::numeric_limits< double >::infinity() )
                {
                    throw fantom::logic_error( "Points contain coordinates with INFINITE as their value." );
                }
                for( size_t i = 1; i < points[d].size(); ++i )
                {
                    if( points[d][i - 1] >= points[d][i] )
                    {
                        throw fantom::logic_error( "Points not strictly monotonically increasing." );
                    }
                    if( points[d][i] == std::numeric_limits< double >::infinity()
                        || points[d][i] == -std::numeric_limits< double >::infinity() )
                    {
                        throw fantom::logic_error( "Points contain coordinates with INFINITE as their value." );
                    }
                }
            }
#endif

            auto pointSet = std::make_shared< PointSetRectilinear< D > >( points );
            return std::make_shared< detail::GridStructured< PointSetRectilinear, D > >( *pointSet );
        }

        /// Creates a uniform rectilinear point set domain.
        /**
         * \tparam D the dimension of the domain, either 1, 2, 3, or 4.
         * \param extent the extent of the domain in each dimension.
         * \param offset the offset of the domain in each dimension.
         * \param spacing the spacing of the domain in each dimension.
         * \pre \a extent is at least 2 for each dimension.
         * \pre \a spacing is positive for each dimension.
         */
        template < size_t D >
        static std::shared_ptr< const PointSet< D > >
        makeUniformPointSet( const size_t ( &extent )[D], const double ( &offset )[D], const double ( &spacing )[D] )
        {
            static_assert( 1 <= D && D <= 4, "D must be either 1, 2, 3, or 4" );
#ifndef NDEBUG
            for( size_t d = 0; d != D; ++d )
            {
                if( 0.0 >= spacing[d] )
                {
                    throw fantom::logic_error( "Spacing must be positive !" );
                }
                if( spacing[d] == std::numeric_limits< double >::infinity()
                    || spacing[d] == -std::numeric_limits< double >::infinity() )
                {
                    throw fantom::logic_error( "Spacing conatins INFINITE values !" );
                }
                if( offset[d] == std::numeric_limits< double >::infinity()
                    || offset[d] == -std::numeric_limits< double >::infinity() )
                {
                    throw fantom::logic_error( "Offset conatins INFINITE values !" );
                }
            }
#endif

            auto pointSet = std::make_shared< PointSetUniform< D > >( extent, offset, spacing );
            return std::make_shared< PointSet< D > >( *pointSet );
        }

        /// Creates a uniform rectilinear grid.
        /**
         * \copydoc makeUniformPointSet
         */
        template < size_t D >
        static std::shared_ptr< const Grid< D > >
        makeUniformGrid( const size_t ( &extent )[D], const double ( &offset )[D], const double ( &spacing )[D] )
        {
            static_assert( 1 <= D && D <= 4, "D must be either 1, 2, 3, or 4" );
#ifndef NDEBUG
            for( size_t d = 0; d != D; ++d )
            {
                if( 0.0 >= spacing[d] )
                {
                    throw fantom::logic_error( "Spacing must be positive !" );
                }
                if( spacing[d] == std::numeric_limits< double >::infinity()
                    || spacing[d] == -std::numeric_limits< double >::infinity() )
                {
                    throw fantom::logic_error( "Spacing conatins INFINITE values !" );
                }
                if( offset[d] == std::numeric_limits< double >::infinity()
                    || offset[d] == -std::numeric_limits< double >::infinity() )
                {
                    throw fantom::logic_error( "Offset conatins INFINITE values !" );
                }
            }
#endif

            auto pointSet = std::make_shared< PointSetUniform< D > >( extent, offset, spacing );
            return std::make_shared< detail::GridStructured< PointSetUniform, D > >( *pointSet );
        }


        /// Creates a subset of a PointSet from a list of indices.
        /**
         * The resulting PointSet as well as its point set will always be "unstructured".
         *
         * \tparam D the dimension of the domain, either 2 or 3.
         * \param parent the original grid.
         * \param indices index list for the sub point set.
         * \pre No entry in \a indices is larger
         *      than the number of points in the parent domain.
         */
        template < size_t D >
        static std::shared_ptr< const PointSet< D > >
        makeSubPointSet( const std::shared_ptr< const PointSet< D > >& parent, std::vector< size_t > indices )
        {
            if( !parent )
            {
                throw fantom::logic_error( "Parent point set must be specified." );
            }
#ifndef NDEBUG
            for( size_t i = 0; i != indices.size(); ++i )
            {
                if( indices[i] >= parent->numPoints() )
                {
                    throw fantom::logic_error( "Subset index out of bounds." );
                }
            }
#endif

            auto lookup = makeLookup( indices );

            auto points = std::make_shared< SubValueArray< Point< D > > >( parent->points(), lookup );

            return std::make_shared< detail::SubPointSet< D > >( parent, *points, lookup );
        }


        /// Creates a subset of a grid from a list of cells.
        /**
         * The resulting grid as well as its point set will always be "unstructured".
         *
         * \tparam D the dimension of the domain, either 2 or 3.
         * \param parent the original grid.
         * \param numCellTypes the number of cell types in the sub grid.
         * \param cellCounts a list of cell type/count pairs.
         * \param indices cell indices list for the sub grid.
         * \pre no cell count may be empty. For empty grids set numCellTypes = 0.
         * \pre No entry in \a indices is larger
         *      than the number of points in the parent domain.
         *
         * With respect to the parent's points' positions,
         * all cells must be convex and oriented counter-clockwise.
         * Cell indices are according to documentation in fantom::Cell.
         */
        template < size_t D >
        static std::shared_ptr< const Grid< D > > makeSubGrid( const std::shared_ptr< const PointSet< D > >& parent,
                                                               size_t numCellTypes,
                                                               const std::pair< Cell::Type, size_t > cellCounts[],
                                                               std::vector< size_t > indices )
        {
            if( !parent )
            {
                throw fantom::logic_error( "Parent grid must be specified." );
            }
#ifndef NDEBUG
            for( size_t i = 0; i != indices.size(); ++i )
            {
                if( indices[i] >= parent->numPoints() )
                {
                    throw fantom::logic_error( "Subset index out of bounds." );
                }
            }
            for( size_t i = 0; i != numCellTypes; ++i )
            {
                if( cellCounts[i].second == 0 )
                {
                    throw fantom::logic_error( "Invalid cell count." );
                }
            }

            checkCellCountsAndIndices( cellCounts, numCellTypes, indices.size() );
#endif

            auto lookup = makeLookup( indices );

            auto cellComplex = std::make_shared< CellComplexUnstructured >(
                numCellTypes,
                cellCounts,
                std::make_unique< DefaultValueArray< size_t > >( move( indices ), Precision::UINT64 ) );
            auto points = std::make_shared< SubValueArray< Point< D > > >( parent->points(), lookup );

            return std::make_shared< detail::Subgrid< D > >( parent, *points, cellComplex, lookup );
        }

        template < size_t D >
        static std::shared_ptr< const Grid< D > >
        makeSubGrid( const std::shared_ptr< const EmbeddedCellComplex< D > >& parent,
                     size_t numCellTypes,
                     const std::pair< Cell::Type, size_t > cellCounts[],
                     std::vector< size_t > indices )
        {
            return makeSubGrid(
                std::static_pointer_cast< const PointSet< D > >( parent ), numCellTypes, cellCounts, move( indices ) );
        }

        template < size_t D >
        static std::shared_ptr< const Grid< D > > makeSubGrid( const std::shared_ptr< const Grid< D > >& parent,
                                                               size_t numCellTypes,
                                                               const std::pair< Cell::Type, size_t > cellCounts[],
                                                               std::vector< size_t > indices )
        {
            return makeSubGrid(
                std::static_pointer_cast< const PointSet< D > >( parent ), numCellTypes, cellCounts, move( indices ) );
        }
    };

} // namespace fantom
