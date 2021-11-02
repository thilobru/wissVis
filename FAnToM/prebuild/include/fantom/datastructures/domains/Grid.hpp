#pragma once

#include "EmbeddedCellComplex.hpp"
#include "Interpolator.hpp"
#include "impl/celltree.hpp"

#include <memory>

namespace fantom
{
    namespace detail
    {
        template < size_t D >
        class InterpolatorPointBased;
        template < size_t D >
        class InterpolatorCellBased;
    }

    /// Representation of grids. \ingroup fields
    /**
     * A grid is defined on a set of control points and cells and allows point location and interpolation.
     */
    template < size_t D >
    class Grid : public EmbeddedCellComplex< D >, public InterpolatableDomain< D >
    {
        typedef RTTI_Info< Grid< D >, EmbeddedCellComplex< D > > TypeInfo;
        TYPE_INFORMATION( types::DimensionName< D >() + " grid" )

    public:
        Grid( const std::shared_ptr< const CellComplex::Implementation >& complex,
              const ValueArray< Point< D > >& points )
            : EmbeddedCellComplex< D >( complex, points )
        {
        }

        /// Determine whether a point is contained in a given cell
        /**
         * \param c cell
         * \param p point
         */
        virtual bool contains( const Cell& c, const Point< D >& p ) const = 0;

        /// Determines the cell under a given point.
        /**
         * \param p the point.
         */
        virtual Cell locate( const Point< D >& p ) const = 0;

        /// Create an Interpolator for this grid
        std::unique_ptr< Interpolator< D > > makeInterpolator( const DomainPart& domainType ) const override;

    private:
        virtual void locate( Cell& cell, double coords[], const Point< D >& p ) const = 0;

        virtual void
        interpolate( size_t indices[], double coefficients[], const Cell& cell, const double coords[] ) const = 0;

        template < size_t >
        friend class detail::InterpolatorPointBased;
        template < size_t >
        friend class detail::InterpolatorCellBased;
    };


    namespace detail
    {
        template < size_t D >
        class InterpolatorPointBased : public Interpolator< D >
        {
        public:
            InterpolatorPointBased( const Grid< D >& grid )
                : mGrid( grid )
            {
            }

            explicit operator bool() const override
            {
                return static_cast< bool >( mCell );
            }

            void implReset( const Point< D >& p ) override
            {
                mGrid.locate( mCell, mCoordinates, p );
            }

            void implValue( Combiner& op ) const override
            {
                mGrid.interpolate( mIndices, mWeights, mCell, mCoordinates );

                op( mCell.numValues(), mIndices, mWeights );
            }

        private:
            const Grid< D >& mGrid;
            Cell mCell;
            double mCoordinates[cells::maxDimensions];
            mutable size_t mIndices[cells::maxNumValuesPerCell];
            mutable double mWeights[cells::maxNumValuesPerCell];
        };

        template < size_t D >
        class InterpolatorCellBased : public Interpolator< D >
        {
        public:
            InterpolatorCellBased( const Grid< D >& grid )
                : mGrid( grid )
                , mWeight( 1.0 )
            {
            }

            explicit operator bool() const override
            {
                return static_cast< bool >( mCell );
            }

            void implReset( const Point< D >& p ) override
            {
                double coords[cells::maxDimensions];
                mGrid.locate( mCell, coords, p );
            }

            void implValue( Combiner& op ) const override
            {
                mIndex = mGrid.index( mCell );
                op( 1, &mIndex, &mWeight );
            }

        private:
            const Grid< D >& mGrid;
            Cell mCell;
            double mWeight;
            mutable size_t mIndex;
        };
    }


    template < size_t D >
    std::unique_ptr< Interpolator< D > > Grid< D >::makeInterpolator( const DomainPart& domainType ) const
    {
        if( domainType == Grid< D >::Points )
        {
            return std::make_unique< detail::InterpolatorPointBased< D > >( *this );
        }
        else if( domainType == Grid< D >::Cells )
        {
            return std::make_unique< detail::InterpolatorCellBased< D > >( *this );
        }
        else
        {
            throw fantom::not_implemented_error( "Not implemented yet!" );
        }
    }


    namespace detail
    {
        //=========================== Cell weights for structured grids ================================

        template < size_t >
        class Dimension
        {
        };

        struct WeightIgnore
        {
            void operator()( double )
            {
            }
        };

        struct WeightStore
        {
            WeightStore( double* weights );

            void operator()( double weight );

            double* mWeights;
        };

        void combine(
            size_t indices[], double coefficients[], const size_t p[], const double w[], size_t i, Dimension< 1 > );

        void combine(
            size_t indices[], double coefficients[], const size_t p[], const double w[], size_t i, Dimension< 2 > );

        void combine(
            size_t indices[], double coefficients[], const size_t p[], const double w[], size_t i, Dimension< 3 > );


        //====================================== Implementation of structured grids =================================


        /**
         * Structured grid means that the point set is structured (uniform or rectilinear).
         * This implies a structured cell complex. Thus, the cell lookup is a simple computation and no acceleration
         * structures are required.
         * \internal
         */
        template < template < size_t D > class Points, size_t D >
        class GridStructured : public Grid< D >
        {
            static_assert( std::is_base_of< PointSetUniform< D >, Points< D > >{}
                               || std::is_base_of< PointSetRectilinear< D >, Points< D > >{},
                           "Structured grids can only be created on top of uniform and rectilinear point sets!" );

        public:
            GridStructured( const Points< D >& points )
                : Grid< D >( std::make_shared< CellComplexStructured< D > >( points.structuringDimensionExtents() ),
                             points )
            {
                // compute strides
                size_t size = 1;
                for( size_t d = 0; d != D; ++d )
                {
                    mStrides[d] = size;
                    size *= points.structuringDimensionExtent( d );
                }
            }

        private:
            bool contains( const Cell& cell, const Point< D >& p ) const override
            {
                auto& cells = static_cast< const CellComplexStructured< D >& >( *this->mComplex );
                if( cell && containedInPointSet( p ) )
                {
                    size_t base = weight( p, WeightIgnore() );
                    return base == cell.mBase && &( cells.mStrategy ) == cell.mStrategy;
                }
                else
                {
                    return false;
                }
            }

            bool containedInPointSet( const Point< D >& p ) const
            {
                if( typeid( Points< D > ) == typeid( PointSetUniform< D > ) )
                {
                    const PointSetUniform< D >& ps = static_cast< const PointSetUniform< D >& >( this->points() );

                    for( size_t d = 0; d != D; ++d )
                        if( p[d] < ps.getOrigin( d )
                            || p[d] > ps.getOrigin( d )
                                          + ( ps.structuringDimensionExtent( d ) - 1 ) * ps.getSpacing( d ) )
                            return false;

                    return true;
                }
                else if( typeid( Points< D > ) == typeid( PointSetRectilinear< D > ) )
                {
                    const PointSetRectilinear< D >& ps
                        = static_cast< const PointSetRectilinear< D >& >( this->points() );

                    for( size_t d = 0; d != D; ++d )
                        if( p[d] < ps.getAxisCoordinates( d )[0]
                            || p[d] > ps.getAxisCoordinates( d )[ps.structuringDimensionExtent( d ) - 1] )
                            return false;

                    return true;
                }
                else
                {
                    return false;
                }
            }

            /// for structured grids based on this: get the local coordinates of \a p
            /**
             * \a op must contain a method <tt>void operator()(double weight)</tt>
             * \a weight is the weight of \a p in one dimension.
             * Starting from the base point index of the interpolation,
             * adding \a stride results in the index of the second point in that direction.
             * \return index of the base point of the interpolation
             */
            template < class Operation >
            size_t weight( const Point< D >& p, Operation op ) const
            {
                if( typeid( Points< D > ) == typeid( PointSetUniform< D > ) )
                {
                    const PointSetUniform< D >& ps = static_cast< const PointSetUniform< D >& >( this->points() );
                    size_t result = 0, size = 1;
                    for( size_t d = 0; d != D; ++d )
                    {
                        size_t coord = std::min( size_t( ( p[d] - ps.getOrigin( d ) ) / ps.getSpacing( d ) ),
                                                 ps.structuringDimensionExtent( d ) - 2 );
                        double weight = ( p[d] - ps.getOrigin( d ) ) / ps.getSpacing( d ) - coord;
                        result += size * coord;
                        op( weight );
                        size *= ps.structuringDimensionExtent( d );
                    }
                    return result;
                }
                else if( typeid( Points< D > ) == typeid( PointSetRectilinear< D > ) )
                {
                    const PointSetRectilinear< D >& ps
                        = static_cast< const PointSetRectilinear< D >& >( this->points() );
                    size_t result = 0, size = 1;
                    for( size_t d = 0; d != D; ++d )
                    {
                        size_t coord
                            = std::lower_bound( &ps.getAxisCoordinates( d )[1],
                                                &ps.getAxisCoordinates( d )[ps.structuringDimensionExtent( d ) - 1],
                                                p[d] )
                              - &ps.getAxisCoordinates( d )[1];
                        double weight = ( p[d] - ps.getAxisCoordinates( d )[coord] )
                                        / ( ps.getAxisCoordinates( d )[coord + 1] - ps.getAxisCoordinates( d )[coord] );
                        result += size * coord;
                        op( weight );
                        size *= ps.structuringDimensionExtent( d );
                    }
                    return result;
                }
                else
                {
                    return 0;
                }
            }

            Cell locate( const Point< D >& p ) const override
            {
                auto& cells = static_cast< const CellComplexStructured< D >& >( *this->mComplex );
                return containedInPointSet( p ) ? Cell( cells.mStrategy, weight( p, WeightIgnore() ) ) : Cell();
            }

            void locate( Cell& cell, double coords[], const Point< D >& p ) const override
            {
                auto& cells = static_cast< const CellComplexStructured< D >& >( *this->mComplex );
                cell = containedInPointSet( p ) ? Cell( cells.mStrategy, weight( p, WeightStore( coords ) ) ) : Cell();
            }

            void interpolate( size_t indices[],
                              double coefficients[],
                              const Cell& cell,
                              const double coords[] ) const override
            {
                combine( indices, coefficients, mStrides, coords, cell.mBase, Dimension< D >() );
            }

            size_t mStrides[D];
        };


        //============================== Implementation of unstructured grids ===================================


        /**
         * Unstructured grid means that the point set is unstructured (either completely unstructured or curvilinear).
         * The cell complex can either be structured or unstructured.
         * But the nature of the point set requires an acceleration structure (in this case the cell tree of Christoph
         * Garth) for a fast lookup of cells.
         * \internal
         */
        template < size_t D >
        class GridUnstructured : public Grid< D >
        {
        public:
            GridUnstructured( const ValueArray< Point< D > >& points,
                              const std::shared_ptr< const CellComplex::Implementation >& cellComplex )
                : Grid< D >( cellComplex, points )
            {
                celltree_builder< D > b;
                b.build( mCellTree, this->numCells(), *this->mComplex, this->points() );
            }

            GridUnstructured( const PointSetCurvilinear< D >& points )
                : Grid< D >( std::make_shared< CellComplexStructured< D > >( points.structuringDimensionExtents() ),
                             points )
            {
                celltree_builder< D > b;
                b.build( mCellTree, this->numCells(), *this->mComplex, this->points() );
            }

        private:
            bool contains( const Cell& c, const Point< D >& p ) const override
            {
                double coords[cells::maxDimensions];
                return static_cast< const cells::Strategy* >( c.mStrategy )
                    ->contains( coords, c.mBase, this->points(), p );
            }

            bool contains( double coordinates[], const Cell& c, const Point< D >& p ) const
            {
                return static_cast< const cells::Strategy* >( c.mStrategy )
                    ->contains( coordinates, c.mBase, this->points(), p );
            }

            Cell locate( const Point< D >& p ) const override
            {
                celltree::point_traversal pt( mCellTree, &p[0] );

                while( const celltree::node* n = pt.next() )
                {
                    const unsigned int* begin = &( mCellTree.leaves[n->start()] );
                    const unsigned int* end = begin + n->size();

                    for( ; begin != end; ++begin )
                    {
                        Cell c = ( *this->mComplex )[*begin];
                        if( contains( c, p ) )
                            return c;
                    }
                }

                return Cell();
            }

            void locate( Cell& cache, double coords[], const Point< D >& p ) const override
            {
                if( !cache || !contains( coords, cache, p ) )
                {
                    celltree::point_traversal pt( mCellTree, &p[0] );

                    while( const celltree::node* n = pt.next() )
                    {
                        const unsigned int* begin = &( mCellTree.leaves[n->start()] );
                        const unsigned int* end = begin + n->size();

                        for( ; begin != end; ++begin )
                        {
                            Cell c = ( *this->mComplex )[*begin];
                            if( contains( coords, c, p ) )
                            {
                                cache = c;
                                return;
                            }
                        }
                    }

                    cache = Cell();
                }
            }

            void interpolate( size_t indices[],
                              double coefficients[],
                              const Cell& cell,
                              const double coords[] ) const override
            {
                return static_cast< const cells::Strategy* >( cell.mStrategy )
                    ->interpolate( indices, coefficients, cell.mBase, coords );
            }

            celltree mCellTree;
        };


        //============================== Implementation of sub grids ===================================


        template < size_t D >
        class Subgrid : public Subdomain, public GridUnstructured< D >
        {
        public:
            Subgrid( const std::shared_ptr< const PointSet< D > >& parent,
                     const ValueArray< Point< D > >& points,
                     const std::shared_ptr< const CellComplex::Implementation >& cellComplex,
                     const std::shared_ptr< const std::vector< size_t > >& lookup )
                : Subdomain( parent, Grid< D >::Points, lookup )
                , GridUnstructured< D >( points, cellComplex )
            {
            }
        };
    }


    /// Simple typelist for matching all grid types in options. \ingroup fields
    using All_Grid = TypeList< Grid< 1 >, Grid< 2 >, Grid< 3 > >;
    /// Simple typelist for matching all 2D and 3D grid types in options. \ingroup fields
    using Grid2_or_3 = TypeList< Grid< 2 >, Grid< 3 > >;


    // --------------------------------------------------------------------------------


    extern template class Grid< 2 >;
    extern template class Grid< 3 >;
    extern template class detail::GridStructured< PointSetRectilinear, 2 >;
    extern template class detail::GridStructured< PointSetUniform, 2 >;
    extern template class detail::GridStructured< PointSetRectilinear, 3 >;
    extern template class detail::GridStructured< PointSetUniform, 3 >;
    extern template class detail::GridUnstructured< 2 >;
    extern template class detail::GridUnstructured< 3 >;
    extern template class detail::Subgrid< 2 >;
    extern template class detail::Subgrid< 3 >;
}
