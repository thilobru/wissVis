#pragma once

#include "../Domain.hpp"


/*
  index structure
 *  of an example table
 *
 *    |0|1|2|
 *    |3|4|5|
 *    |6|7|8|
 *
*/

namespace fantom
{
    /// Representation of a table.
    /**
     * As a domain, a table only defines a collection of table cells (represented by column and row count).
     * Data can be registered for the cells through the definition of a Function< T >.
     * An index lookup of row/column index pair into a flat index of the data list can be done using the getIndex()
     * method.
     */
    class Table : public Domain
    {
        typedef RTTI_Info< Table, Domain > TypeInfo;
        TYPE_INFORMATION( "Table" )

    public:
        static const DomainPart Entries;

        Table( size_t nRow, size_t nColum );
        ~Table();

        /// Returns the dimensions of the table.
        std::pair< size_t /*rows*/, size_t /* columns */ > getSize() const;

        /// sets the heading names of the table, one for each column
        void setHeadings( const std::vector< std::string >& headings );

        /// returns the heading names of the table, one for each column
        std::vector< std::string > getHeadings() const;

        /// returns the running index for the specified table cell
        size_t getIndex( size_t row, size_t col ) const;

        /// compute the table cell from the running index
        std::pair< size_t, size_t > getPosition( size_t idx ) const;

        /// \copydoc getIndex()
        size_t operator()( size_t row, size_t col ) const
        {
            return getIndex( row, col );
        }

        const std::vector< const DomainPart* >& domainParts() const override;
        size_t domainPartSize( const DomainPart& part ) const override;

    private:
        std::vector< std::string > mHeadings;
        std::pair< size_t /* rows */, size_t /* columns */ > mSize;
    };
}
