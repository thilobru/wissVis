#include <fantom/dataset.hpp> 
#include <fantom/algorithm.hpp>
#include <fantom/graphics.hpp>
#include <fantom/register.hpp>

// needed for BoundinSphere-Calculation and normal calculation
#include <fantom-plugins/utils/Graphics/HelperFunctions.hpp>
#include <fantom-plugins/utils/Graphics/ObjectRenderer.hpp>

using namespace fantom;
 
namespace
{
 
    class Graphics : public VisAlgorithm
    {
 
    public:
        struct Options : public VisAlgorithm::Options
        {
            Options( fantom::Options::Control& control )
                : VisAlgorithm::Options( control )
            {
                add< Grid <3> >( "Grid", "A 3D vector field");//, definedOn< Grid< 3 > >(Grid< 3 >::Points) );
                add< Color >( "Color", "The color of the graphics.", Color( 0.9, 0.7, 0.3 ) );
                add< size_t >( "Index Single Cell", "Index of Cell if Show Single Cell is selected.", 0 );
                add< bool >( "Show Single Cell", "Show only Cell selected above.", false );
                add< bool >( "Show surface", "Show surface of full grid.", true );
            }
        };
        struct VisOutputs : public VisAlgorithm::VisOutputs
        {
            // These are the graphic outputs which can be toggled on and off in the interface.
            VisOutputs( fantom::VisOutputs::Control& control)
                : VisAlgorithm::VisOutputs( control)
            {              
                addGraphics( "ownGrid" );
            }
        };
 
        Graphics( InitData& data )
            : VisAlgorithm( data )
        {
        }


        virtual void execute( const Algorithm::Options& options, const volatile bool& /*abortFlag*/ ) override
        {
            // The GraphicsSystem is needed to create Drawables, which represent the to be rendererd objects.
            auto const& system = graphics::GraphicsSystem::instance();
            // This is the path to the general resources folder, where you can find the shaders.
            std::string resourcePath = PluginRegistrationService::getInstance().getResourcePath( "utils/Graphics" );
            // This method is used if the shaders are plugin specific and not used otherwise;
            std::string resourcePathLocal 
                = PluginRegistrationService::getInstance().getResourcePath( "general/Tutorial" );
            
            //get house grid from Input / previous DataAlgorithm
            std::shared_ptr< const Grid< 3 > > grid = options.get< Grid<3> >( "Grid" );
            //get chosen color, cell index (if given), if show single cell or whole grid, if surface or grid
            Color col = options.get<Color>("Color");
            size_t cell_index = options.get<size_t>("Index Single Cell");
            bool show_single_cell = options.get<bool>("Show Single Cell");
            bool show_surface = options.get<bool>("Show surface");
            
            if(show_surface){
                showSurf(resourcePath, system, grid, col, cell_index, show_single_cell);
            } else{
                showGrid(resourcePathLocal, system, grid, col, cell_index, show_single_cell);
            }
        }

        void showGrid(const std::string resourcePathLocal, auto const& system, std::shared_ptr< const Grid< 3 > > grid, Color col, size_t cell_index, bool single){
            //get points from grid
            const ValueArray< Point3 >& points = grid->points();
            //create variables
            std::vector< PointF< 3 > > points_for_drawing = {};
            size_t start;
            size_t end;
            if(single){
                start = cell_index;
                end = cell_index+1;
            } else{
                start = 0;
                end = grid->numCells();
            }
            //loop through all cells
            for( size_t i = start; i < end; ++i ){
                Cell cell = grid->cell( i );
                Point3 point;
                
                if(cell.type() == Cell::Type::HEXAHEDRON){                    
                    const int SIZE = 24;
                    size_t reihenfolge[SIZE] = { 0,1,1,2,2,3,3,0,0,7,7,6,6,1,6,5,5,2,5,4,4,3,4,7 };
                    
                    for(int j = 0; j != SIZE; ++j){
                        point = points[cell.index( reihenfolge[j] )];
                        points_for_drawing.push_back(PointF< 3 >( point[0], point[1], point[2] ));
                    }

                }

                else if(cell.type() == Cell::Type::PYRAMID){
                    const int SIZE = 16;
                    size_t reihenfolge[SIZE] = { 0,1,1,2,2,3,3,0,0,4,1,4,2,4,3,4 };
                    for(int j = 0; j != SIZE; ++j){
                        point = points[cell.index( reihenfolge[j] )];
                        points_for_drawing.push_back(PointF< 3 >( point[0], point[1], point[2] ));
                       }
                }

                else if(cell.type() == Cell::Type::LINE){
                    const int SIZE = 2;
                    size_t reihenfolge[SIZE] = {0, 1};
                    for(int j = 0; j != SIZE; ++j){
                        point = points[cell.index( reihenfolge[j] )];
                        points_for_drawing.push_back(PointF< 3 >( point[0], point[1], point[2] ));
                    }
                }
                else if(cell.type() == Cell::Type::QUAD){
                    const int SIZE = 8;
                    size_t reihenfolge[SIZE] = {1,2,2,3,3,0,0,1};

                    for(int j = 0; j != SIZE; ++j){
                        point = points[cell.index( reihenfolge[j] )];
                        points_for_drawing.push_back(PointF< 3 >( point[0], point[1], point[2] ));
                    }
                }
            }
 
            // The BoundingSphere should contain all elements of the drawable and is needed for its creation.
            auto bs = graphics::computeBoundingSphere( points_for_drawing );
 
            std::shared_ptr< graphics::Drawable > gridDrawable
                = system.makePrimitive( graphics::PrimitiveConfig{ graphics::RenderPrimitives::LINES }
                                            .vertexBuffer( "position", system.makeBuffer( points_for_drawing ) )
                                            .boundingSphere( bs ),
                                            // system.makeProgramFromFiles(resourcePath + "shader/line/noShading/multiColor/fragment.glsl",
                                            //                             resourcePath + "shader/line/noShading/multiColor/geometry.glsl",
                                            //                             resourcePath + "shader/line/noShading/multiColor/vertex.glsl" ) );
                                            system.makeProgramFromFiles( resourcePathLocal + "swizzle-vertex.glsl",
                                                                         resourcePathLocal + "swizzle-fragment.glsl",
                                                                         resourcePathLocal + "swizzle-geometry.glsl" ) );

         
 
            auto testObjectRenderer = std::make_shared< graphics::ObjectRenderer >( system );
 
            setGraphics( "ownGrid" , gridDrawable);
        }

        void showSurf(const std::string resourcePath, auto const& system, std::shared_ptr< const Grid< 3 > > grid, Color col, size_t cell_index, bool single){
            //get points from grid
            const ValueArray< Point3 >& points = grid->points();
            int index_count = 0;
            int index_count_line = 0;
            //if show only one cell -> for loop only for that specific cell
            size_t start;
            size_t end;
            if(single){
                start = cell_index;
                end = cell_index+1;
            } else{
                start = 0;
                end = grid->numCells();
            }
            //create variables
            std::vector< PointF< 3 > > all_points = {};
            std::vector< unsigned int > indices = {};
            std::vector< PointF< 3 > > line_points = {};
            std::vector< unsigned int > line_indices = {};

            //loop through all cells
            for( size_t i = start; i < end; ++i ){
                Cell cell = grid->cell( i );
                Point3 point;               
                
                if(cell.type() == Cell::Type::HEXAHEDRON){     
                    const int SIZE = 36;
                    //order to create triangles for drawing hexahedrons
                    size_t reihenfolge[SIZE] = {0,1,2, 0,2,3, 0,1,6, 0,6,7, 0,3,4, 0,4,7, 7,4,6, 6,4,5, 3,4,5, 3,2,5, 1,2,5, 1,6,5};
                    for(int j = 0; j != SIZE; ++j){
                        point = points[cell.index( reihenfolge[j] )];
                        all_points.push_back(PointF< 3 >( point[0], point[1], point[2] ));
                        indices.push_back(j + index_count);
                    }
                    index_count += SIZE;
                }
                
                else if(cell.type() == Cell::Type::PYRAMID){
                    const int SIZE = 18;
                    size_t reihenfolge[SIZE] = {0,1,2 ,0,2,3, 0,1,4, 1,2,4, 2,3,4, 3,0,4};
                    for(int j = 0; j != SIZE; ++j){
                        point = points[cell.index( reihenfolge[j] )];
                        all_points.push_back(PointF< 3 >( point[0], point[1], point[2] ));
                        indices.push_back(j + index_count);
                    }
                    index_count += SIZE;
                }

                else if(cell.type() == Cell::Type::LINE){
                    const int SIZE = 2;
                    size_t reihenfolge[SIZE] = {1,0};
                    for(int j=0; j!=SIZE; ++j){
                        point = points[cell.index( reihenfolge[j] )];
                        line_points.push_back(PointF< 3 >( point[0], point[1], point[2] ));
                        line_indices.push_back(j + index_count_line);
                    }                    
                    index_count_line += SIZE;
                }
                else if(cell.type() == Cell::Type::QUAD){
                    const int SIZE = 6;
                    size_t reihenfolge[SIZE] = {1,2,3, 1,3,0};
                    for(int j = 0; j != SIZE; ++j){
                        point = points[cell.index( reihenfolge[j] )];
                        all_points.push_back(PointF< 3 >( point[0], point[1], point[2] ));
                        indices.push_back(j + index_count);
                    }
                    index_count += SIZE;
                }

            }
            // The BoundingSphere should contain all elements of the drawable and is needed for its creation.
            auto bs_line = graphics::computeBoundingSphere( line_points );
            // For the used Phong-Shading, the calculation of surface normals is necessary.
            auto norm_line = graphics::computeNormals( line_points, line_indices );
            // The Drawable object defines the input streams for the shaders.
            // Vertex- and IndexBuffers as well as Uniforms can be defined as seen below.
            std::shared_ptr< graphics::Drawable > lineDrawable = system.makePrimitive(
                graphics::PrimitiveConfig{ graphics::RenderPrimitives::LINES }
                    .vertexBuffer( "position", system.makeBuffer( line_points ) )
                    .vertexBuffer( "normal", system.makeBuffer( norm_line ) )
                    .indexBuffer( system.makeIndexBuffer( line_indices ) )
                    .uniform( "color", col )
                    .renderOption( graphics::RenderOption::LineSmooth, true )
                    .boundingSphere( bs_line ),
                system.makeProgramFromFiles( resourcePath + "shader/surface/phong/singleColor/vertex.glsl",
                                            resourcePath + "shader/surface/phong/singleColor/fragment.glsl" ) );
    
            // The BoundingSphere should contain all elements of the drawable and is needed for its creation.
            auto bs = graphics::computeBoundingSphere( all_points );
            // For the used Phong-Shading, the calculation of surface normals is necessary.
            auto norm = graphics::computeNormals( all_points, indices );
            // The Drawable object defines the input streams for the shaders.
            // Vertex- and IndexBuffers as well as Uniforms can be defined as seen below.
            std::shared_ptr< graphics::Drawable > Drawable = system.makePrimitive(
                graphics::PrimitiveConfig{ graphics::RenderPrimitives::TRIANGLES }
                    .vertexBuffer( "position", system.makeBuffer( all_points ) )
                    .vertexBuffer( "normal", system.makeBuffer( norm ) )
                    .indexBuffer( system.makeIndexBuffer( indices ) )
                    .uniform( "color", col )
                    //.renderOption( graphics::RenderOption::LineSmooth, true )
                    .boundingSphere( bs ),
                system.makeProgramFromFiles( resourcePath + "shader/surface/phong/singleColor/vertex.glsl",
                                            resourcePath + "shader/surface/phong/singleColor/fragment.glsl" ) );

            setGraphics( "ownGrid",
                         graphics::makeCompound( { Drawable, lineDrawable} ) );
        }
    };
 
    AlgorithmRegister< Graphics > dummy( "Tasks/shirinT2", "Task2" );
} // namespace
