#pragma once

#include "CellComplex.hpp"
#include "PointSet.hpp"

namespace fantom
{
    namespace detail
    {
        const std::vector< const DomainPart* >& embeddedCellComplexParts();

        extern const DomainPart Cells;
    }

    /// Cell complex that is embedded into Euclidean space.
    template < size_t D >
    class EmbeddedCellComplex : public PointSet< D >, public CellComplex
    {
        typedef RTTI_Info< EmbeddedCellComplex< D >, PointSet< D > > TypeInfo;
        TYPE_INFORMATION( "Cell complex in " + types::DimensionName< D >() + " space" )

    public:
        /// Part that represents the cells of this cell complex.
        static const DomainPart& Cells;

        EmbeddedCellComplex( const std::shared_ptr< const Implementation >& complex,
                             const ValueArray< Point< D > >& points )
            : PointSet< D >( points )
            , CellComplex( complex )
        {
        }

        const std::vector< const DomainPart* >& domainParts() const override
        {
            return detail::embeddedCellComplexParts();
        }

        size_t domainPartSize( const DomainPart& part ) const override
        {
            if( part == EmbeddedCellComplex< D >::Cells )
                return this->numCells();
            else if( part == PointSetBase::Points )
                return this->numPoints();
            else
                throw std::runtime_error( "Unknown domain part " + part.getName() );
        }

        /**
         * @copydoc DataObject::getInfoStrings()
         */
        std::vector< std::pair< std::string, std::string > > getInfoStrings() const override
        {
            std::vector< std::pair< std::string, std::string > > infoStrings = PointSet< D >::getInfoStrings();
            infoStrings.emplace_back( "cell complex", topologyTypeNames[(unsigned char)topologyType()] );
            for( size_t i = 0; i != numCellTypes(); ++i )
            {
                infoStrings.emplace_back( cellNames[(size_t)cellType( i )],
                                          fantom::lexical_cast< std::string >( cellCount( i ) ) );
            }
            return infoStrings;
        }
    };

    template < size_t D >
    const DomainPart& EmbeddedCellComplex< D >::Cells = detail::Cells;

    /// Simple typelist for matching all cell complex types in options. \ingroup fields
    using All_CellComplex = TypeList< EmbeddedCellComplex< 1 >, EmbeddedCellComplex< 2 >, EmbeddedCellComplex< 3 > >;
    /// Simple typelist for matching all 2D and 3D cell complex types in options. \ingroup fields
    using CellComplex2_or_3 = TypeList< EmbeddedCellComplex< 2 >, EmbeddedCellComplex< 3 > >;


    /// Acceptor function for detecting surface structures. \ingroup fields
    bool isSurface( const DataObject& obj );

    /// Acceptor function that tests whether a cell complex is simplicial \ingroup fields
    bool isSimplicial( const DataObject& obj );

    /// Acceptor function for detecting linesetss. \ingroup fields
    bool isLineset( const DataObject& obj );


    // --------------------------------------------------------------------------------


    extern template class EmbeddedCellComplex< 1 >;
    extern template class EmbeddedCellComplex< 2 >;
    extern template class EmbeddedCellComplex< 3 >;
}
