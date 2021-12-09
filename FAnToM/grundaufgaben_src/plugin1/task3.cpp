#include <fantom/algorithm.hpp>
#include <fantom/datastructures/interfaces/Field.hpp>
#include <fantom/graphics.hpp>
#include <fantom/register.hpp>
// needed for BoundinSphere-Calculation
#include <fantom-plugins/utils/Graphics/HelperFunctions.hpp>
#include <fantom-plugins/utils/Graphics/ObjectRenderer.hpp>


using namespace fantom;

namespace {

    class PositionTask : public VisAlgorithm {

    public:
        struct Options : public VisAlgorithm::Options {
            Options(fantom::Options::Control &control)
                    : VisAlgorithm::Options(control) {
                add< Field< 3, Scalar > >( "Field", "A 3D vector field");
                add<Color>("Color", "COlor of Kugels", Color(0.75, 0.75, 0.0));
                add<double>("Threshold", "The minimum value of each point.", 0.0008);
            }
        };

        struct VisOutputs : public VisAlgorithm::VisOutputs {
            VisOutputs(fantom::VisOutputs::Control &control)
                    : VisAlgorithm::VisOutputs(control) {
                addGraphics( "Kugels" );
            }
        };


        PositionTask(InitData &data)
                : VisAlgorithm(data) {
        }

        virtual void execute(const Algorithm::Options &options, const volatile bool& /*abortFlag*/ ) override 
        {
            // The GraphicsSystem is needed to create Drawables, which represent the to be rendererd objects.
            auto const& system = graphics::GraphicsSystem::instance();
            // This is the path to the general resources folder, where you can find the shaders.
            std::string resourcePath = PluginRegistrationService::getInstance().getResourcePath( "utils/Graphics" );
            // obj renderer to handle Kugels
            auto performanceObjectRenderer = std::make_shared< graphics::ObjectRenderer >( system );
            //--------------------------------------------------------------------------------------------------------------------

            std::shared_ptr< const Field< 3, Scalar > > field = options.get< Field< 3, Scalar > >( "Field" );
            std::shared_ptr< const ScalarFunction > function = options.get< ScalarFunction >( "Field" );

            Color color = options.get< Color >( "Color" );
            double threshold = options.get< double >( "Threshold" );

            // if there is no input, do nothing
            if (!field) {
                debugLog() << "Input Field not set." << std::endl;
                return;
            }
            
            // sanity check that interpolated fields really use the correct grid type. This should never fail
            std::shared_ptr< const Grid< 3 > > grid = std::dynamic_pointer_cast< const Grid< 3 > >( function->domain() );
            if( !grid )
            {
                throw std::logic_error( "Wrong type of grid!" );
            }
            const ValueArray < Point3> & points = grid->points();
            auto eval = field->makeEvaluator();

            for(size_t i = 0; i < points.size(); i++) {
                Point<3> point = points[i];
                if (eval->reset(point)){
                    auto v = eval->value();
                    if (v[0] > threshold) {
                        performanceObjectRenderer->addSphere(point, 0.1, color);
                    }
                }
            }
            setGraphics("Kugels", performanceObjectRenderer->commit());
        
        }
    };
    AlgorithmRegister< PositionTask > dummy( "Tasks/Task3", "Show points of VTK above threshold");
}