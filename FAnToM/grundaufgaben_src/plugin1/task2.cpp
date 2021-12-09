#include <vector>
//#include <stdexcept>

//#include <fantom/datastructures/interfaces/Field.hpp>
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
    class GraphicsTutorialAlgorithm : public VisAlgorithm
    {
 
    public:
        struct Options : public VisAlgorithm::Options
        {
            Options( fantom::Options::Control& control )
                : VisAlgorithm::Options( control )
            {
                add< Grid <3> >( "grid", "A 3D vector field");//, definedOn< Grid< 3 > >(Grid< 3 >::Points) );
                add< Color >( "color", "The color of the graphics.", Color( 0.9, 0.7, 0.3 ) );
                add< int >( "Index", "Index of Cell if Show Single Cell is selected.", 0 );
                add< bool >( "Show Single Cell", "Show only Cell selected above.", false );
                add< bool >( "Show surface", "Show surface of full grid.", true );
                add< bool >( "Show grid", "Show full grid.", false);
            }
        };
        struct VisOutputs : public VisAlgorithm::VisOutputs
        {
            // These are the graphic outputs which can be toggled on and off in the interface.
            VisOutputs( fantom::VisOutputs::Control& control )
                : VisAlgorithm::VisOutputs( control )
            {
                addGraphics( "graphics" );
            }
        };

        GraphicsTutorialAlgorithm( InitData& data )
            : VisAlgorithm( data )
        {
        }
 
        virtual void execute( const Algorithm::Options& options, const volatile bool& /*abortFlag*/ ) override
        {
         // The GraphicsSystem is needed to create Drawables, which represent the to be rendererd objects.
            auto const& system = graphics::GraphicsSystem::instance();
 
            // // This is the path to the general resources folder, where you can find the shaders.
            std::string resourcePath = PluginRegistrationService::getInstance().getResourcePath( "utils/Graphics" );
 
            // // It is also possible to address a "resources" folder within a plugin.
            // // This method is used if the shaders are plugin specific and not used otherwise;
            // std::string resourcePathLocal
            //     = PluginRegistrationService::getInstance().getResourcePath( "general/Tutorial" );

            // //get house grid
            std::shared_ptr< const Grid< 3 > > grid = options.get< const Grid < 3 > >("grid");
            Color color = options.get<Color>("color");
            const ValueArray< Point3 >& pointsIn = grid->points();
            std::vector< PointF< 3 > > pointsOut = {};
            std::vector< unsigned int > indexes = {};
            int offset = 0;
            std::vector<size_t> pConnect = {0,1,2, 0,2,3, 0,1,6, 
                                    0,6,7, 0,3,4, 0,4,7, 
                                    7,4,6, 6,4,5, 3,4,5, 
                                    3,2,5, 1,2,5, 1,6,5};
            
            for(size_t i = 0; i < grid->numCells(); i++){
                Cell cell = grid->cell(i);
                if (cell.type() == Cell::Type::HEXAHEDRON) { 
                    for(size_t j = 0; j < pConnect.size(); j++){
                        pointsOut.push_back(PointF< 3 > (cell.index(pConnect[j])));
                        indexes.push_back(j + offset);
                    }
                    offset += sizeof(pConnect);
                }
                // case Cell::Type::PYRAMID: 
                //     size_t pConnect[] = {0,1,2, 0,2,3, 0,1,4,
                //                          1,2,4, 2,3,4, 3,0,4};
                //     break;
                // case Cell::Type::LINE: break;
                // default: std::cout << "Did not recognize cell type" << std::endl;
                // }
            }

            // The BoundingSphere should contain all elements of the drawable and is needed for its creation.
            auto bs = graphics::computeBoundingSphere(pointsOut);
 
            // For the used Phong-Shading, the calculation of surface normals is necessary.
            auto norm = graphics::computeNormals(pointsOut, indexes);
 
            std::shared_ptr< graphics::Drawable > simpleDrawable = system.makePrimitive(
                graphics::PrimitiveConfig{ graphics::RenderPrimitives::TRIANGLES }
                    .vertexBuffer( "position", system.makeBuffer( pointsOut ) )
                    .vertexBuffer( "normal", system.makeBuffer( norm ) )
                    .indexBuffer( system.makeIndexBuffer( indexes ) )
                    .uniform( "color", color )
                    //.renderOption( graphics::RenderOption::LineSmooth, true )
                    .boundingSphere( bs ),
                system.makeProgramFromFiles(resourcePath + "shader/surface/phong/singleColor/vertex.glsl",
                                            resourcePath + "shader/surface/phong/singleColor/fragment.glsl"));
            //setGraphics( "outGrid", simpleDrawable );
        }
    };
 
    AlgorithmRegister< GraphicsTutorialAlgorithm > dummy( "Tasks/Task2", "Show some example graphics." );
} // namespace
