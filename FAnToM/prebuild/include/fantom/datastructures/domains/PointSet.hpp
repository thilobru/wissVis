#pragma once

#include "../../math.hpp"
#include "../Domain.hpp"
#include "../ValueArray.hpp"

#include <cassert>

namespace fantom
{
    /// base class of all point sets \ingroup internal
    class PointSetBase : public Domain
    {
        typedef RTTI_Info< PointSetBase, Domain > TypeInfo;
        TYPE_INFORMATION( "Pointset" )

    public:
        class BoundingBox;

        /// Part that represents the cells of this cell complex.
        static const DomainPart Points;

        /// Type of the structure within this point set.
        StructuringType structuringType() const;

        /// Determines the number of structuring dimensions, 0 if unstructured.
        size_t numStructuringDimensions() const;

        /// Determines the extent of the given structuring dimension
        /**
         * \param dimension the extent's index.
         * \pre 0 <= \a i < \c numExtents().
         */
        size_t structuringDimensionExtent( size_t dimension ) const;

        /// Determines the number of control points.
        virtual size_t numPoints() const = 0;

        /// Converts structured coordinates into an index useable in the points() ValueArray
        template < size_t SD >
        size_t indexFromCoordinates( const std::array< size_t, SD >& coordinates ) const
        {
            if( SD != numStructuringDimensions() )
                throw fantom::out_of_range( "Wrong number of coordinates!" );
            size_t result = 0;
            for( size_t i = SD; i > 0; --i )
            {
                result *= mExtents[i - 1];
                result += coordinates[i - 1];
            }
            return result;
        }

        /// Determine the bounding box of the point set
        virtual BoundingBox getBoundingBox() const = 0;

        /**
         * @copydoc DataObject::getInfoStrings()
         */
        std::vector< std::pair< std::string, std::string > > getInfoStrings() const override;

    protected:
        PointSetBase( StructuringType structuring, std::vector< size_t > extents );

    private:
        size_t domainPartSize( const DomainPart& part ) const override;
        const std::vector< const DomainPart* >& domainParts() const override;

        // metadata about the structuring of the points
        StructuringType mStructuringType;
        std::vector< size_t > mExtents;
    };


    /// representation of a discrete set of points
    /**
     * The purpose of this class is to store a point set.
     * It also holds meta information about the structuring of the point set.
     * \tparam D geometric dimension of the domain.
     * \throw fantom::out_of_range when accessing invalid indices (only in debug build)
     * \note All members are thread-safe.
     */
    template < size_t D >
    class PointSet : public PointSetBase
    {
        typedef RTTI_Info< PointSet< D >, PointSetBase > TypeInfo;
        TYPE_INFORMATION( types::DimensionName< D >() + " pointset" )

    public:
        static const size_t dimension = D;

        PointSet( const ValueArray< Point< D > >& points );

        size_t numPoints() const override
        {
            return mPoints->size();
        }

        /// Determines the control points.
        /// The positions in the ValueArray are ordered by structuring dimensions (if any).
        /// The first dimension is incremented first, then the second, then the third and so on.
        const ValueArray< Point< D > >& points() const
        {
            return *mPoints;
        }

        /// \copydoc PointSetBase::getBoundingBox
        BoundingBox getBoundingBox() const override;

    private:
        std::shared_ptr< const ValueArray< Point< D > > > mPoints;
    };


    class PointSetBase::BoundingBox
    {
    public:
        BoundingBox();
        BoundingBox( std::pair< double, double > boundsX );
        BoundingBox( std::pair< double, double > boundsX, std::pair< double, double > boundsY );
        BoundingBox( std::pair< double, double > boundsX,
                     std::pair< double, double > boundsY,
                     std::pair< double, double > boundsZ );
        BoundingBox( std::vector< std::pair< double, double > > bounds );

        /// Returns the bounds for dimension dim.
        std::pair< double, double > operator[]( size_t dim ) const;

        /// Returns the extent in the given dimension.
        double getLength( size_t dim ) const;

        /// Returns the number of dimensions of this bounding box
        size_t getDimensions() const;

        bool operator==( const BoundingBox& rhs ) const;

        bool operator!=( const BoundingBox& rhs ) const;

    private:
        std::vector< std::pair< double, double > > mBounds;
    };


    //========================================= PointSetCurvilinear ==============================================


    /// A curvilinear point set. Indices are mapped to positions according to the documentation in DiscreteDomain.
    /// The positions in the ValueArray are ordered by structuring dimensions.
    /// The first dimension is incremented first, then the second, then the third and so on.
    template < size_t D >
    class PointSetCurvilinear : public ValueArray< Point< D > >
    {
    public:
        PointSetCurvilinear( const ValueArray< Point< D > >& points, size_t numExtents, const size_t extents[] )
            : ValueArray< Point< D > >( Precision::FLOAT64, points.hasRefAccess() )
            , mNumExtents( numExtents )
            , mPoints( points.shared_from_this() )
        {
            fantom_release_assert( mNumExtents <= D,
                                   "Topological dimension must be less than or equal to geometric dimension!" );

            for( size_t d = 0; d != mNumExtents; ++d )
            {
                mExtent[d] = extents[d];
            }

            assert( mPoints->size()
                    == std::accumulate( mExtent, mExtent + mNumExtents, size_t( 1 ), std::multiplies< size_t >() ) );
        }

        size_t size() const override
        {
            return mPoints->size();
        }

        /**
         * \returns the number of structuring dimensions.
         */
        size_t numStructuringDimensions() const
        {
            return mNumExtents;
        }

        /**
         * \returns direct access to the structuringDimensionExtents
         */
        const size_t* structuringDimensionExtents() const
        {
            return mExtent;
        }

        /**
         * \param dimension the dimension to query
         * \return the number of points in the direction dimension
         */
        size_t structuringDimensionExtent( size_t dimension ) const
        {
#ifndef NDEBUG
            if( dimension >= numStructuringDimensions() )
            {
                throw std::logic_error( "Unknown structuring dimension: " + lexical_cast< std::string >( dimension ) );
            }
#endif

            return mExtent[dimension];
        }

    private:
        Point< D > get( size_t i ) const override
        {
            return ( *mPoints )[i];
        }

        const Point< D >& getRef( size_t i ) const override
        {
            return mPoints->access( i );
        }

        Point< D >& getRef( size_t ) override
        {
            // FIXME: somehow, we should also implement write access here...
            // But then, we need a maker function that guarantees a const PointSetCurvilinear when a const ValueArray is
            // used for construction.

            throw fantom::logic_error( "Not implemented yet" );
        }

        size_t mNumExtents;
        size_t mExtent[D];
        std::shared_ptr< const ValueArray< Point< D > > > mPoints;
    };


    //====================================== PointSetRectilinear ============================================


    /// A rectilinear point set. Indices are mapped to positions according to the documentation in DiscreteDomain.
    /// The positions in the ValueArray are ordered by structuring dimensions.
    /// The first dimension is incremented first, then the second, then the third and so on.
    template < size_t D >
    class PointSetRectilinear : public ValueArray< Point< D > >
    {
    public:
        PointSetRectilinear( std::vector< double > ( &points )[D] )
            : ValueArray< Point< D > >( Precision::FLOAT64, false )
        {
            for( size_t d = 0; d != D; ++d )
            {
                mExtent[d] = points[d].size();
                mPoints[d] = move( points[d] );
            }

            mSize = std::accumulate( mExtent, mExtent + D, 1, std::multiplies< size_t >() );
        }

        PointSetBase::BoundingBox getBoundingBox() const
        {
            std::vector< std::pair< double, double > > bounds;

            for( size_t d = 0; d != D; ++d )
            {
                bounds.emplace_back( mPoints[d].front(), mPoints[d].back() );
            }

            return PointSetBase::BoundingBox( move( bounds ) );
        }

        size_t size() const override
        {
            return mSize;
        }

        /**
         * \returns the number of structuring dimensions.
         */
        size_t numStructuringDimensions() const
        {
            return D;
        }

        /**
         * \returns direct access to the structuringDimensionExtents
         */
        const size_t* structuringDimensionExtents() const
        {
            return mExtent;
        }

        /**
         * \param dimension the dimension to query
         * \return the number of points in the direction dimension
         */
        size_t structuringDimensionExtent( size_t dimension ) const
        {
#ifndef NDEBUG
            if( dimension >= numStructuringDimensions() )
            {
                throw std::logic_error( "Unknown structuring dimension: " + lexical_cast< std::string >( dimension ) );
            }
#endif

            return mExtent[dimension];
        }

        /**
         * \param dimension the dimension to query
         * \return the vector of axis coordinates in the direction dimension
         */
        const std::vector< double >& getAxisCoordinates( size_t dimension ) const
        {
#ifndef NDEBUG
            if( dimension >= numStructuringDimensions() )
            {
                throw std::logic_error( "Unknown structuring dimension: " + lexical_cast< std::string >( dimension ) );
            }
#endif

            return mPoints[dimension];
        }

    private:
        Point< D > get( size_t i ) const override
        {
            Point< D > point;

            for( size_t d = 0; d != D; ++d )
            {
                point[d] = mPoints[d][i % mExtent[d]];
                i /= mExtent[d];
            }

            return point;
        }

        size_t mExtent[D];
        size_t mSize;
        std::vector< double > mPoints[D];
    };


    //====================================== PointSetUniform ============================================


    /// A uniform point set. Indices are mapped to positions according to the documentation in DiscreteDomain.
    /// The positions in the ValueArray are ordered by structuring dimensions.
    /// The first dimension is incremented first, then the second, then the third and so on.
    template < size_t D >
    class PointSetUniform : public ValueArray< Point< D > >
    {
    public:
        PointSetUniform( const size_t extent[D], const double origin[D], const double spacing[D] )
            : ValueArray< Point< D > >( Precision::FLOAT64, false )
        {
            for( size_t d = 0; d != D; ++d )
                mExtent[d] = extent[d];

            for( size_t d = 0; d != D; ++d )
                mOrigin[d] = origin[d];

            for( size_t d = 0; d != D; ++d )
                mSpacing[d] = spacing[d];

            mSize = std::accumulate( mExtent, mExtent + D, 1, std::multiplies< size_t >() );
        }

        PointSetBase::BoundingBox getBoundingBox() const
        {
            std::vector< std::pair< double, double > > bounds;

            for( size_t d = 0; d != D; ++d )
            {
                bounds.push_back( std::make_pair( mOrigin[d], mOrigin[d] + mSpacing[d] * ( mExtent[d] - 1 ) ) );
            }

            return PointSetBase::BoundingBox( move( bounds ) );
        }

        size_t size() const override
        {
            return mSize;
        }

        /**
         * \returns the number of structuring dimensions.
         */
        size_t numStructuringDimensions() const
        {
            return D;
        }

        /**
         * \returns direct access to the structuringDimensionExtents
         */
        const size_t* structuringDimensionExtents() const
        {
            return mExtent;
        }

        /**
         * \param dimension the dimension to query
         * \return the number of points in the direction dimension
         */
        size_t structuringDimensionExtent( size_t dimension ) const
        {
#ifndef NDEBUG
            if( dimension >= numStructuringDimensions() )
            {
                throw std::logic_error( "Unknown structuring dimension: " + lexical_cast< std::string >( dimension ) );
            }
#endif

            return mExtent[dimension];
        }

        /**
         * \param dimension the dimension to query
         * \return the spacing of points (i.e. their distance) along the d-th coordinate axis
         */
        double getSpacing( size_t dimension ) const
        {
#ifndef NDEBUG
            if( dimension >= numStructuringDimensions() )
            {
                throw std::logic_error( "Unknown dimension: " + lexical_cast< std::string >( dimension ) );
            }
#endif

            return mSpacing[dimension];
        }

        /**
         * \param dimension the dimension to query
         * \return coordinate of the origin along the d-th coordinate dimension
         * \sa getOrigin()
         * \sa get( size_t )
         */
        double getOrigin( size_t dimension ) const
        {
#ifndef NDEBUG
            if( dimension >= numStructuringDimensions() )
            {
                throw std::logic_error( "Unknown dimension: " + lexical_cast< std::string >( dimension ) );
            }
#endif

            return mOrigin[dimension];
        }

        /**
         * convenience function that returns the origin of the reference coordinate frame
         * This is the same as calling get( (size_t) 0 )
         */
        Point< D > getOrigin() const
        {
            return Point< D >( mOrigin );
        }

    private:
        Point< D > get( size_t i ) const override
        {
            Point< D > point;

            for( size_t d = 0; d != D; ++d )
            {
                point[d] = mOrigin[d] + ( i % mExtent[d] ) * mSpacing[d];
                i /= mExtent[d];
            }

            return point;
        }

        size_t mExtent[D];
        size_t mSize;
        double mOrigin[D];
        double mSpacing[D];
    };


    // ===================================== SubPointSet ============================================


    namespace detail
    {
        template < size_t D >
        class SubPointSet : public Subdomain, public PointSet< D >
        {
        public:
            SubPointSet( const std::shared_ptr< const PointSet< D > >& parent,
                         const ValueArray< Point< D > >& points,
                         const std::shared_ptr< const std::vector< size_t > >& lookup )
                : Subdomain( parent, PointSet< D >::Points, lookup )
                , PointSet< D >( points )
            {
            }
        };
    }


    // ===================================== implementation ============================================

    namespace detail
    {
        template < size_t D >
        StructuringType getStructuringType( const ValueArray< Point< D > >& points )
        {
            if( dynamic_cast< const PointSetUniform< D >* >( &points ) )
            {
                return StructuringType::UNIFORM;
            }
            else if( dynamic_cast< const PointSetRectilinear< D >* >( &points ) )
            {
                return StructuringType::RECTILINEAR;
            }
            else if( dynamic_cast< const PointSetCurvilinear< D >* >( &points ) )
            {
                return StructuringType::CURVILINEAR;
            }
            else
            {
                return StructuringType::UNSTRUCTURED;
            }
        }

        template < size_t D >
        std::vector< size_t > getStructuringDimensionExtents( const ValueArray< Point< D > >& points )
        {
            std::vector< size_t > extents;

            if( auto ps = dynamic_cast< const PointSetUniform< D >* >( &points ) )
            {
                for( size_t i = 0; i < ps->numStructuringDimensions(); ++i )
                {
                    extents.push_back( ps->structuringDimensionExtent( i ) );
                }
            }
            else if( auto ps2 = dynamic_cast< const PointSetRectilinear< D >* >( &points ) )
            {
                for( size_t i = 0; i < ps2->numStructuringDimensions(); ++i )
                {
                    extents.push_back( ps2->structuringDimensionExtent( i ) );
                }
            }
            else if( auto ps3 = dynamic_cast< const PointSetCurvilinear< D >* >( &points ) )
            {
                for( size_t i = 0; i < ps3->numStructuringDimensions(); ++i )
                {
                    extents.push_back( ps3->structuringDimensionExtent( i ) );
                }
            }

            return extents;
        }
    }

    template < size_t D >
    PointSet< D >::PointSet( const ValueArray< Point< D > >& points )
        : PointSetBase( detail::getStructuringType( points ), detail::getStructuringDimensionExtents( points ) )
        , mPoints( points.shared_from_this() )
    {
    }

    template < size_t D >
    PointSetBase::BoundingBox PointSet< D >::getBoundingBox() const
    {
        if( auto ps = dynamic_cast< const PointSetUniform< D >* >( mPoints.get() ) )
        {
            return ps->getBoundingBox();
        }
        else if( auto ps2 = dynamic_cast< const PointSetRectilinear< D >* >( mPoints.get() ) )
        {
            return ps2->getBoundingBox();
        }
        else
        {
            Point< D > pMin, pMax;
            for( size_t d = 0; d != D; ++d )
            {
                pMin[d] = std::numeric_limits< double >::max();
                pMax[d] = std::numeric_limits< double >::lowest();
            }

            for( size_t i = 0; i < mPoints->size(); ++i )
            {
                auto value = ( *mPoints )[i];
                for( size_t d = 0; d != D; ++d )
                {
                    pMin[d] = std::min( pMin[d], value[d] );
                    pMax[d] = std::max( pMax[d], value[d] );
                }
            }
            std::vector< std::pair< double, double > > bounds;
            for( size_t d = 0; d != D; ++d )
            {
                bounds.push_back( std::make_pair( pMin[d], pMax[d] ) );
            }
            return BoundingBox( move( bounds ) );
        }
    }


    /// Simple typelist for matching all PointSet types in options. \ingroup fields
    using All_PointSet = TypeList< PointSet< 1 >, PointSet< 2 >, PointSet< 3 > >;
    /// Simple typelist for matching all 2D and 3D PointSet types in options. \ingroup fields
    using PointSet2_or_3 = TypeList< PointSet< 2 >, PointSet< 3 > >;


    // --------------------------------------------------------------------------------


    extern template class PointSet< 2 >;
    extern template class PointSet< 3 >;
    extern template class PointSetUniform< 2 >;
    extern template class PointSetUniform< 3 >;
    extern template class PointSetRectilinear< 2 >;
    extern template class PointSetRectilinear< 3 >;
    extern template class PointSetCurvilinear< 2 >;
    extern template class PointSetCurvilinear< 3 >;
    extern template class ValueArray< Point< 2 > >;
    extern template class ValueArray< Point< 3 > >;
}
