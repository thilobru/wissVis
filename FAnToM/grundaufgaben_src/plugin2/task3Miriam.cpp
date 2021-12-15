// #include <fantom/algorithm.hpp>
// #include <fantom/datastructures/interfaces/Field.hpp>
// #include <fantom/graphics.hpp>
// #include <fantom/register.hpp>
// #include <math.h>
// #include <stdlib.h>

// // needed for BoundinSphere-Calculation
// #include <fantom-plugins/utils/Graphics/HelperFunctions.hpp>
// #include <fantom-plugins/utils/Graphics/ObjectRenderer.hpp>

// #include <stdexcept>
// #include <vector>

// using namespace fantom;

// namespace {

//     class PositionFeatureAlgorithm : public VisAlgorithm {

//     public:
//         struct Options : public VisAlgorithm::Options {
//             Options(fantom::Options::Control &control)
//                     : VisAlgorithm::Options(control) {
//                 add< Field< 3, Scalar > >( "Field", "A 3D vector field");
//                 add<Color>("Color", "The color of the graphics.", Color(0.75, 0.75, 0.0));
//                 add<double>("Threshold", "The minimum value of each point.", 0.0008);
//             }
//         };

//         struct VisOutputs : public VisAlgorithm::VisOutputs {
//             VisOutputs(fantom::VisOutputs::Control &control)
//                     : VisAlgorithm::VisOutputs(control) {
//                 addGraphics( "Spheres" );
//             }
//         };


//         PositionFeatureAlgorithm(InitData &data)
//                 : VisAlgorithm(data) {
//         }

//         virtual void execute(const Algorithm::Options &options, const volatile bool& /*abortFlag*/ ) override 
//         {
//             // The GraphicsSystem is needed to create Drawables, which represent the to be rendererd objects.
//             auto const& system = graphics::GraphicsSystem::instance();
 
//             // This is the path to the general resources folder, where you can find the shaders.
//             std::string resourcePath = PluginRegistrationService::getInstance().getResourcePath( "utils/Graphics" );

//             // object renderer will contain all the spheres
//             auto performanceObjectRenderer = std::make_shared< graphics::ObjectRenderer >( system );

//             //--------------------------------------------------------------------------------------------------------------------

//             // get options and print in log
//             std::shared_ptr< const Field< 3, Scalar > > field = options.get< Field< 3, Scalar > >( "Field" );
//             std::shared_ptr< const ScalarFunction > function = options.get< ScalarFunction >( "Field" );

//             Color color = options.get< Color >( "Color" );
//             double threshold = options.get< double >( "Threshold" );
//             infoLog() << "Threshold:  " << threshold << std::endl;

//             // if there is no input, do nothing
//             if (!field) {
//                 debugLog() << "Input Field not set." << std::endl;
//                 return;
//             }
            
//             // sanity check that interpolated fields really use the correct grid type. This should never fail
//             std::shared_ptr< const Grid< 3 > > grid = std::dynamic_pointer_cast< const Grid< 3 > >( function->domain() );
            
//             if( !grid )
//             {
//                 throw std::logic_error( "Wrong type of grid!" );
//             }
            
//             //--------------------------------------------------------------------------------------------------------------------

//             //get the points and indices from the input field
//             const ValueArray< Point3 >& points = grid->points();

//             auto evaluator = field->makeEvaluator();

//             //for each cell in the grid
//             for( size_t i = 0; i < points.size(); ++i )
//                 {
//                     auto point = points[i];

//                     if ( evaluator->reset(point) )
//                     {
//                         // get value v at this point
//                         auto v = evaluator->value();

//                         // add Sphere to Renderer if value bigger than threshold
//                         if (v[0] > threshold)
//                         {
//                             performanceObjectRenderer->addSphere( point, 0.25, color );
//                         }
//                     }
//                 }

//             setGraphics( "Spheres", performanceObjectRenderer->commit() );

//         }
//     };
    
//     AlgorithmRegister< PositionFeatureAlgorithm > dummy( "Aufgaben/Show Positionfeature", "Show only points of a dataset bigger than a minimum");
// }