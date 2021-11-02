#pragma once

#include "../Domain.hpp"

namespace fantom
{
    /// Simple data list.
    /**
     * This domain type can be used as the basis for multivariate datasets.
     */
    class List : public Domain
    {
        typedef RTTI_Info< List, Domain > TypeInfo;
        TYPE_INFORMATION( "List" )

    public:
        static const DomainPart Entries;

        List( size_t size );

        size_t numEntries() const;

        const std::vector< const DomainPart* >& domainParts() const override;
        size_t domainPartSize( const DomainPart& part ) const override;

    private:
        size_t mSize;
    };

    namespace fantom_detail
    {
        class Sublist : public List, public Subdomain
        {
        public:
            Sublist( const std::shared_ptr< const List >& origin,
                     const std::shared_ptr< const std::vector< size_t > >& lookup );
        };
    }

    /// Create a sublist by filtering a list.
    /**
     * \param list the original list
     * \param pred Predicate function
     * \tparam Predicate the type of the predicate function. Must contain a bool operator()(size_t i)
     *         that returns whether the i-th element of the list will be present or not in the sublist.
     */
    template < typename Predicate >
    std::shared_ptr< List > filterList( const std::shared_ptr< const List >& list, Predicate pred )
    {
        if( list == nullptr )
        {
            throw std::runtime_error( "Invalid list used for filtering" );
        }

        auto lookup = std::make_shared< std::vector< size_t > >();

        size_t count = 0;
        for( size_t i = 0; i < list->numEntries(); ++i )
        {
            if( pred( i ) )
            {
                ++count;
            }
        }

        lookup->resize( count );
        size_t current = 0;
        for( size_t i = 0; i < list->numEntries(); ++i )
        {
            if( pred( i ) )
            {
                ( *lookup )[current] = i;
                ++current;
            }
        }

        return std::make_shared< fantom_detail::Sublist >( list, lookup );
    }
}
