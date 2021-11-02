#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <vector>

namespace fantom
{
    /**
     * \brief
     * Implements the CellTree search structure to locate cells
     *
     * This is an implementation of the data structure and data construction algorithm
     * for the fast cell location as presented in
     * "Fast, Memory-Efficient Cell Location in Unstructured Grids for Visualization"
     * by Christoph Garth and Kenneth I. Joy presented at VisWeek 2011
     */
    struct celltree
    {
        /**
         * \brief
         * A node in the graph that can either be an internal node or a leaf
         */
        struct node
        {
            unsigned int index;

            // make the structs share the same memory
            // as we use one of these only but want
            // to avoid unnecessary typing issues
            union data_t {
                struct data_float_t
                {
                    float lm;
                    float rm;
                } f;

                struct data_int_t
                {
                    unsigned int sz;
                    unsigned int st;
                } i;

                // sanity check to ensure optimal memory usage
                static_assert( sizeof( data_float_t ) == sizeof( data_int_t ),
                               "Sizes of data types in celltree data structure are not ideal" );
            } data;

            void make_node( unsigned int left, unsigned int d, float b[2] );

            void set_children( unsigned int left );

            unsigned int left() const;

            unsigned int right() const;

            unsigned int dim() const;

            const float& lmax() const;

            const float& rmin() const;

            void make_leaf( unsigned int start, unsigned int size );

            bool is_leaf() const;

            unsigned int start() const;

            unsigned int size() const;
        };

        std::vector< node > nodes;
        std::vector< unsigned int > leaves;

        /**
         * \brief
         * Point traversal class that is used for cell location
         *
         * The class traverses the tree and is used to extract
         * a number of possible matching cells.
         *
         * The usage of the code can be found in FieldsImpl.cpp
         * The following is an example of how to locate cells,
         * where contains checks whether the given point is inside
         * the cell and must be stable for neighboring cells in a
         * way that it is always true for at least one of the cells
         *
         * \code{.cpp}
         *  celltree::point_traversal pt( mCellTree, &p[ 0 ] );
         *
         *  while ( const celltree::node* n = pt.next() )
         *  {
         *    const unsigned int* begin = &( mCellTree.leaves[ n->start() ] );
         *    const unsigned int* end   = begin + n->size();
         *
         *    for ( ; begin != end; ++begin )
         *    {
         *      Cell c = (*mCells)[ *begin ];
         *      if ( contains( c, p ) )
         *        return c;
         *    }
         *  }
         *  \endcode
         */
        struct point_traversal
        {
            const celltree& m_ct;
            unsigned int m_stack[32];
            unsigned int* m_sp;
            const double* m_pos;

            /**
             * \brief Constructor
             *
             * \param ct the cell tree to search in
             * \param pos a position of the same dimension as the tree,
             *            for which the locator searches the tree.
             */
            point_traversal( const celltree& ct, const double* pos );

            /**
             * Iterate through the valid nodes
             * \returns the next matching node in the graph of null pointer if
             *          no further nodes are found
             */
            const celltree::node* next();
        };
    };

    /**
     * \brief
     * Builds the cell tree
     */
    template < std::size_t D >
    class celltree_builder
    {
    private:
        struct bucket
        {
            float min;
            float max;
            unsigned int cnt;

            bucket()
            {
                cnt = 0;
                min = std::numeric_limits< float >::max();
                max = -std::numeric_limits< float >::max();
            }

            void add( const float _min, const float _max )
            {
                ++cnt;

                if( _min < min )
                {
                    min = _min;
                }

                if( _max > max )
                {
                    max = _max;
                }
            }
        };

        struct per_cell
        {
            unsigned int ind;
            float min[D];
            float max[D];
        };

        struct center_order
        {
            unsigned int d;

            center_order( unsigned int _d )
                : d( _d )
            {
            }

            bool operator()( const per_cell& pc0, const per_cell& pc1 )
            {
                return pc0.min[d] + pc0.max[d] < pc1.min[d] + pc1.max[d];
            }
        };

        struct left_predicate
        {
            unsigned int d;
            float p;

            left_predicate( unsigned int _d, float _p )
                : d( _d )
                , p( _p )
            {
            }

            bool operator()( const per_cell& pc )
            {
                return ( pc.min[d] + pc.max[d] ) / 2.0 < p;
            }
        };

        void find_min_max( const per_cell* begin, const per_cell* end, float* min, float* max )
        {
            if( begin == end )
            {
                return;
            }

            for( unsigned int d = 0; d < D; ++d )
            {
                min[d] = begin->min[d];
                max[d] = begin->max[d];
            }

            while( ++begin != end )
            {
                for( unsigned int d = 0; d < D; ++d )
                {
                    if( begin->min[d] < min[d] )
                    {
                        min[d] = begin->min[d];
                    }
                    if( begin->max[d] > max[d] )
                    {
                        max[d] = begin->max[d];
                    }
                }
            }
        }

        void find_min_d( const per_cell* begin, const per_cell* end, unsigned int d, float& min )
        {
            min = begin->min[d];

            while( ++begin != end )
            {
                if( begin->min[d] < min )
                {
                    min = begin->min[d];
                }
            }
        }

        void find_max_d( const per_cell* begin, const per_cell* end, unsigned int d, float& max )
        {
            max = begin->max[d];

            while( ++begin != end )
            {
                if( begin->max[d] > max )
                {
                    max = begin->max[d];
                }
            }
        }

        void recursive_split( unsigned int index, per_cell* first, per_cell* begin, per_cell* end )
        {
            size_t size = end - begin;

            if( size < m_leafsize )
            {
                return;
            }

            float cost = std::numeric_limits< float >::max();
            float plane = 0.0f;
            int dim = 0;
            float clip[2];

            float min[D], max[D], ext[D];
            for( unsigned int d = 0; d < D; ++d )
            {
                min[d] = max[d] = 0.0f;
            }

            find_min_max( begin, end, min, max );

            for( unsigned int d = 0; d < D; ++d )
            {
                ext[d] = max[d] - min[d];
            }

            per_cell* mid = begin;

            const unsigned int nbuckets = m_buckets;

            bucket b[3][m_buckets];

            for( const per_cell* pc = begin; pc != end; ++pc )
            {
                for( unsigned int d = 0; d < D; ++d )
                {
                    if( ext[d] == 0 )
                    {
                        continue;
                    }

                    float cen = 0.5 * ( pc->min[d] + pc->max[d] );

                    // the computation of ceil(...)-1 could be negative for axis-aligned Hyperplanes (dimension D-1)
                    // that are located at the left of the bounding box, so that cen - min[d] == 0.
                    // If cen - min[d] > 0, everything is fine. Using floor instead of ceil and omitting the -1 would
                    // result in problems for cen - min[d] < 0 (because of numerical errors). Therefore, we just
                    // clip possible negative indices to 0 or greater.
                    int ind = std::max( 0, int( std::ceil( nbuckets * ( cen - min[d] ) / ext[d] ) ) - 1 );
                    b[d][ind].add( pc->min[d], pc->max[d] );
                }
            }

            for( unsigned int d = 0; d < D; ++d )
            {
                unsigned int sum = 0;
                for( unsigned int n = 0; n < nbuckets - 1; ++n )
                {
                    float lmax = -std::numeric_limits< float >::max();
                    float rmin = std::numeric_limits< float >::max();

                    for( unsigned int m = 0; m <= n; ++m )
                    {
                        if( b[d][m].max > lmax )
                        {
                            lmax = b[d][m].max;
                        }
                    }

                    for( unsigned int m = n + 1; m < nbuckets; ++m )
                    {
                        if( b[d][m].min < rmin )
                        {
                            rmin = b[d][m].min;
                        }
                    }

                    sum += b[d][n].cnt;

                    float lvol = ( lmax - min[d] ) / ext[d];
                    float rvol = ( max[d] - rmin ) / ext[d];

                    float c = lvol * sum + rvol * ( size - sum );

                    if( sum > 0 && sum < size && c < cost )
                    {
                        cost = c;
                        dim = d;
                        plane = min[d] + ( n + 1 ) * ext[d] / nbuckets;
                        clip[0] = lmax;
                        clip[1] = rmin;
                    }
                }
            }

            // fallback
            if( cost != std::numeric_limits< float >::max() )
            {
                mid = std::partition( begin, end, left_predicate( dim, plane ) );
            }

            if( mid == begin || mid == end )
            {
                dim = std::min_element( ext, ext + D ) - ext;

                mid = begin + ( end - begin ) / 2;
                std::nth_element( begin, mid, end, center_order( dim ) );
            }

            find_max_d( begin, mid, dim, clip[0] );
            find_min_d( mid, end, dim, clip[1] );

            celltree::node child[2];
            child[0].make_leaf( begin - first, mid - begin );
            child[1].make_leaf( mid - first, end - mid );

            size_t left, right;
#pragma omp critical( m_nodes )
            {
                m_nodes[index].make_node( m_nodes.size(), dim, clip );
                m_nodes.insert( m_nodes.end(), child, child + 2 );
                left = m_nodes[index].left();
                right = m_nodes[index].right();
            }
#pragma omp task
            recursive_split( left, first, begin, mid );
#pragma omp task
            recursive_split( right, first, mid, end );
        }

    public:
        /**
         * Cells is an index-accessible array of size size of cells.
         * A cell must provide the functions
         * - c.index( size_t i ), which returns the index of the i-th vertex in the cell
         * - c.numVertices(), which returns the number of vertices in the cell
         *
         * Points is an index-accessible array of point locations.
         * A point must provide an index operator p( dim ), which returns the dim-th
         * coordinate of the point for 0 <= dim < D.
         */
        template < class Cells, class Points >
        void build( celltree& ct, std::size_t size, const Cells& cells, const Points& points )
        {
            auto m_pc = new per_cell[size];

#pragma omp parallel
            {
#pragma omp for
                for( size_t i = 0; i < size; ++i )
                {
                    m_pc[i].ind = i;
                    auto c = cells[i];
                    auto p = points[c.index( 0 )];
                    for( size_t d = 0; d != D; ++d )
                    {
                        m_pc[i].min[d] = m_pc[i].max[d] = p[d];
                    }

                    for( size_t k = 1; k != c.numVertices(); ++k )
                    {
                        p = points[c.index( k )];
                        for( size_t d = 0; d != D; ++d )
                        {
                            if( m_pc[i].min[d] > p[d] )
                            {
                                m_pc[i].min[d] = p[d];
                            }
                            if( m_pc[i].max[d] < p[d] )
                            {
                                m_pc[i].max[d] = p[d];
                            }
                        }
                    }
                }
#pragma omp barrier

#pragma omp single nowait
                {
                    celltree::node root;
                    root.make_leaf( 0, size );
                    m_nodes.push_back( root );

                    recursive_split( 0, m_pc, m_pc, m_pc + size );
                }
#pragma omp barrier

#pragma omp single
                {
                    ct.nodes.resize( m_nodes.size() );
                    ct.nodes[0] = m_nodes[0];

                    std::vector< celltree::node >::iterator ni = ct.nodes.begin();
                    std::vector< celltree::node >::iterator nn = ct.nodes.begin() + 1;

                    for( ; ni != ct.nodes.end(); ++ni )
                    {
                        if( ni->is_leaf() )
                        {
                            continue;
                        }

                        *( nn++ ) = m_nodes[ni->left()];
                        *( nn++ ) = m_nodes[ni->right()];

                        ni->set_children( nn - ct.nodes.begin() - 2 );
                    }

                    ct.leaves.resize( size );
                }
#pragma omp barrier

#pragma omp for
                for( std::size_t i = 0; i < size; ++i )
                {
                    ct.leaves[i] = m_pc[i].ind;
                }
            }

            delete[] m_pc;
        }

    public:
        //! the number of buckets
        static const unsigned int m_buckets = 5;

        //! the number of cells in each leaf
        // \todo check for an appropriate size. Christian uses 4, VTK uses 8
        static const unsigned int m_leafsize = 4;
        std::vector< celltree::node > m_nodes;
    };
}
