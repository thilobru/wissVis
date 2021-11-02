#pragma once

#include <unordered_map>

#include <fantom/dataset.hpp>
#include <fantom/graphics.hpp>
#include <fantom/math.hpp>

#include <fantom-plugins/utils/Graphics/HelperFunctions.hpp>
#include <fantom-plugins/utils/Graphics/Helpers.hpp>

namespace fantom
{
    namespace graphics
    {

        namespace private_namespace
        {
            /**
             * The grid visitor is responsible for converting the given grid to a geometry representation
             * This cell-based version duplicates grid points to be able to set attributes on a "per-cell"
             * basis, i.e., all vertices belonging to the cell can have the same attribute values.
             */

            /*
            template < size_t D >
            struct GridVisitorCellBased : public Cell::Visitor
            {
                const ValueArray< Tensor< double, D > >& mAllPoints;
                std::vector< unsigned int >& mVertexIndices;
                std::vector< unsigned int >& mCellIndices;
                std::vector< Tensor< float, 3 > >& mPoints;
                std::vector< unsigned int >& mTriangleIndices;
                std::vector< unsigned int >& mLineIndices;
                std::vector< unsigned int >& mPointIndices;
                size_t& mNumQuads;
                mutable std::unordered_map< size_t, size_t > point2point;

                mutable size_t mCellIndex;

                GridVisitorCellBased( const ValueArray< Tensor< double, D > >& allPoints,
                                      std::vector< unsigned int >& vertexIndices,
                                      std::vector< unsigned int >& cellIndices,
                                      std::vector< PointF< 3 > >& points,
                                      std::vector< unsigned int >& triangles,
                                      std::vector< unsigned int >& lines,
                                      std::vector< unsigned int >& pointIndices,
                                      size_t& numQuads )
                    : mAllPoints( allPoints )
                    , mVertexIndices( vertexIndices )
                    , mCellIndices( cellIndices )
                    , mPoints( points )
                    , mTriangleIndices( triangles )
                    , mLineIndices( lines )
                    , mPointIndices( pointIndices )
                    , mNumQuads( numQuads )
                {
                    mPoints.clear();
                    mTriangleIndices.clear();
                    mLineIndices.clear();
                    mPointIndices.clear();
                    mCellIndex = 0;
                    mNumQuads = 0;
                }

                /// put point into points if necessary
                size_t point( const size_t pi ) const
                {
                    Tensor< float, 3 > temp( fantom::toPoint3( mAllPoints[pi] ) );
                    mPoints.push_back( temp );
                    mCellIndices.push_back( mCellIndex );
                    return mPoints.size() - 1;
                }

                virtual void processPoint( const Cell& c ) const override
                {
                    mPointIndices.push_back( point( c.index( 0 ) ) );
                    ++mCellIndex;
                }

                virtual void processLine( const Cell& c ) const override
                {
                    for( size_t i = 0; i < 2; i++ )
                    {
                        mLineIndices.push_back( point( c.index( i ) ) );
                    }
                    ++mCellIndex;
                }

                virtual void processTriangle( const Cell& c ) const override
                {
                    for( size_t i = 0; i < 3; i++ )
                    {
                        mTriangleIndices.push_back( point( c.index( i ) ) );
                    }
                    ++mCellIndex;
                }

                virtual void processQuad( const Cell& c ) const override
                {
                    size_t triangles[] = { 0, 2, 1, 3, 2, 0 };
                    for( size_t i = 0; i < 6; i++ )
                    {
                        mTriangleIndices.push_back( point( c.index( triangles[i] ) ) );
                    }
                    ++mCellIndex;
                    ++mNumQuads;
                }

                virtual void processTetrahedron( const Cell& ) const override
                {
                    ++mCellIndex;
                    throw std::logic_error( "Cell type Tetrahedron occurred where it should never be" );
                }

                virtual void processPyramid( const Cell& ) const override
                {
                    ++mCellIndex;
                    throw std::logic_error( "Cell type Pyramid occurred where it should never be" );
                }

                virtual void processPrism( const Cell& ) const override
                {
                    ++mCellIndex;
                    throw std::logic_error( "Cell type Prism occurred where it should never be" );
                }

                virtual void processHexahedron( const Cell& ) const override
                {
                    ++mCellIndex;
                    throw std::logic_error( "Cell type Hexahedron occurred where it should never be" );
                }

                virtual void processQuadraticQuad( const Cell& c ) const override
                {
                    // for debugging only:
                    // decompose quad in five triangles: two splitting the center quad, four on the sides
                    // this is for convex cells, only, so it is useless.
                    //
                    // a split with an additional centerpoint is the way to go
                    size_t triangles[] = { 0, 1, 2, 0, 2, 3, 0, 4, 1, 1, 5, 2, 2, 6, 3, 3, 7, 0 };
                    for( size_t i = 0; i < 3 * 6; ++i )
                    {
                        mTriangleIndices.push_back( point( c.index( triangles[i] ) ) );
                    }
                    ++mCellIndex;
                }
            };
            */


            /**
             * the grid visitor is responsible for converting the given grid to a geometry representation
             */
            template < size_t D >
            struct GridVisitor : public Cell::Visitor
            {
                const ValueArray< Tensor< double, D > >& mAllPoints;
                std::vector< unsigned int >& mVertexIndices;
                std::vector< unsigned int >& mCellIndices;
                std::vector< PointF< 3 > >& mPoints;
                std::vector< unsigned int >& mTriangleIndices;
                std::vector< unsigned int >& mLineIndices;
                std::vector< unsigned int >& mPointIndices;
                size_t& mNumQuads;
                mutable size_t cellIndex;
                mutable std::unordered_map< unsigned int, unsigned int > point2point;

                GridVisitor( const ValueArray< Tensor< double, D > >& allPoints,
                             std::vector< unsigned int >& vertexIndices,
                             std::vector< unsigned int >& cellIndices,
                             std::vector< PointF< 3 > >& points,
                             std::vector< unsigned int >& triangles,
                             std::vector< unsigned int >& lines,
                             std::vector< unsigned int >& pointIndices,
                             size_t& numQuads )
                    : mAllPoints( allPoints )
                    , mVertexIndices( vertexIndices )
                    , mCellIndices( cellIndices )
                    , mPoints( points )
                    , mTriangleIndices( triangles )
                    , mLineIndices( lines )
                    , mPointIndices( pointIndices )
                    , mNumQuads( numQuads )
                {
                    mPoints.clear();
                    mTriangleIndices.clear();
                    mLineIndices.clear();
                    mPointIndices.clear();
                    mNumQuads = 0;
                    cellIndex = 0;
                }

                /// put point into points if necessary
                unsigned int point( const unsigned int pi ) const
                {
                    auto i = point2point.find( pi );
                    if( i == point2point.end() )
                    {
                        i = point2point.insert( std::make_pair( pi, mPoints.size() ) ).first;
                        mPoints.push_back( PointF< 3 >( fantom::toPoint3( mAllPoints[pi] ) ) );
                        mVertexIndices.push_back( pi );
                    }
                    return i->second;
                }

                virtual void processPoint( const Cell& c ) const override
                {
                    mPointIndices.push_back( point( c.index( 0 ) ) );
                    mCellIndices.push_back( cellIndex );
                    cellIndex++;
                }

                virtual void processLine( const Cell& c ) const override
                {
                    for( unsigned int i = 0; i < 2; i++ )
                    {
                        mLineIndices.push_back( point( c.index( i ) ) );
                    }
                    mCellIndices.push_back( cellIndex );
                    cellIndex++;
                }

                virtual void processTriangle( const Cell& c ) const override
                {
                    for( unsigned int i = 0; i < 3; i++ )
                    {
                        mTriangleIndices.push_back( point( c.index( i ) ) );
                    }
                    mCellIndices.push_back( cellIndex );
                    cellIndex++;
                }

                virtual void processQuad( const Cell& c ) const override
                {
                    unsigned int triangles[] = { 0, 2, 1, 3, 2, 0 };
                    for( unsigned int i = 0; i < 6; i++ )
                    {
                        mTriangleIndices.push_back( point( c.index( triangles[i] ) ) );
                    }
                    mCellIndices.push_back( cellIndex );
                    cellIndex++;
                    mNumQuads++;
                }

                virtual void processTetrahedron( const Cell& ) const override
                {
                    throw std::logic_error( "Cell type Tetrahedron occurred where it should never be" );
                }

                virtual void processPyramid( const Cell& ) const override
                {
                    throw std::logic_error( "Cell type Pyramid occurred where it should never be" );
                }

                virtual void processPrism( const Cell& ) const override
                {
                    throw std::logic_error( "Cell type Prism occurred where it should never be" );
                }

                virtual void processHexahedron( const Cell& ) const override
                {
                    throw std::logic_error( "Cell type Hexahedron occurred where it should never be" );
                }

                virtual void processQuadraticQuad( const Cell& c ) const override
                {
                    // for debugging only:
                    // decompose quad in five triangles: two splitting the center quad, four on the sides
                    // this is for convex cells, only, so it is useless.
                    //
                    // a split with an additional centerpoint is the way to go
                    unsigned int triangles[] = { 0, 1, 2, 0, 2, 3, 0, 4, 1, 1, 5, 2, 2, 6, 3, 3, 7, 0 };
                    for( unsigned int i = 0; i < 3 * 6; ++i )
                    {
                        mTriangleIndices.push_back( point( c.index( triangles[i] ) ) );
                    }

                    mCellIndices.push_back( cellIndex );
                    cellIndex++;
                }
            };
        } // namespace private_namespace

        class GridRenderer
        {
        public:
            GridRenderer( const graphics::GraphicsSystem& system )
                : mIsPointBased( true )
                , mIs2D( false )
                , mShadingType( ShadingType::NO_SHADING )
                , mLineWidth( 1.0 )
                , mResPath( PluginRegistrationService::getInstance().getResourcePath( "utils/Graphics" ) )
                , mSystem( system )
            {
            }

            GridRenderer& setShadingType( ShadingType type )
            {
                if( ( ( mShadingType == ShadingType::BASIC ) || ( mShadingType == ShadingType::PHONG ) )
                    && ( type == ShadingType::NO_SHADING ) )
                {
                    clearSurfaceNormals();
                }

                if( ( ( type == ShadingType::BASIC ) || ( type == ShadingType::PHONG ) )
                    && ( mShadingType == ShadingType::NO_SHADING ) )
                {
                    this->computeSurfaceNormals();
                }

                mShadingType = type;
                return *this;
            }

            GridRenderer& setLineWidth( float width )
            {
                mLineWidth = width;

                return *this;
            }

            /// convenience function using a pointer
            GridRenderer& createMesh( std::shared_ptr< const PointSetBase > domain )
            {
                return createMesh( *domain );
            }
            /// create the internal graphical representation from a grid
            GridRenderer& createMesh( const PointSetBase& domain )
            {
                if( auto grid = dynamic_cast< const EmbeddedCellComplex< 3 >* >( &domain ) )
                {
                    return createMesh( *grid );
                }
                else if( auto grid = dynamic_cast< const EmbeddedCellComplex< 2 >* >( &domain ) )
                {
                    return createMesh( *grid );
                }
                else if( auto grid = dynamic_cast< const EmbeddedCellComplex< 1 >* >( &domain ) )
                {
                    return createMesh( *grid );
                }
                else
                {
                    throw fantom::logic_error( "Grid not a Grid of dimension 1, 2, or 3" );
                }
            }
            /**
             * Creates the grid for the visualization.
             * (convenience function)
             */
            template < size_t D >
            GridRenderer& createMesh( std::shared_ptr< const EmbeddedCellComplex< D > > grid )
            {
                return createMesh< D >( *grid );
            }
            /**
             * Extracts the data for visualization using the cell visitor on the given grid.
             * It stores results in
             * mGridIndices, mGridPositions, mGridTriangles, mGridLines, mGridPoints
             */
            template < size_t D >
            GridRenderer& createMesh( const EmbeddedCellComplex< D >& grid )
            {
                mGridIndices.clear();
                mGridPositions.clear();
                mGridTriangles.clear();
                mGridLines.clear();
                mGridPoints.clear();
                mIs2D = ( D == 2 );
                mCellIndices.clear();
                mQuadIndices.clear();
                mNumCells = grid.numCells();
                mNumQuads = 0;


                const size_t size = grid.numCells();
                // if( mIsPointBased )
                //{
                private_namespace::GridVisitor< D > visitor( grid.points(),
                                                             mGridIndices,
                                                             mCellIndices,
                                                             mGridPositions,
                                                             mGridTriangles,
                                                             mGridLines,
                                                             mGridPoints,
                                                             mNumQuads );
                for( unsigned int i = 0; i != size; ++i )
                {
                    grid.cell( i ).apply( visitor );
                    if( grid.cell( i ).type() == fantom::Cell::Type::QUAD )
                    {
                        mQuadIndices.push_back( i );
                    }
                }
                //}
                /*
                else
                {
                    private_namespace::GridVisitorCellBased< D > visitor( grid.points(),
                                                                          mGridIndices,
                                                                          mCellIndices,
                                                                          mGridPositions,
                                                                          mGridTriangles,
                                                                          mGridLines,
                                                                          mGridPoints,
                                                                          mNumQuads );
                    for( size_t i = 0; i != size; ++i )
                    {
                        grid.cell( i ).apply( visitor );
                        if( grid.cell( i ).type() == fantom::Cell::Type::QUAD )
                        {
                            mQuadIndices.push_back( i );
                        }
                    }
                }
                */

                if( mShadingType == ShadingType::NO_SHADING )
                {
                    this->clearSurfaceNormals();
                }
                else
                {
                    this->computeSurfaceNormals();
                }

                return *this;
            }

            /**
             * Computes the surface normals of triangles
             */
            void computeSurfaceNormals()
            {
                // fast track for 2D grids
                if( mIs2D )
                {
                    mSurfaceNormals.clear();
                    mSurfaceNormals.resize( mGridPositions.size(), VectorF< 3 >( 0.0, 0.0, 1.0 ) );
                    return;
                }
                mSurfaceNormals.clear();
                mSurfaceNormals = graphics::computeNormals( mGridPositions, mGridTriangles );
            }

            void clearSurfaceNormals()
            {
                mSurfaceNormals.clear();
            }


            std::shared_ptr< graphics::DrawableCompound > renderWithColor( Color color )
            {

                std::vector< std::shared_ptr< graphics::Drawable > > drawables;
                graphics::BoundingSphere bs;
                graphics::RenderBin bin
                    = color.a() < 1.0 ? graphics::RenderBin::Transparent : graphics::RenderBin::Opaque;

                if( !mGridTriangles.empty() || !mGridLines.empty() /* || !mGridPoints.empty */ )
                    bs = computeBoundingSphere( mGridPositions );

                if( mShadingType == ShadingType::NO_SHADING )
                {
                    if( !mGridTriangles.empty() )
                    {
                        auto drawableTriangles = mSystem.makePrimitive(
                            graphics::PrimitiveConfig{ graphics::RenderPrimitives::TRIANGLES }
                                .renderBin( bin )
                                .vertexBuffer( "position", mSystem.makeBuffer( mGridPositions ) )
                                .indexBuffer( mSystem.makeIndexBuffer( mGridTriangles ) )
                                .uniform( "color", color )
                                .boundingSphere( bs ),
                            mSystem.makeProgramFromFiles(
                                mResPath + "shader/surface/noShading/singleColor/vertex.glsl",
                                mResPath + "shader/surface/noShading/singleColor/fragment.glsl" ) );

                        drawables.push_back( drawableTriangles );
                    }

                    if( !mGridLines.empty() )
                    {
                        auto drawableLines = mSystem.makePrimitive(
                            graphics::PrimitiveConfig{ graphics::RenderPrimitives::LINES }
                                .renderBin( bin )
                                .vertexBuffer( "in_vertex", mSystem.makeBuffer( mGridPositions ) )
                                .indexBuffer( mSystem.makeIndexBuffer( mGridLines ) )
                                .uniform( "u_lineWidth", mLineWidth )
                                .uniform( "u_color", color )
                                .boundingSphere( bs ),
                            mSystem.makeProgramFromFiles( mResPath + "shader/line/noShading/singleColor/vertex.glsl",
                                                          mResPath + "shader/line/noShading/singleColor/fragment.glsl",
                                                          mResPath
                                                              + "shader/line/noShading/singleColor/geometry.glsl" ) );

                        drawables.push_back( drawableLines );
                    }

                    /*if( !mGridPoints.empty() )
                    {
                        auto drawablePoints = mSystem.makePrimitive(
                            graphics::PrimitiveConfig{ graphics::RenderPrimitives::POINTS }
                                .renderBin( bin )
                                .vertexBuffer( "position", mSystem.makeBuffer( mGridPositions ) )
                                .indexBuffer( mSystem.makeIndexBuffer( mGridPoints ) )
                                .uniform( "color", color )
                                .boundingSphere( bs ),
                            mSystem.makeProgramFromFiles( mResPath + "shader/surface/noShading/singleColor/vertex.glsl",
                                                          mResPath +
                    "shader/surface/noShading/singleColor/fragment.glsl" ) );

                       drawables.push_back( drawablePoints );
                    }*/
                }
                else if( mShadingType == ShadingType::BASIC )
                {
                    if( !mGridTriangles.empty() )
                    {
                        auto drawableTriangles = mSystem.makePrimitive(
                            graphics::PrimitiveConfig{ graphics::RenderPrimitives::TRIANGLES }
                                .renderBin( bin )
                                .vertexBuffer( "position", mSystem.makeBuffer( mGridPositions ) )
                                .vertexBuffer( "normal", mSystem.makeBuffer( mSurfaceNormals ) )
                                .indexBuffer( mSystem.makeIndexBuffer( mGridTriangles ) )
                                .uniform( "color", color )
                                .boundingSphere( bs ),
                            mSystem.makeProgramFromFiles( mResPath + "shader/surface/basic/singleColor/vertex.glsl",
                                                          mResPath
                                                              + "shader/surface/basic/singleColor/fragment.glsl" ) );

                        drawables.push_back( drawableTriangles );
                    }

                    if( !mGridLines.empty() )
                    {
                        auto drawableLines = mSystem.makePrimitive(
                            graphics::PrimitiveConfig{ graphics::RenderPrimitives::LINES }
                                .renderBin( bin )
                                .vertexBuffer( "in_vertex", mSystem.makeBuffer( mGridPositions ) )
                                .indexBuffer( mSystem.makeIndexBuffer( mGridLines ) )
                                .uniform( "u_lineWidth", mLineWidth )
                                .uniform( "u_color", color )
                                .boundingSphere( bs ),
                            mSystem.makeProgramFromFiles( mResPath + "shader/line/noShading/singleColor/vertex.glsl",
                                                          mResPath + "shader/line/noShading/singleColor/fragment.glsl",
                                                          mResPath
                                                              + "shader/line/noShading/singleColor/geometry.glsl" ) );

                        drawables.push_back( drawableLines );
                    }

                    /*if( !mGridPoints.empty() )
                    {
                        auto drawablePoints = mSystem.makePrimitive(
                            graphics::PrimitiveConfig{ graphics::RenderPrimitives::POINTS }
                                .renderBin( bin )
                                .vertexBuffer( "position", mSystem.makeBuffer( mGridPositions ) )
                                .vertexBuffer( "normal", mSystem.makeBuffer( mGridPositions ) )
                                .indexBuffer( mSystem.makeIndexBuffer( mGridPoints ) )
                                .uniform( "color", color)
                                .boundingSphere( bs ),
                            mSystem.makeProgramFromFiles( mResPath + "shader/surface/basic/singleColor/vertex.glsl",
                                                          mResPath + "shader/surface/basic/singleColor/fragment.glsl" )
                    );

                       drawables.push_back( drawablePoints );
                    }*/
                }

                else
                {
                    if( !mGridTriangles.empty() )
                    {
                        auto drawableTriangles = mSystem.makePrimitive(
                            graphics::PrimitiveConfig{ graphics::RenderPrimitives::TRIANGLES }
                                .renderBin( bin )
                                .vertexBuffer( "position", mSystem.makeBuffer( mGridPositions ) )
                                .vertexBuffer( "normal", mSystem.makeBuffer( mSurfaceNormals ) )
                                .indexBuffer( mSystem.makeIndexBuffer( mGridTriangles ) )
                                .uniform( "color", color )
                                .boundingSphere( bs ),
                            mSystem.makeProgramFromFiles( mResPath + "shader/surface/phong/singleColor/vertex.glsl",
                                                          mResPath
                                                              + "shader/surface/phong/singleColor/fragment.glsl" ) );

                        drawables.push_back( drawableTriangles );
                    }

                    if( !mGridLines.empty() )
                    {
                        auto drawableLines = mSystem.makePrimitive(
                            graphics::PrimitiveConfig{ graphics::RenderPrimitives::LINES }
                                .renderBin( bin )
                                .vertexBuffer( "in_vertex", mSystem.makeBuffer( mGridPositions ) )
                                .vertexBuffer( "in_normal", mSystem.makeBuffer( mGridPositions ) )
                                .indexBuffer( mSystem.makeIndexBuffer( mGridLines ) )
                                .uniform( "u_lineWidth", mLineWidth )
                                .uniform( "u_color", color )
                                .boundingSphere( bs ),
                            mSystem.makeProgramFromFiles( mResPath + "shader/line/phong/singleColor/vertex.glsl",
                                                          mResPath + "shader/line/phong/singleColor/fragment.glsl",
                                                          mResPath + "shader/line/phong/singleColor/geometry.glsl" ) );

                        drawables.push_back( drawableLines );
                    }

                    /*if( !mGridPoints.empty() )
                    {
                        auto drawablePoints = mSystem.makePrimitive(
                            graphics::PrimitiveConfig{ graphics::RenderPrimitives::POINTS }
                                .renderBin( bin )
                                .vertexBuffer( "position", mSystem.makeBuffer( mGridPositions ) )
                                .vertexBuffer( "normal", mSystem.makeBuffer( mGridPositions ) )
                                .indexBuffer( mSystem.makeIndexBuffer( mGridPoints ) )
                                .uniform( "color", color)
                                .boundingSphere( bs ),
                            mSystem.makeProgramFromFiles( mResPath + "shader/surface/phong/singleColor/vertex.glsl",
                                                          mResPath + "shader/surface/phong/singleColor/fragment.glsl" )
                    );

                       drawables.push_back( drawablePoints );
                    }*/
                }

                return graphics::makeCompound( drawables );
            }

            std::shared_ptr< graphics::DrawableCompound > renderWithColors( std::vector< Color > colors )
            {
                std::vector< std::shared_ptr< graphics::Drawable > > drawables;
                graphics::BoundingSphere bs;
                graphics::RenderBin bin = graphics::RenderBin::Opaque;
                for( auto& color : colors )
                {
                    if( color.a() < 1 )
                    {
                        bin = graphics::RenderBin::Transparent;
                        break;
                    }
                }

                if( mIsPointBased ) // perPointColoring
                {
                    if( colors.size() < mGridPositions.size() )
                    {
                        throw fantom::logic_error( "Number of grid-points exceeds number of input colors." );
                    }
                    else if( colors.size() > mGridPositions.size() )
                    {
                        throw fantom::logic_error( "Number of input colors exceeds number of grid points." );
                    }

                    if( !mGridTriangles.empty() || !mGridLines.empty() /* || !mGridPoints.empty */ )
                        bs = computeBoundingSphere( mGridPositions );

                    if( mShadingType == ShadingType::NO_SHADING )
                    {
                        if( !mGridTriangles.empty() )
                        {
                            auto drawableTriangles = mSystem.makePrimitive(
                                graphics::PrimitiveConfig{ graphics::RenderPrimitives::TRIANGLES }
                                    .renderBin( bin )
                                    .vertexBuffer( "position", mSystem.makeBuffer( mGridPositions ) )
                                    .vertexBuffer( "color", mSystem.makeBuffer( colors, graphics::ColorChannel::RGBA ) )
                                    .indexBuffer( mSystem.makeIndexBuffer( mGridTriangles ) )
                                    .boundingSphere( bs ),
                                mSystem.makeProgramFromFiles(
                                    mResPath + "shader/surface/noShading/multiColor/vertex.glsl",
                                    mResPath + "shader/surface/noShading/multiColor/fragment.glsl" ) );

                            drawables.push_back( drawableTriangles );
                        }

                        if( !mGridLines.empty() )
                        {
                            auto drawableLines = mSystem.makePrimitive(
                                graphics::PrimitiveConfig{ graphics::RenderPrimitives::LINES }
                                    .renderBin( bin )
                                    .vertexBuffer( "in_vertex", mSystem.makeBuffer( mGridPositions ) )
                                    .vertexBuffer( "in_color", mSystem.makeBuffer( colors ) )
                                    .indexBuffer( mSystem.makeIndexBuffer( mGridLines ) )
                                    .uniform( "u_lineWidth", mLineWidth )
                                    .boundingSphere( bs ),
                                mSystem.makeProgramFromFiles(
                                    mResPath + "shader/line/noShading/multiColor/vertex.glsl",
                                    mResPath + "shader/line/noShading/multiColor/fragment.glsl",
                                    mResPath + "shader/line/noShading/multiColor/geometry.glsl" ) );

                            drawables.push_back( drawableLines );
                        }

                        /*if( !mGridPoints.empty() )
                        {
                            auto drawablePoints = mSystem.makePrimitive(
                                graphics::PrimitiveConfig{ graphics::RenderPrimitives::POINTS }
                                .renderBin( bin )
                                    .vertexBuffer( "position", mSystem.makeBuffer( mGridPositions ) )v
                                    .vertexBuffer( "color", mSystem.makeBuffer( colors, graphics::ColorChannel::RGBA ) )
                                    .indexBuffer( mSystem.makeIndexBuffer( mGridPoints ) )
                                    .boundingSphere( bs ),
                                mSystem.makeProgramFromFiles( mResPath +
                        "shader/surface/noShading/multiColor/vertex.glsl",
                                                              mResPath +
                        "shader/surface/noShading/multiColor/fragment.glsl" ) );

                        drawables.push_back( drawablePoints );
                        }*/
                    }
                    else if( mShadingType == ShadingType::BASIC )
                    {
                        if( !mGridTriangles.empty() )
                        {
                            auto drawableTriangles = mSystem.makePrimitive(
                                graphics::PrimitiveConfig{ graphics::RenderPrimitives::TRIANGLES }
                                    .renderBin( bin )
                                    .vertexBuffer( "position", mSystem.makeBuffer( mGridPositions ) )
                                    .vertexBuffer( "normal", mSystem.makeBuffer( mSurfaceNormals ) )
                                    .vertexBuffer( "color", mSystem.makeBuffer( colors, graphics::ColorChannel::RGBA ) )
                                    .indexBuffer( mSystem.makeIndexBuffer( mGridTriangles ) )
                                    .boundingSphere( bs ),
                                mSystem.makeProgramFromFiles( mResPath + "shader/surface/basic/multiColor/vertex.glsl",
                                                              mResPath
                                                                  + "shader/surface/basic/multiColor/fragment.glsl" ) );

                            drawables.push_back( drawableTriangles );
                        }

                        if( !mGridLines.empty() )
                        {
                            auto drawableLines = mSystem.makePrimitive(
                                graphics::PrimitiveConfig{ graphics::RenderPrimitives::LINES }
                                    .renderBin( bin )
                                    .vertexBuffer( "in_vertex", mSystem.makeBuffer( mGridPositions ) )
                                    .vertexBuffer( "in_color", mSystem.makeBuffer( colors ) )
                                    .indexBuffer( mSystem.makeIndexBuffer( mGridLines ) )
                                    .uniform( "u_lineWidth", mLineWidth )
                                    .boundingSphere( bs ),
                                mSystem.makeProgramFromFiles(
                                    mResPath + "shader/line/noShading/multiColor/vertex.glsl",
                                    mResPath + "shader/line/noShading/multiColor/fragment.glsl",
                                    mResPath + "shader/line/noShading/multiColor/geometry.glsl" ) );

                            drawables.push_back( drawableLines );
                        }

                        /*if( !mGridPoints.empty() )
                        {
                            auto drawablePoints = mSystem.makePrimitive(
                                graphics::PrimitiveConfig{ graphics::RenderPrimitives::POINTS }
                                .renderBin( bin )
                                    .vertexBuffer( "position", mSystem.makeBuffer( mGridPositions ) )
                                    .vertexBuffer( "normal", mSystem.makeBuffer( mGridPositions ) )
                                    .vertexBuffer( "color", mSystem.makeBuffer( colors, graphics::ColorChannel::RGBA ) )
                                    .indexBuffer( mSystem.makeIndexBuffer( mGridPoints ) )
                                    .boundingSphere( bs ),
                                mSystem.makeProgramFromFiles( mResPath + "shader/surface/basic/multiColor/vertex.glsl",
                                                              mResPath + "shader/surface/basic/multiColor/fragment.glsl"
                        ) );

                        drawables.push_back( drawablePoints );
                        }*/
                    }

                    else
                    {
                        if( !mGridTriangles.empty() )
                        {
                            auto drawableTriangles = mSystem.makePrimitive(
                                graphics::PrimitiveConfig{ graphics::RenderPrimitives::TRIANGLES }
                                    .renderBin( bin )
                                    .vertexBuffer( "position", mSystem.makeBuffer( mGridPositions ) )
                                    .vertexBuffer( "normal", mSystem.makeBuffer( mSurfaceNormals ) )
                                    .vertexBuffer( "color", mSystem.makeBuffer( colors, graphics::ColorChannel::RGBA ) )
                                    .indexBuffer( mSystem.makeIndexBuffer( mGridTriangles ) )
                                    .boundingSphere( bs ),
                                mSystem.makeProgramFromFiles( mResPath + "shader/surface/phong/multiColor/vertex.glsl",
                                                              mResPath
                                                                  + "shader/surface/phong/multiColor/fragment.glsl" ) );

                            drawables.push_back( drawableTriangles );
                        }

                        if( !mGridLines.empty() )
                        {
                            auto drawableLines = mSystem.makePrimitive(
                                graphics::PrimitiveConfig{ graphics::RenderPrimitives::LINES }
                                    .renderBin( bin )
                                    .vertexBuffer( "in_vertex", mSystem.makeBuffer( mGridPositions ) )
                                    .vertexBuffer( "in_normal", mSystem.makeBuffer( mGridPositions ) )
                                    .vertexBuffer( "in_color", mSystem.makeBuffer( colors ) )
                                    .indexBuffer( mSystem.makeIndexBuffer( mGridLines ) )
                                    .uniform( "u_lineWidth", mLineWidth )
                                    .boundingSphere( bs ),
                                mSystem.makeProgramFromFiles( mResPath + "shader/line/phong/multiColor/vertex.glsl",
                                                              mResPath + "shader/line/phong/multiColor/fragment.glsl",
                                                              mResPath
                                                                  + "shader/line/phong/multiColor/geometry.glsl" ) );

                            drawables.push_back( drawableLines );
                        }

                        /*if( !mGridPoints.empty() )
                        {
                            auto drawablePoints = mSystem.makePrimitive(
                                graphics::PrimitiveConfig{ graphics::RenderPrimitives::POINTS }
                                .renderBin( bin )
                                    .vertexBuffer( "position", mSystem.makeBuffer( mGridPositions ) )
                                    .vertexBuffer( "normal", mSystem.makeBuffer( mGridPositions ) )
                                    .vertexBuffer( "color", mSystem.makeBuffer( colors, graphics::ColorChannel::RGBA ) )
                                    .indexBuffer( mSystem.makeIndexBuffer( mGridPoints ) )
                                    .boundingSphere( bs ),
                                mSystem.makeProgramFromFiles( mResPath + "shader/surface/phong/multiColor/vertex.glsl",
                                                              mResPath + "shader/surface/phong/multiColor/fragment.glsl"
                        ) );

                        drawables.push_back( drawablePoints );
                        }*/
                    }
                }
                else // perCellColoring
                {
                    if( colors.size() < mNumCells )
                    {
                        throw fantom::logic_error( "Number of grid-cells exceeds number of input colors." );
                    }
                    else if( colors.size() > mNumCells )
                    {
                        throw fantom::logic_error( "Number of input colors exceeds number of grid cells." );
                    }

                    // maximum extent for a single 3D texture dimension is 2048.
                    // Therefore we will split everything into a 3D respresentation
                    // Maximum texture storage is then 2^33
                    Size3D textureExtents;
                    {
                        size_t maxTexDim = 2048;
                        
                        size_t texture1Dsize = mNumCells + mNumQuads;
                        size_t zTexDimExtent = ceil( double(texture1Dsize) / double(maxTexDim * maxTexDim) );
                        double r = double(texture1Dsize) - double(zTexDimExtent - 1 ) * double(maxTexDim * maxTexDim);
                        size_t yTexDimExtent = ceil( r / double(maxTexDim) );
                        r = r - double(yTexDimExtent - 1 ) * double(maxTexDim);
                        size_t xTexDimExtent = yTexDimExtent > 1 ? maxTexDim : texture1Dsize;
                        
                        textureExtents[0] = xTexDimExtent;
                        textureExtents[1] = yTexDimExtent;
                        textureExtents[2] = zTexDimExtent;
                    }
     
                    
                    auto texture = mSystem.makeTexture( textureExtents, graphics::ColorChannel::RGBA );
                    std::vector< Color > tempColors;
                    size_t j = 0;
                    size_t nextQuad = mNumCells * 2;

                    if( !mQuadIndices.empty() )
                    {
                        nextQuad = mQuadIndices[j];
                    }

                    for( size_t i = 0; i < colors.size(); i++ )
                    {
                        tempColors.push_back( colors[i] );
                        if( i == nextQuad )
                        {
                            tempColors.push_back( colors[i] );
                            if( j < mQuadIndices.size() )
                            {
                                j++;
                                nextQuad = mQuadIndices[j];
                            }
                        }
                    }

                    tempColors.resize(textureExtents[0]*textureExtents[1] * textureExtents[2], Color(0,0,0,1));
                    if( !mGridTriangles.empty() || !mGridLines.empty() /* || !mGridPoints.empty */ )
                    {
                        texture->range( Pos3D(0,0,0), textureExtents, tempColors );
                        bs = computeBoundingSphere( mGridPositions );
                    }

                    // drawables
                    if( mShadingType == ShadingType::NO_SHADING )
                    {
                        if( !mGridTriangles.empty() )
                        {
                            auto drawableTriangles = mSystem.makePrimitive(
                                graphics::PrimitiveConfig{ graphics::RenderPrimitives::TRIANGLES }
                                    .renderBin( bin )
                                    .vertexBuffer( "position", mSystem.makeBuffer( mGridPositions ) )
                                    .indexBuffer( mSystem.makeIndexBuffer( mGridTriangles ) )
                                    .texture( "colormap", texture )
                                    .boundingSphere( bs ),
                                mSystem.makeProgramFromFiles(
                                    mResPath + "shader/surface/noShading/multiColorPerCell/vertex.glsl",
                                    mResPath + "shader/surface/noShading/multiColorPerCell/fragment.glsl" ) );

                            drawables.push_back( drawableTriangles );
                        }

                        if( !mGridLines.empty() )
                        {
                            auto drawableLines = mSystem.makePrimitive(
                                graphics::PrimitiveConfig{ graphics::RenderPrimitives::LINES }
                                    .renderBin( bin )
                                    .vertexBuffer( "in_vertex", mSystem.makeBuffer( mGridPositions ) )
                                    .indexBuffer( mSystem.makeIndexBuffer( mGridLines ) )
                                    .uniform( "u_lineWidth", mLineWidth )
                                    .texture( "colormap", texture )
                                    .boundingSphere( bs ),
                                mSystem.makeProgramFromFiles(
                                    mResPath + "shader/line/noShading/multiColorPerCell/vertex.glsl",
                                    mResPath + "shader/line/noShading/multiColorPerCell/fragment.glsl",
                                    mResPath + "shader/line/noShading/multiColorPerCell/geometry.glsl" ) );

                            drawables.push_back( drawableLines );
                        }

                        /*if( !mGridPoints.empty() )
                        {
                            auto drawablePoints = mSystem.makePrimitive(
                                graphics::PrimitiveConfig{ graphics::RenderPrimitives::POINTS }
                                    .renderBin( bin )
                                    .vertexBuffer( "position", mSystem.makeBuffer( mGridPositions ) )
                                    .indexBuffer( mSystem.makeIndexBuffer( mGridPoints ) )
                                    .texture( "colormap", texture )
                                    .boundingSphere( bs ),
                                mSystem.makeProgramFromFiles( mResPath +
                        "shader/surface/noShading/multiColorPerCell/vertex.glsl",
                                                              mResPath +
                        "shader/surface/noShading/multiColorPerCell/fragment.glsl" ) );

                        drawables.push_back( drawablePoints );
                        }*/
                    }
                    else if( mShadingType == ShadingType::BASIC )
                    {
                        if( !mGridTriangles.empty() )
                        {
                            auto drawableTriangles = mSystem.makePrimitive(
                                graphics::PrimitiveConfig{ graphics::RenderPrimitives::TRIANGLES }
                                    .renderBin( bin )
                                    .vertexBuffer( "position", mSystem.makeBuffer( mGridPositions ) )
                                    .vertexBuffer( "normal", mSystem.makeBuffer( mSurfaceNormals ) )
                                    .indexBuffer( mSystem.makeIndexBuffer( mGridTriangles ) )
                                    .texture( "colormap", texture )
                                    .boundingSphere( bs ),
                                mSystem.makeProgramFromFiles(
                                    mResPath + "shader/surface/basic/multiColorPerCell/vertex.glsl",
                                    mResPath + "shader/surface/basic/multiColorPerCell/fragment.glsl" ) );

                            drawables.push_back( drawableTriangles );
                        }

                        if( !mGridLines.empty() )
                        {
                            auto drawableLines = mSystem.makePrimitive(
                                graphics::PrimitiveConfig{ graphics::RenderPrimitives::LINES }
                                    .renderBin( bin )
                                    .vertexBuffer( "in_vertex", mSystem.makeBuffer( mGridPositions ) )
                                    .indexBuffer( mSystem.makeIndexBuffer( mGridLines ) )
                                    .uniform( "u_lineWidth", mLineWidth )
                                    .texture( "colormap", texture )
                                    .boundingSphere( bs ),
                                mSystem.makeProgramFromFiles(
                                    mResPath + "shader/line/noShading/multiColorPerCell/vertex.glsl",
                                    mResPath + "shader/line/noShading/multiColorPerCell/fragment.glsl",
                                    mResPath + "shader/line/noShading/multiColorPerCell/geometry.glsl" ) );

                            drawables.push_back( drawableLines );
                        }

                        /*if( !mGridPoints.empty() )
                        {
                            auto drawablePoints = mSystem.makePrimitive(
                                graphics::PrimitiveConfig{ graphics::RenderPrimitives::POINTS }
                                    .renderBin( bin )
                                    .vertexBuffer( "position", mSystem.makeBuffer( mGridPositions ) )
                                    .vertexBuffer( "normal", mSystem.makeBuffer( mGridPositions ) )
                                    .indexBuffer( mSystem.makeIndexBuffer( mGridPoints ) )
                                    .texture( "colormap", texture )
                                    .boundingSphere( bs ),
                                mSystem.makeProgramFromFiles( mResPath +
                        "shader/surface/basic/multiColorPerCell/vertex.glsl",
                                                              mResPath +
                        "shader/surface/basic/multiColorPerCell/fragment.glsl" ) );

                        drawables.push_back( drawablePoints );
                        }*/
                    }
                    else
                    {
                        if( !mGridTriangles.empty() )
                        {
                            auto drawableTriangles = mSystem.makePrimitive(
                                graphics::PrimitiveConfig{ graphics::RenderPrimitives::TRIANGLES }
                                    .renderBin( bin )
                                    .vertexBuffer( "position", mSystem.makeBuffer( mGridPositions ) )
                                    .vertexBuffer( "normal", mSystem.makeBuffer( mSurfaceNormals ) )
                                    .indexBuffer( mSystem.makeIndexBuffer( mGridTriangles ) )
                                    .texture( "colormap", texture )
                                    .boundingSphere( bs ),
                                mSystem.makeProgramFromFiles(
                                    mResPath + "shader/surface/phong/multiColorPerCell/vertex.glsl",
                                    mResPath + "shader/surface/phong/multiColorPerCell/fragment.glsl" ) );

                            drawables.push_back( drawableTriangles );
                        }

                        if( !mGridLines.empty() )
                        {
                            auto drawableLines = mSystem.makePrimitive(
                                graphics::PrimitiveConfig{ graphics::RenderPrimitives::LINES }
                                    .renderBin( bin )
                                    .vertexBuffer( "in_vertex", mSystem.makeBuffer( mGridPositions ) )
                                    .vertexBuffer( "in_normal", mSystem.makeBuffer( mGridPositions ) )
                                    .indexBuffer( mSystem.makeIndexBuffer( mGridLines ) )
                                    .uniform( "u_lineWidth", mLineWidth )
                                    .texture( "colormap", texture )
                                    .boundingSphere( bs ),
                                mSystem.makeProgramFromFiles(
                                    mResPath + "shader/line/phong/multiColorPerCell/vertex.glsl",
                                    mResPath + "shader/line/phong/multiColorPerCell/fragment.glsl",
                                    mResPath + "shader/line/phong/multiColorPerCell/geometry.glsl" ) );

                            drawables.push_back( drawableLines );
                        }

                        /*if( !mGridPoints.empty() )
                        {
                            auto drawablePoints = mSystem.makePrimitive(
                                graphics::PrimitiveConfig{ graphics::RenderPrimitives::POINTS }
                                    .renderBin( bin )
                                    .vertexBuffer( "position", mSystem.makeBuffer( mGridPositions ) )
                                    .vertexBuffer( "normal", mSystem.makeBuffer( mGridPositions ) )
                                    .indexBuffer( mSystem.makeIndexBuffer( mGridPoints ) )
                                    .texture( "colormap", texture )
                                    .boundingSphere( bs ),
                                mSystem.makeProgramFromFiles( mResPath +
                        "shader/surface/phong/multiColorPerCell/vertex.glsl",
                                                              mResPath +
                        "shader/surface/phong/multiColorPerCell/fragment.glsl" ) );

                        drawables.push_back( drawablePoints );
                        }*/
                    }
                }

                return graphics::makeCompound( drawables );
            }

            void setIsPointBased( bool isPointBased = true )
            {
                mIsPointBased = isPointBased;
            }

            bool isPointBased() const
            {
                return mIsPointBased;
            }

            const std::vector< unsigned int >& getGridIndices()
            {
                return mGridIndices;
            }

            const std::vector< unsigned int >& getCellIndices()
            {
                return mCellIndices;
            }

            std::vector< PointF< 3 > >& getGridPositions()
            {
                return mGridPositions;
            }

            size_t getNumCells()
            {
                return mNumCells;
            }

        private:
            std::vector< PointF< 3 > > mGridPositions;
            std::vector< VectorF< 3 > > mSurfaceNormals;

            std::vector< unsigned int > mGridTriangles;
            std::vector< unsigned int > mGridLines;
            std::vector< unsigned int > mGridPoints;

            std::vector< Color > mColors;

            // these can be accessed before rendering to calculate point based attributes
            std::vector< unsigned int > mGridIndices;
            std::vector< unsigned int > mCellIndices;
            std::vector< unsigned int > mQuadIndices;


            bool mIsPointBased;
            bool mIs2D;
            ShadingType mShadingType;
            float mLineWidth;
            size_t mNumCells;
            size_t mNumQuads;
            std::string mResPath;
            const fantom::graphics::GraphicsSystem& mSystem;
        };
    } // namespace graphics

} // namespace fantom
