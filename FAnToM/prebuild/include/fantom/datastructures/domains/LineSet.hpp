#pragma once

#include "EmbeddedCellComplex.hpp"
#include "PointSet.hpp"
#include "impl/CellStrategies.hpp"

namespace
{
    using namespace fantom;

    class LSValueArray : public ValueArray< size_t >
    {
    public:
        LSValueArray( const std::vector< std::vector< size_t > >& lines )
            : ValueArray( Precision::FLOAT64, false )
            , mLines( lines )
        {
        }

        size_t size() const override
        {
            size_t size = 0;
            for( auto& line : mLines )
            {
                size += std::max( size_t( 2 ), 2 * line.size() )
                        - 2; // every line has N-1 segments and twice the number of points
            }
            return size;
        }

    private:
        size_t get( size_t i ) const override
        {
            size_t base = i / 2; // = index of line segment
            size_t idx = i % 2;  // = index of point in line segment

            auto lineIt = begin( mLines );
            while( base + 1 >= lineIt->size() )
            {
                base -= std::max( size_t( 1 ), lineIt->size() ) - 1; // N-1 segments in every line
                ++lineIt;
            }
            return ( *lineIt )[base + idx];
        }

        const std::vector< std::vector< size_t > >& mLines;
    };

    class CellComplexLineSet : public CellComplex::Implementation
    {
    public:
        CellComplexLineSet()
            : mLineView( mLines )
            , mStrategy( mLineView )
        {
            mCellCounts.resize( 1 );
            mCellCounts.front().first = Cell::LINE;
            mCellCounts.front().second = 0;
        }

        CellComplexLineSet( std::vector< std::vector< size_t > > indices )
            : mLines( std::move( indices ) )
            , mLineView( mLines )
            , mStrategy( mLineView )
        {
            mCellCounts.resize( 1 );
            mCellCounts.front().first = Cell::LINE;
            mCellCounts.front().second = std::accumulate(
                indices.begin(), indices.end(), size_t( 0 ), []( const auto& accVal, const auto& nextVal ) {
                    return accVal + nextVal.size() - 1;
                } );
        }

        Cell operator[]( size_t cellIndex ) const override
        {
            return Cell( mStrategy, 2 * cellIndex );
        }

        size_t index( const Cell& cell ) const override
        {
            return cell.mBase / 2;
        }

        size_t cellTypeDimension( size_t ) const override
        {
            return 1;
        }

        size_t addLine( std::vector< size_t > line )
        {
            if( !line.empty() )
            {
                mCellCounts.front().second += line.size() - 1;
            }
            mLines.push_back( move( line ) );
            return mLines.size() - 1;
        }

        void addPointToLine( size_t pointNr, size_t lineNr )
        {
            auto& line = mLines.at( lineNr );
            if( !line.empty() )
            {
                mCellCounts.front().second++;
            }
            line.push_back( pointNr );
        }

        size_t numLines() const
        {
            return mLines.size();
        }

        const std::vector< std::vector< size_t > >& getLines() const
        {
            return mLines;
        }

        void clear()
        {
            mLines.clear();
            mCellCounts.front().second = 0;
        }

    private:
        std::vector< std::vector< size_t > > mLines;
        LSValueArray mLineView;
        cells::PrimaryCellStrategy< cells::LinearLine > mStrategy;
    };
} // namespace

namespace fantom
{
    /// Collection of lines.
    /**
     * A line set is a collection of several lines, where every line is a list of points.
     */
    template < size_t D >
    class LineSet : public EmbeddedCellComplex< D >
    {
        typedef RTTI_Info< LineSet< D >, EmbeddedCellComplex< D > > TypeInfo;
        TYPE_INFORMATION( types::DimensionName< D >() + " LineSet" )

    public:
        /**
         * Create an empty line set
         */
        LineSet()
            : EmbeddedCellComplex< D >( std::make_shared<::CellComplexLineSet >(),
                                        *std::make_shared< DefaultValueArray< Point< D > > >(
                                            std::vector< Point< D > >(), Precision::FLOAT64 ) )
            , mPoints( static_cast< DefaultValueArray< Point< D > >& >(
                  const_cast< ValueArray< Point< D > >& >( this->points() ) ) )
        {
        }

        LineSet( const ValueArray< Point< D > >& points, std::vector< std::vector< size_t > > indices )
            : EmbeddedCellComplex< D >( std::make_shared<::CellComplexLineSet >( move( indices ) ), points )
            , mPoints( static_cast< DefaultValueArray< Point< D > >& >(
                  const_cast< ValueArray< Point< D > >& >( this->points() ) ) )
        {
        }

        /**
         * Add a point to the line set
         *
         * \returns the index of the point in the data
         */
        size_t addPoint( Point< D > point )
        {
            mPoints.getValues().push_back( point );
            return mPoints.size() - 1;
        }

        /**
         * adds a line described by a set of indices to points
         *
         * \pre points must have been added to the line set already
         */
        size_t addLine( std::vector< size_t > line )
        {
            return static_cast<::CellComplexLineSet& >( const_cast< CellComplex::Implementation& >( *this->mComplex ) )
                .addLine( move( line ) );
        }

        /**
         * appends a point to an existing line
         */
        void addPointToLine( size_t pointNr, size_t lineNr )
        {
            static_cast<::CellComplexLineSet& >( const_cast< CellComplex::Implementation& >( *this->mComplex ) )
                .addPointToLine( pointNr, lineNr );
        }

        /**
         * \returns the number of lines
         */
        size_t numLines() const
        {
            return static_cast< const ::CellComplexLineSet& >( *this->mComplex ).numLines();
        }

        /**
         * \param lineNr the index of the line in the line set
         * \returns the length of line lineNr
         */
        size_t getLineLength( size_t lineNr ) const
        {
            return static_cast< const ::CellComplexLineSet& >( *this->mComplex ).getLines().at( lineNr ).size();
        }

        /**
         * \returns a reference to the line indices
         */
        const std::vector< size_t >& getLine( size_t lineNr ) const
        {
            return static_cast< const ::CellComplexLineSet& >( *this->mComplex ).getLines().at( lineNr );
        }
        /**
         * returns a reference to all lines
         */
        const std::vector< std::vector< size_t > >& getLines() const
        {
            return static_cast< const ::CellComplexLineSet& >( *this->mComplex ).getLines();
        }

        /**
         * \param lineNr the index of the line in the line set
         * \param pointIdx the index/position of a point on that line
         * \returns the spatial location of a point on a line
         * \pre 0 <= pointIdx < getLineLength( lineNr )
         */
        const Point< D >& getPointOnLine( size_t lineNr, size_t pointIdx ) const
        {
            return this->points().access( getLine( lineNr ).at( pointIdx ) );
        }

        /**
         * \param lineNr the index of the line in the line set
         * \param pointIdx the index/position of a point on that line
         * \returns the index of a point on a line that can be used to address points in the points() ValueArray
         */
        size_t getPointIndexOnLine( size_t lineNr, size_t pointIdx ) const
        {
            return getLine( lineNr ).at( pointIdx );
        }

        /**
         * clears all data in the line set
         */
        void clearLineSet()
        {
            static_cast<::CellComplexLineSet& >( const_cast< CellComplex::Implementation& >( *this->mComplex ) )
                .clear();
            mPoints.getValues().clear();
        }

        std::vector< std::pair< std::string, std::string > > getInfoStrings() const override
        {
            std::vector< std::pair< std::string, std::string > > infoStrings
                = EmbeddedCellComplex< D >::getInfoStrings();
            infoStrings.emplace_back( "Number of Lines", lexical_cast< std::string >( numLines() ) );
            return infoStrings;
        }


    private:
        DefaultValueArray< Point< D > >& mPoints;
    };

    /// Simple typelist for matching all LineSet types in options. \ingroup fields
    using All_LineSet = TypeList< LineSet< 1 >, LineSet< 2 >, LineSet< 3 > >;
    /// Simple typelist for matching all 2D and 3D LineSet types in options. \ingroup fields
    using LineSet2_or_3 = TypeList< LineSet< 2 >, LineSet< 3 > >;

    extern template class LineSet< 2 >;
    extern template class LineSet< 3 >;
} // namespace fantom
