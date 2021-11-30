//#include <fantom/datastructures/interfaces/Field.hpp>
#include <fantom/dataset.hpp>
 

#include <fantom/algorithm.hpp>
#include <fantom/graphics.hpp>
#include <fantom/register.hpp>
 
// needed for BoundinSphere-Calculation and normal calculation
#include <fantom-plugins/utils/Graphics/HelperFunctions.hpp>
#include <fantom-plugins/utils/Graphics/ObjectRenderer.hpp>
 
//#include <stdexcept>
//#include <vector>

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
                add< DataObjectBundle >( "Bundle", "A Bundle");//, definedOn< Grid< 3 > >(Grid< 3 >::Points) );
            }
        };
        struct VisOutputs : public VisAlgorithm::VisOutputs
        {
            // These are the graphic outputs which can be toggled on and off in the interface.
            VisOutputs( fantom::VisOutputs::Control& control )
                : VisAlgorithm::VisOutputs( control )
            {
                addGraphics( "outGrid" );
            }
        };
 
        GraphicsTutorialAlgorithm( InitData& data )
            : VisAlgorithm( data )
        {
        }
 
        virtual void execute( const Algorithm::Options& options, const volatile bool& /*abortFlag*/ ) override
        {
         // The GraphicsSystem is needed to create Drawables, which represent the to be rendererd objects.
            // auto const& system = graphics::GraphicsSystem::instance();
 
            // // This is the path to the general resources folder, where you can find the shaders.
            // std::string resourcePath = PluginRegistrationService::getInstance().getResourcePath( "utils/Graphics" );
 
            // // It is also possible to address a "resources" folder within a plugin.
            // // This method is used if the shaders are plugin specific and not used otherwise;
            // std::string resourcePathLocal
            //     = PluginRegistrationService::getInstance().getResourcePath( "general/Tutorial" );

            // //get house grid
            // std::shared_ptr< const Grid< 3 > > grid = options.get< Grid<3> >( "Grid" );
            std::shared_ptr< const DataObjectBundle > bundle = options.get< DataObjectBundle >("Bundle");
            debugLog() << "num grids: " << bundle->isEmpty() << std::endl;
            //size_t i = bundle->getSize();
            //setGraphics( "outGrid", simpleDrawable );
        }
    };
 
    AlgorithmRegister< GraphicsTutorialAlgorithm > dummy( "Tasks/Task2", "Show some example graphics." );
} // namespace
