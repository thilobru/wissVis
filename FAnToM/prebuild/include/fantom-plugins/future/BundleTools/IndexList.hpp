#pragma once

#include <fantom/datastructures/Function.hpp>
#include <fantom/datastructures/domains/List.hpp>

#include <fstream>
#include <vector>

using namespace fantom;

namespace fantom
{
    /// Data structure containing a list of integers
    /**
     * Algorithms could use this class to communicate indices.
     *
     * One usage is the selection of items from a bundle when
     * removing a set of items that are not of interest.
     *
     * It is used for example to extract maxima and minima that
     * could be cancelled from the contour tree and use it as
     * input for the cancellation algorithm.
     */
    class IndexList : public IntegerFunction
    {

    public:
        /// returns whether the index list is sorted
        bool isSorted() const;

        /// returns the index at position idx
        DataType getIndex( size_t idx ) const;

        /// returns the size of the index list, i.e., the number of indices that are stored
        size_t getSize() const;

        /**
         * Create an index list from a std::vector
         */
        // use std::move to make it fast!
        IndexList( std::vector< DataType > rhs );

        /**
         * Create an index list from a std::vector
         * @param isSorted: indicates whether the data is sorted
         */
        // use std::move to make it fast!
        IndexList( std::vector< DataType > rhs, bool isSorted );


        DataType getMaximum() const;
        DataType getMinimum() const;

        /**
         * returns true if the list contains the given index, false otherwise.
         * Performance shall be O( log n ) if isSorted(). On unsorted lists,  average performance of O(n/2)
         * and worst-case performance of O(n)
         */
        bool containsIndex( DataType idx ) const;

        /**
         * write the index list to a file in ASCII format. The header format is:
         * INDEX_LIST numberOfIndices sortedFlag\n
         * followed by a space-separated list of indices.
         * sortedFlag is either "sorted" or "unsorted" (without quotation marks).
         */
        void writeToFile( std::ostream& os ) const;

        static std::unique_ptr< IndexList > readFromFile( std::istream& is );

    private:
        bool mSorted;                   ///< indicator whether the list is sorted or not
        IndexList( std::vector< DataType >& rhs, size_t size );
        IndexList( std::vector< DataType >& rhs, size_t size, bool isSorted );
    };
} // namespace fantom
