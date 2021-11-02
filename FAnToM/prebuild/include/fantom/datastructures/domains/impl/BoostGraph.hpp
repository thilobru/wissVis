#pragma once
#include "../Graph.hpp"

#include <unordered_map>
#include <vector>

namespace fantom
{
    namespace graph_detail
    {
        template < typename EdgeType >
        struct EdgeIdMapper
        {
            size_t getId( const EdgeType& edge ) const
            {
                return mEdgeIds.at( edge );
            }

            const EdgeType& getEdge( size_t id ) const
            {
                return mEdges[id];
            }

            template < typename EdgeIter >
            EdgeIdMapper( size_t numEdges, std::pair< EdgeIter, EdgeIter > edges )
            {
                mEdges.resize( numEdges, EdgeType{} );
                mEdgeIds.reserve( numEdges );
                size_t id = 0;
                for( ; edges.first != edges.second; ++edges.first )
                {
                    mEdges[id] = *edges.first;
                    mEdgeIds[*edges.first] = id;
                    ++id;
                }
            }

        private:
            std::vector< EdgeType > mEdges;
            std::unordered_map< EdgeType, size_t, boost::hash< EdgeType > > mEdgeIds;
        };

        template < typename VertexType >
        struct VertexIdMapper
        {
            size_t getId( const VertexType& vertex ) const
            {
                return mVertexIds.at( vertex );
            }

            const VertexType& getVertex( size_t id ) const
            {
                return mVertices[id];
            }

            template < typename VertexIter >
            VertexIdMapper( size_t numVertices, std::pair< VertexIter, VertexIter > vertices )
            {
                mVertices.resize( numVertices, VertexType{} );
                mVertexIds.reserve( numVertices );
                size_t id = 0;
                for( ; vertices.first != vertices.second; ++vertices.first )
                {
                    mVertices[id] = *vertices.first;
                    mVertexIds[*vertices.first] = id;
                    ++id;
                }
            }

        private:
            std::vector< VertexType > mVertices;
            std::unordered_map< VertexType, size_t, boost::hash< VertexType > > mVertexIds;
        };

        template <>
        struct VertexIdMapper< size_t >
        {
            size_t getId( size_t vertex ) const
            {
                return vertex;
            }

            size_t getVertex( size_t id ) const
            {
                return id;
            }

            template < typename VertexIter >
            VertexIdMapper( size_t, std::pair< VertexIter, VertexIter > )
            {
            }
        };

        template < typename GraphType >
        class BoostGraph : public fantom::Graph
        {
            GraphType mGraph;
            EdgeIdMapper< typename boost::graph_traits< GraphType >::edge_descriptor > mEdgeMapper;
            VertexIdMapper< typename boost::graph_traits< GraphType >::vertex_descriptor > mVertexMapper;

            template < typename EdgeType, typename EdgeIterator >
            struct BoostEdgeIterImpl : Graph::EdgeIterImpl
            {
            private:
                const EdgeIdMapper< EdgeType >& mMapper;
                EdgeIterator mCurrent;
                EdgeIterator mEnd;

            public:
                BoostEdgeIterImpl( const EdgeIdMapper< EdgeType >& mapper,
                                   std::pair< EdgeIterator, EdgeIterator > edgeIterators )
                    : mMapper( mapper )
                    , mCurrent( edgeIterators.first )
                    , mEnd( edgeIterators.second )
                {
                }

                void next() override
                {
                    if( mCurrent != mEnd )
                        ++mCurrent;
                }

                Edge get() const override
                {
                    return Edge{ mMapper.getId( *mCurrent ) };
                }

                bool isValid() const override
                {
                    return mCurrent != mEnd;
                }
            };

            template < typename EdgeType, typename EdgeIterator >
            std::unique_ptr< Graph::EdgeIterImpl >
            makeBoostEdgeIter( const EdgeIdMapper< EdgeType >& mapper,
                               std::pair< EdgeIterator, EdgeIterator > iters ) const
            {
                return std::make_unique< BoostEdgeIterImpl< EdgeType, EdgeIterator > >( mapper, iters );
            }

        public:
            BoostGraph( GraphType&& graph )
                : mGraph( std::forward< GraphType >( graph ) )
                , mEdgeMapper( boost::num_edges( mGraph ), boost::edges( mGraph ) )
                , mVertexMapper( boost::num_vertices( mGraph ), boost::vertices( mGraph ) )
            {
            }

            size_t numVertices() const override
            {
                using boost::num_vertices;

                return num_vertices( mGraph );
            }

            size_t numEdges() const override
            {
                using boost::num_edges;

                return num_edges( mGraph );
            }

            EdgeIterator edges() const override
            {
                using boost::edges;

                return EdgeIterator( makeBoostEdgeIter( mEdgeMapper, edges( mGraph ) ) );
            }

            EdgeIterator outEdges( Vertex vertex ) const override
            {
                using boost::out_edges;

                return EdgeIterator(
                    makeBoostEdgeIter( mEdgeMapper, out_edges( mVertexMapper.getVertex( vertex ), mGraph ) ) );
            }

            EdgeIterator inEdges( Vertex vertex ) const override
            {
                using boost::in_edges;

                return EdgeIterator(
                    makeBoostEdgeIter( mEdgeMapper, in_edges( mVertexMapper.getVertex( vertex ), mGraph ) ) );
            }

            Vertex source( Edge edge ) const override
            {
                using boost::source;

                return Vertex{ mVertexMapper.getId( source( mEdgeMapper.getEdge( edge ), mGraph ) ) };
            }

            Vertex target( Edge edge ) const override
            {
                using boost::target;

                return Vertex{ mVertexMapper.getId( target( mEdgeMapper.getEdge( edge ), mGraph ) ) };
            }

            size_t outDegree( Vertex vertex ) const override
            {
                using boost::out_degree;

                return out_degree( mVertexMapper.getVertex( vertex ), mGraph );
            }

            size_t inDegree( Vertex vertex ) const override
            {
                using boost::in_degree;

                return in_degree( mVertexMapper.getVertex( vertex ), mGraph );
            }

            size_t degree( Vertex vertex ) const override
            {
                using boost::degree;

                return degree( mVertexMapper.getVertex( vertex ), mGraph );
            }
        };
    }

    template < typename GraphType >
    std::shared_ptr< const Graph > adaptBoostGraph( GraphType&& graph )
    {
        return std::make_shared< graph_detail::BoostGraph< GraphType > >( std::forward< GraphType >( graph ) );
    }
}
