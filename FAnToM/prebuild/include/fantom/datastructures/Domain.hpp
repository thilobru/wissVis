#pragma once

#include "DataObject.hpp"
#include "ValueArray.hpp"

#include <memory>

namespace fantom
{
    /// Representation of a part of a domain,
    /// for example the points of a grid, the edges of a graph, or the cells of a table.
    class DomainPart
    {
    public:
        /// Creates an unique domain part, identified by a string name.
        explicit DomainPart( std::string name );

        DomainPart( const DomainPart& ) = delete;
        DomainPart( DomainPart&& ) = delete;
        DomainPart& operator=( const DomainPart& ) = delete;
        DomainPart& operator=( DomainPart&& ) = delete;

        bool operator==( const DomainPart& rhs ) const;
        bool operator!=( const DomainPart& rhs ) const;

        /// Get the name of this domain part.
        const std::string& getName() const;

    private:
        size_t mPartId;
        std::string mName;
    };


    // --------------------------------------------------------------------------------


    /// Discrete domain. \ingroup fields commit
    /**
     * A domain defines sets of elements on which mappings (or \a Function S) can be defined.
     * Each domain consists of one or multiple parts. These are discrete and distinct sets of elements that characterize
     * compartments of the domain.
     * E.g., a domain can be a set of points in the Euclidean space, or a grid consisting of point set and cell complex,
     * or a graph consisting of vertex and edge sets.
     *
     * Data can be registered on top of a domain (with reference to the respective domain part) by means of \a Function.
     * The goal of this design is to have an easy interface to the data that is independent from the specific domain the
     * data is registered on (regarding the C++ type). This makes it easy to write common and general algorithms that
     * only process data without needing to know the domain type.
     * All domain-related data is encoded in the meta-data and is easily copyable.
     * This allows for truly domain-agnostic algorithms without the disadvantage that the domain-related data would be
     * pruned.
     */
    class Domain : public DataObject
    {
        typedef RTTI_Info< Domain, DataObject > TypeInfo;
        TYPE_INFORMATION( "Domain" )

    public:
        static const DomainPart any;

        /// Returns a list of all parts of this domain.
        virtual const std::vector< const DomainPart* >& domainParts() const = 0;

        /// Returns the number of parts in this domain.
        virtual size_t domainPartSize( const DomainPart& part ) const = 0;

        /// Returns a vector containing the info strings.
        std::vector< std::pair< std::string, std::string > > getInfoStrings() const override;
    };

    /// Detect whether a type is a domain
    template < typename T >
    using is_domain = std::is_base_of< Domain, T >;


    /// Representation of a subset of a domain.
    /**
     * A sub domain contains a subset of a domain part of the parent domain.
     * It may differ in all other domain parts, but with respect to the specified domain part
     * a direct mapping of child indices to parent indices is possible.
     */
    class Subdomain
    {
    public:
        Subdomain( const std::shared_ptr< const Domain >& parent,
                   const DomainPart& part,
                   const std::shared_ptr< const std::vector< size_t > >& lookup );

        virtual ~Subdomain();

        const DomainPart& subclassedPart() const;
        const std::shared_ptr< const Domain >& parent() const;
        const std::shared_ptr< const std::vector< size_t > >& getLookup() const;

    private:
        const DomainPart& mPart;
        std::shared_ptr< const Domain > mParent;
        std::shared_ptr< const std::vector< size_t > > mLookup;
    };
}
