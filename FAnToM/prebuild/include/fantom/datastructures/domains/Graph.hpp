#pragma once

#include "../../utils.hpp"
#include "../Domain.hpp"
#include <iterator>

namespace fantom
{
    /// Graph domain.
    /**
     * A graph consists of a vertex set and an edge set.
     * Edges are directed and connect one vertex to another.
     * Data can be registered on both, the vertex set and the edge set.
     */
    class Graph : public Domain
    {
        typedef RTTI_Info< Graph, Domain > TypeInfo;
        TYPE_INFORMATION( "Graph" )

        struct VertexTag
        {
        };
        struct EdgeTag
        {
        };

    public:
        using Vertex = detail::Index< VertexTag >;
        using Edge = detail::Index< EdgeTag >;

        static const DomainPart Vertices;
        static const DomainPart Edges;

        struct EdgeIterImpl
        {
            virtual ~EdgeIterImpl();
            virtual Edge get() const = 0;
            virtual void next() = 0;
            virtual bool isValid() const = 0;
        };

        struct EdgeIterator
        {
            using difference_type = std::ptrdiff_t;
            using value_type = Edge;
            using pointer = Edge*;
            using reference = Edge; // no actual reference. Must be a copy!
            using iterator_category = std::input_iterator_tag;

            EdgeIterator&& begin();   // copy of self or move of self
            EdgeIterator end() const; // empty mPimpl!

            Edge operator*() const;
            EdgeIterator& operator++(); // pre-increment

            bool operator==( const EdgeIterator& rhs ) const; // if rhs.mPimpl == nullptr, return self.mPimpl.hasNext().
                                                              // otherwise compare mPimpls
            bool operator!=( const EdgeIterator& rhs ) const;

        public:
            EdgeIterator( std::unique_ptr< EdgeIterImpl > pimpl )
                : mPimpl( move( pimpl ) )
            {
            }

            EdgeIterator( EdgeIterator&& rhs )
                : mPimpl( move( rhs.mPimpl ) )
            {
            }

            EdgeIterator& operator=( EdgeIterator&& rhs )
            {
                mPimpl = move( rhs.mPimpl );
                return *this;
            }

            ~EdgeIterator();

        private:
            std::unique_ptr< EdgeIterImpl > mPimpl;
        };

        virtual size_t numVertices() const = 0;
        virtual size_t numEdges() const = 0;

        virtual EdgeIterator edges() const = 0;

        virtual EdgeIterator outEdges( Vertex vertex ) const = 0;
        virtual EdgeIterator inEdges( Vertex vertex ) const = 0;

        virtual Vertex source( Edge edge ) const = 0;
        virtual Vertex target( Edge edge ) const = 0;

        virtual size_t outDegree( Vertex vertex ) const = 0;
        virtual size_t inDegree( Vertex vertex ) const = 0;
        virtual size_t degree( Vertex vertex ) const = 0;

        const std::vector< const DomainPart* >& domainParts() const override;
        size_t domainPartSize( const DomainPart& part ) const override;
    };

    // adaptor for boost interface --------------------------------------------------------------------------------

    namespace detail
    {
        struct NumberIt
        {
            operator size_t&()
            {
                return mN;
            }

            operator size_t() const
            {
                return mN;
            }

            size_t operator*() const
            {
                return mN;
            }

            size_t mN;
        };
    }

    inline size_t num_edges( const Graph& g )
    {
        return g.numEdges();
    }

    inline size_t num_vertices( const Graph& g )
    {
        return g.numVertices();
    }

    inline std::pair< detail::NumberIt, detail::NumberIt > vertices( const Graph& g )
    {
        return std::make_pair( detail::NumberIt{ 0 }, detail::NumberIt{ g.numVertices() } );
    }

    inline std::pair< detail::NumberIt, detail::NumberIt > edges( const Graph& g )
    {
        return std::make_pair( detail::NumberIt{ 0 }, detail::NumberIt{ g.numEdges() } );
    }

    inline size_t in_degree( size_t v, const Graph& g )
    {
        return g.inDegree( Graph::Vertex{ v } );
    }

    inline size_t out_degree( size_t v, const Graph& g )
    {
        return g.outDegree( Graph::Vertex{ v } );
    }

    inline size_t degree( size_t v, const Graph& g )
    {
        return g.degree( Graph::Vertex{ v } );
    }

    inline std::pair< Graph::EdgeIterator, Graph::EdgeIterator > in_edges( size_t v, const Graph& g )
    {
        auto begin = g.inEdges( Graph::Vertex{ v } );
        auto end = begin.end();
        return std::make_pair( std::move( begin ), std::move( end ) );
    }

    inline std::pair< Graph::EdgeIterator, Graph::EdgeIterator > out_edges( size_t v, const Graph& g )
    {
        auto begin = g.outEdges( Graph::Vertex{ v } );
        auto end = begin.end();
        return std::make_pair( std::move( begin ), std::move( end ) );
    }

    inline size_t source( size_t e, const Graph& g )
    {
        return g.source( Graph::Edge{ e } );
    }

    inline size_t target( size_t e, const Graph& g )
    {
        return g.target( Graph::Edge{ e } );
    }
}
