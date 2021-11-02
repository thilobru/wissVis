#pragma once

#include <cstddef>
#include <iostream>
#include <stdexcept>

#include "exceptions.hpp"

namespace fantom
{
    const std::string cellNames[] = {
        "Points",
        "Lines",
        "Triangles",
        "Quads",
        "Tetrahedra",
        "Pyramids",
        "Prisms",
        "Hexahedra",
        "Subparametric Quadratic Line",
        "Subparametric Quadratic Triangle",
        "Subparametric Quadratic Quad",
        "Subparametric Quadratic Tetrahedron",
        "Subparametric Quadratic Hexahedron",
        // quadratic cell
        "Quadratic Line",
        "Quadratic Triangle",
        "Quadratic Quad",
        "Quadratic Tetrahedron",
        "Quadratic Hexahedron",
        "INVALID" // must be last to be able to iterate through types
    };

    /// reference to a grid cell \ingroup fields
    /**
     * Undefined behaviour occurs when a member of this class is called after the
     * grid that this cell belongs to was destroyed.
     * \throw No member throws, with the exception of apply().
     * \note All members are thread-safe.
     *
     * The following vertex numbering is implemented in the cells:
     * \code
     * Triangle    Quad
     *
     *    0       0----3
     *   / \      |    |
     *  /   \     |    |
     * 1-----2    1----2
     *
     *
     *   Tetrahedron        Prism        Pyramid       Hexahedron
     *
     *                         3                        4--------5
     *        3              / |\             4        /|       /|
     *       /|\           /   | \          //|\      / |      / |
     *      / | \        5--------4        // | \    7--------6  |
     *     /  |  \       |     |  |       //  |  \   |  |     |  |
     *    /   2   \      |     2  |      /3---|--2   |  3-----|--2
     *   /  /   \  \     |   /  \ |     //    | /    | /      | /
     *  / /       \ \    | /     \|    //     |/     |/       |/
     * 0-------------1   0--------1   0-------1      0--------1
     *
     * in the back:  2       2, 3          2, 3       2, 3, 4, 5
     *
     *
     * Quadratic Line
     *
     *    0--2--1
     *
     * QuadTriangle    QuadQuad
     *
     *      0             0--7--3
     *     / \            |     |
     *    3   5           4     6
     *  /       \         |     |
     * 1----4----2        1--5--2
     *
     *   Tetrahedron        Prism        Pyramid       Hexahedron
     *
     *                          3                        4---12---5
     *        3              x/ |\x            4      15/|     13/|
     *       /|\            /   x \          //|\      / 19     / |
     *      / 9 \         5---x----4        // | \    7----14--6  18
     *     /  |  \        |     |  |       //  |  \   |  |     |  |
     *    5   2   8       |     2  |      /3---|--2   |  3---10|--2
     *   /  /   \  \      |   /  \ |     //    | /   16 /     17 /
     *  /  6     5  \     x  x   x x                  | 11       9
     * / /         \ \    | /     \|    //     |/     |/       |/
     * 0------4------1    0----x---1   0-------1      0---8----1
     *
     * in the back:  2       2, 3          2, 3       2, 3, 4, 5
     * \endcode
     */
    class Cell
    {

    public:
        /// cell type
        enum Type
        {
            // linear cell types
            POINT = 0,       ///< point
            LINE = 1,        ///< line
            TRIANGLE = 2,    ///< triangle
            QUAD = 3,        ///< quad
            TETRAHEDRON = 4, ///< tetrahedron
            PYRAMID = 5,     ///< pyramid
            PRISM = 6,       ///< prism
            HEXAHEDRON = 7,  ///< hexahedron
            // cell types that are quadratic but only in value not shape
            // ( the extra points lie on the edge centers of linear cells )
            SUBPARAMETRIC_QUADRATIC_LINE,
            SUBPARAMETRIC_QUADRATIC_TRIANGLE,
            SUBPARAMETRIC_QUADRATIC_QUAD,
            SUBPARAMETRIC_QUADRATIC_TETRAHEDRON,
            SUBPARAMETRIC_QUADRATIC_HEXAHEDRON,
            // quadratic cell
            QUADRATIC_LINE,
            QUADRATIC_TRIANGLE,
            QUADRATIC_QUAD,
            QUADRATIC_TETRAHEDRON,
            QUADRATIC_HEXAHEDRON,
            INVALID // must be last to be able to iterate through types
        };

        /// cell type visitor
        struct Visitor
        {

            /// Process point cell.
            virtual void processPoint( const Cell& ) const
            {
            }

            /// Process line cell.
            virtual void processLine( const Cell& ) const
            {
            }

            /// Process triangle cell.
            virtual void processTriangle( const Cell& ) const
            {
            }

            /// Process quad cell.
            virtual void processQuad( const Cell& ) const
            {
            }

            /// Process tetrahedron cell.
            virtual void processTetrahedron( const Cell& ) const
            {
            }

            /// Process pyramid cell.
            virtual void processPyramid( const Cell& ) const
            {
            }

            /// Process prism cell.
            virtual void processPrism( const Cell& ) const
            {
            }

            /// Process hexahdron cell.
            virtual void processHexahedron( const Cell& ) const
            {
            }

            /// Process subparametric quadratic line cell.
            virtual void processSubparametricQuadraticLine( const Cell& ) const
            {
            }

            /// Process quadratic line cell.
            virtual void processQuadraticLine( const Cell& ) const
            {
            }

            /// Process subparametric quadratic triangle cell.
            virtual void processSubparametricQuadraticTriangle( const Cell& ) const
            {
            }

            /// Process quadratic triangle cell.
            virtual void processQuadraticTriangle( const Cell& ) const
            {
            }

            /// Process quadratic quad cell.
            virtual void processQuadraticQuad( const Cell& ) const
            {
            }

            /// Process subparametric quadratic quad cell.
            virtual void processSubparametricQuadraticQuad( const Cell& ) const
            {
            }

            /// Process tetrahedronratic tetrahedron cell.
            virtual void processQuadraticTetrahedron( const Cell& ) const
            {
            }

            /// Process subparametric tetrahedronratic tetrahedron cell.
            virtual void processSubparametricQuadraticTetrahedron( const Cell& ) const
            {
            }

            /// Process hexahedronratic hexahedron cell.
            virtual void processQuadraticHexahedron( const Cell& ) const
            {
            }

            /// Process subparametric hexahedronratic hexahedron cell.
            virtual void processSubparametricQuadraticHexahedron( const Cell& ) const
            {
            }


        protected:
            virtual ~Visitor();
            Visitor& operator=( const Visitor& );
        };

        /// \internal cell stats
        struct Stats
        {
            const Type mType;
            const size_t mDimension;
            const size_t mNumPoints;
            const size_t mNumValues;
            const size_t mNumFaces;
            void ( Visitor::*const mVisitor )( const Cell& ) const;
        };

        /// \internal strategy for cell implementations
        struct Strategy
        {

            const Stats& mStats;

            Strategy( const Stats& stats )
                : mStats( stats )
            {
            }

            virtual ~Strategy();
            virtual size_t index( size_t, size_t ) const = 0;
            virtual const Strategy* face( size_t ) const = 0;
        };

        /// Creates an invalid cell.
        Cell()
            : mStrategy( nullptr )
        {
        }

        /// Determines whether the cell is valid.
        explicit operator bool() const
        {
            return mStrategy != nullptr;
        }

        /// Determines the cell type.
        Type type() const
        {
#ifndef NDEBUG
            if( !*this )
            {
                throw fantom::logic_error( "Invalid cell!" );
            }
#endif
            return mStrategy->mStats.mType;
        }

        /// Determines the cell's topological dimension.
        size_t dimension() const
        {
#ifndef NDEBUG
            if( !*this )
                throw fantom::logic_error( "Invalid cell!" );
#endif
            return mStrategy->mStats.mDimension;
        }

        /// Determines the number of control points.
        /** @note the number of spatial control points may
         *        differ from that of the number of values
         *        stored in a cell @sa numValues
         */
        size_t numVertices() const
        {
#ifndef NDEBUG
            if( !*this )
            {
                throw fantom::logic_error( "Invalid cell!" );
            }
#endif
            return mStrategy->mStats.mNumPoints;
        }

        /// Determines the number of values in a cell.
        size_t numValues() const
        {
#ifndef NDEBUG
            if( !*this )
            {
                throw fantom::logic_error( "Invalid cell!" );
            }
#endif
            return mStrategy->mStats.mNumValues;
        }

        /// Determines the i-th control point.
        /**
         * \param i control point index.
         * \pre 0 <= \a i < \c numVertices().
         */
        size_t index( size_t i ) const
        {
#ifndef NDEBUG
            if( i >= numVertices() )
            {
                throw fantom::logic_error( "Control point index out of bounds." );
            }
#endif
            return mStrategy->index( mBase, i );
        }

        /// Determines the number of incident faces.
        size_t numFaces() const
        {
#ifndef NDEBUG
            if( !*this )
            {
                throw fantom::logic_error( "Invalid cell!" );
            }
#endif
            return mStrategy->mStats.mNumFaces;
        }

        /// Determines the i-th incident face.
        /**
         * \param i face index.
         * \pre 0 <= \a i < \c numFaces().
         */
        Cell face( size_t i ) const
        {
#ifndef NDEBUG
            if( i >= numFaces() )
            {
                throw fantom::out_of_range( "Face index out of bounds." );
            }
#endif
            return Cell( *mStrategy->face( i ), mBase );
        }

        /// Applies visitor based on cell type.
        /**
         * \param visitor the visitor.
         * \throw this member does not throw itself, but the member of \a visitor
         *        that is called may throw. In this case the safety guarantee is the
         *        same as that of the called function.
         */
        void apply( const Visitor& visitor ) const
        {
#ifndef NDEBUG
            if( !*this )
            {
                throw fantom::logic_error( "Invalid cell!" );
            }
#endif
            ( visitor.*( mStrategy->mStats.mVisitor ) )( *this );
        }

        bool operator==( const Cell& rhs )
        {
            return rhs.mStrategy == mStrategy && rhs.mBase == mBase;
        }

        bool operator!=( const Cell& rhs )
        {
            return !( *this == rhs );
        }

    public: // FIXME: this should be private...
        Cell( const Strategy& strategy, size_t base )
            : mStrategy( &strategy )
            , mBase( base )
        {
        }

        const Strategy* mStrategy;
        size_t mBase;
    };

    inline Cell::Visitor::~Visitor() = default;
    inline Cell::Visitor& Cell::Visitor::operator=( const Visitor& ) = default;
    inline Cell::Strategy::~Strategy() = default;

    inline std::ostream& operator<<( std::ostream& o, const Cell::Type& type )
    {
        static const char* const typenames[] = { "POINT",
                                                 "LINE",
                                                 "TRIANGLE",
                                                 "QUAD",
                                                 "TETRAHEDRON",
                                                 "PYRAMID",
                                                 "PRISM",
                                                 "HEXAHEDRON",
                                                 "SUBPARAMETRIC_QUADRATIC_LINE",
                                                 "SUBPARAMETRIC_QUADRATIC_TRIANGLE",
                                                 "SUBPARAMETRIX_QUADRATIC_QUAD",
                                                 "SUBPARAMETRIC_QUADRATIC_TETRAHEDRON",
                                                 "SUBPARAMETRIC_QUADRATIC_HEXAHEDRON",
                                                 "QUADRATIC_LINE",
                                                 "QUADRATIC_TRIANGLE",
                                                 "QUADRATIC_QUAD",
                                                 "QUADRATIC_TETRAHEDRON",
                                                 "QUADRATIC_HEXAHEDRON",
                                                 "INVALID" };
        if( type >= 0 && type < Cell::INVALID )
        {
            o << typenames[size_t( type )] << "(" << int( type ) << ")";
        }
        else if( type == Cell::INVALID )
        {
            o << typenames[size_t( type )] << "(" << int( type ) << ")";
        }
        else
        {
            o << "( internal error ) unknown cell type: " << int( type );
        }
        return o;
    }
}
