#pragma once

#include "../ValueArray.hpp"
#include "impl/CellDefinitions.hpp"
#include "impl/CellStrategies.hpp"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

namespace fantom
{
    // =============================== CellComplex ============================================


    /// Cell complex.
    /**
     * A cell complex is a collection of abstract cells.
     * Each cell is defined by means of its vertex indices.
     * The cells in the complex are adjacent if they share common indices.
     */
    class CellComplex
    {
    public:
        class Implementation;

        CellComplex( const std::shared_ptr< const Implementation >& complex );
        ~CellComplex();

        /// Returns the topological structuring type of the cell complex.
        TopologyType topologyType() const;

        /// Determines whether the complex only contains simplex cells
        bool isSimplicial() const;

        /// Determines the maximal topological dimension of cells present in this complex.
        /// This corresponds to the dimension of the manifold represented by the complex.
        size_t getMaximalCellDimension() const;

        /// Determines the number of cell kinds.
        size_t numCellTypes() const;

        /// Determines the number of the i-th kind of cells.
        /**
         * \param i cell type index.
         * \pre 0 <= \a i < \c numCellTypes().
         */
        size_t cellCount( size_t i ) const;

        /// Determines the type of the i-th kind of cells.
        /**
         * \param i cell type index.
         * \pre 0 <= \a i < \c numCellTypes().
         */
        Cell::Type cellType( size_t i ) const;

        /// Determines the number of cells.
        size_t numCells() const;

        /// Determines a reference to the i-th cell.
        /**
         * \param i cell index.
         * \pre 0 <= \a i < \c numCells().
         */
        Cell cell( size_t i ) const;

        /// Determines the index of a given cell.
        /**
         * \param c the cell.
         * \pre there must be one i for which c == operator[](i).
         */
        size_t index( const Cell& c ) const;

        /// Gives a ValueArray to access the cells
        const ValueArray< Cell >& cells() const;

        /// \internal Return reference to the internal implementation.
        const Implementation& getImpl() const;

    protected:
        std::shared_ptr< const Implementation > mComplex; // cell complex implementation. Protected for direct access in
                                                          // Grid implementation.

    private:
        std::shared_ptr< const ValueArray< Cell > > mCells;
        TopologyType mTopologyType;

        friend class DomainFactory;
    };


    //===================================== CellComplex Implementation =================================


    /**
     * The implementation of an abstract cell complex.
     * Its interface is defined in this class.
     *
     * The internal behavior and major parts of the logic for the cell access is implemented by a Strategy for each cell
     * type present in the cell complex.
     */
    class CellComplex::Implementation
    {
    public:
        virtual ~Implementation();

        std::vector< std::pair< Cell::Type, size_t > > mCellCounts; //< an array containing cell types and how
                                                                    // many cells of that type exist

        virtual size_t numCells() const; //< convenience function for accessing the number of cells

        virtual Cell operator[]( size_t cellIndex ) const = 0; //< factory function to access the cell with index
                                                               // cellIndex

        virtual size_t index( const Cell& cell ) const = 0; //< inverse lookup from a given cell to the index of the
                                                            // cell in the complex

        virtual size_t cellTypeDimension( size_t cellIndex ) const = 0; //< returns the topological dimension of the
                                                                        // cell with index cellIndex
    };


    // --------------------------------------------------------------------------------

    class CellComplexStructuredBase : public CellComplex::Implementation
    {
    };

    template < size_t D >
    class CellComplexStructured : public CellComplexStructuredBase
    {
    public:
        cells::PrimaryHyperCellStrategy< D > mStrategy;
        size_t mExtent[D]; // extent of the points on which the cells exist

        CellComplexStructured( const size_t extent[D] )
            : mStrategy( extent )
        {
            mCellCounts.emplace_back( cells::HyperCell< D >::type::stats.mType, 1 );
            for( size_t d = 0; d != D; ++d )
                mCellCounts[0].second *= extent[d] - 1;

            std::copy( extent, extent + D, mExtent );
        }

        size_t numCells() const override
        {
            return mCellCounts[0].second;
        }

        Cell operator[]( size_t cellIndex ) const override
        {
            return Cell( mStrategy, mStrategy.makeBase( cellIndex ) );
        }

        size_t index( const Cell& cell ) const override
        {
            return mStrategy.makeIndex( cell.mBase );
        }

        size_t cellTypeDimension( size_t /* cellIndex */ ) const override
        {
            return mStrategy.dimension();
        }
    };


    // --------------------------------------------------------------------------------


    class CellComplexUnstructured : public CellComplex::Implementation
    {
    public:
        CellComplexUnstructured( size_t numCellTypes,
                                 const std::pair< Cell::Type, size_t > cellCounts[],
                                 std::unique_ptr< ValueArray< size_t > > indices );

        Cell operator[]( size_t cellIndex ) const override;

        size_t index( const Cell& cell ) const override;

        size_t cellTypeDimension( size_t cellIndex ) const override;

    private:
        const std::unique_ptr< std::unique_ptr< cells::Strategy >[] > mStrategies;
        const std::unique_ptr< size_t[] > mSize;
        const std::unique_ptr< size_t[] > mBase;
        const std::unique_ptr< ValueArray< size_t > > mIndices;
    };


    // --------------------------------------------------------------------------------


    extern template class CellComplexStructured< 2 >;
    extern template class CellComplexStructured< 3 >;
}
