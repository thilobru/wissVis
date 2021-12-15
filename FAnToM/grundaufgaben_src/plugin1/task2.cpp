// #include <fantom/dataset.hpp>
// #include <fantom/algorithm.hpp>
// #include <fantom/graphics.hpp>
// #include <fantom/register.hpp>
 
// // needed for BoundinSphere-Calculation and normal calculation
// #include <fantom-plugins/utils/Graphics/HelperFunctions.hpp>
// #include <fantom-plugins/utils/Graphics/ObjectRenderer.hpp>
 
// #include <vector>

// using namespace fantom;
 
// namespace
// {
//     class GraphicsTask : public VisAlgorithm
//     {
 
//     public:
//         struct Options : public VisAlgorithm::Options
//         {
//             Options( fantom::Options::Control& control )
//                 : VisAlgorithm::Options( control )
//             {
//                 add< Grid <3> >( "grid", "A 3D vector field");
//                 add< Color >( "color", "The color of graphics", Color(1.0, 0.5, 0.5));
//                 add< int >( "index", "index of desired cell", 0 );
//                 add< bool >( "showSingle", "Show only one cell", false );
//                 add< bool >( "showSurface", "Show surface", true );
//                 add< bool >( "showGrid", "Show grid", false);
//             }
//         };
//         struct VisOutputs : public VisAlgorithm::VisOutputs
//         {
//             // These are the graphic outputs which can be toggled on and off in the interface.
//             VisOutputs( fantom::VisOutputs::Control& control )
//                 : VisAlgorithm::VisOutputs( control )
//             {
//                 addGraphics( "graphics" );
//             }
//         };

//         GraphicsTask( InitData& data )
//             : VisAlgorithm( data )
//         {
//         }

//         static void makeGrid(const std::string resourcePath, auto const& system, std::shared_ptr< const Grid< 3 > > grid, Color col, size_t cell_index, bool single){
//             //make point array
//             const ValueArray< Point3 >& points = grid->points();

//             std::vector< PointF< 3 > > pointsOut = {};
//             size_t start;
//             size_t end;
//             if(single) {
//                 start = cell_index;
//                 end = cell_index+1;
//             } else {
//                 start = 0;
//                 end = grid->numCells();
//             }
//             //looping cells
//             for(size_t i = start; i < end; i++) {
//                 Cell cell = grid->cell(i);
//                 Point3 point;
//                 switch(cell.type()) {
//                 case Cell::Type::HEXAHEDRON:{
//                     std::vector<size_t> connections = {0,1, 1,2, 2,3, 3,0,
//                                                        7,6, 6,5, 5,4, 4,7,
//                                                        0,7, 1,6, 2,5, 3,4 };
//                     for(size_t j = 0; j < connections.size(); j++){
//                         point = points[cell.index(connections[j])];
//                         pointsOut.push_back(PointF< 3 >(point[0], point[1], point[2]));
//                     }
//                     break;}
//                 case Cell::Type::PYRAMID:{
//                     std::vector<size_t> connections = {0,1, 1,2, 2,3, 3,0,
//                                                        0,4, 1,4, 2,4, 3,4};
//                     for(size_t j = 0; j < connections.size(); j++){
//                         point = points[cell.index(connections[j])];
//                         pointsOut.push_back(PointF< 3 >(point[0], point[1], point[2]));
//                     }
//                     break;}
//                 case Cell::Type::LINE:{
//                     std::vector<size_t> connections = {0,1};
//                     for(size_t j = 0; j < connections.size(); j++){
//                         point = points[cell.index(connections[j])];
//                         pointsOut.push_back(PointF< 3 >(point[0], point[1], point[2]));
//                     }
//                     break;
//                     }
//                 default: printf("error! unknown cell type");
//                 break;
//                 }
//             }
//             drawLines(resourcePath, system, pointsOut);
//         }

//         static void makeSurface(const std::string resourcePath, auto const& system, std::shared_ptr<const Grid< 3 >> grid, Color color, size_t cell_index, bool single){
//             //make point array
//             const ValueArray< Point3 >& points = grid->points();

//             int index = 0;
//             int index_line = 0;
//             std::vector< PointF< 3 > > surfPointsOut;
//             std::vector< unsigned int > indexes;
//             std::vector< PointF< 3 > > linePointsOut;
//             std::vector< size_t > line_indexes;

//             //if show only one cell -> for loop only for that specific cell
//             size_t start;
//             size_t end;
//             if(single){
//                 start = cell_index;
//                 end = cell_index+1;
//             } else {
//                 start = 0;
//                 end = grid->numCells();
//             }

//             //looping cells
//             for(size_t i = start; i < end; i++){
//                 Cell cell = grid->cell(i);
//                 Point3 point;               
//                 switch(cell.type()) {
//                 case Cell::Type::HEXAHEDRON:{
//                     //order to create triangles for drawing hexahedrons
//                     std::vector<size_t> connections = {0,1,2, 0,2,3, 7,4,6, 6,4,5,
//                                                        0,3,4, 0,4,7, 0,1,6, 0,6,7, 
//                                                        3,4,5, 3,2,5, 1,2,5, 1,6,5};
//                     for(size_t j = 0; j < connections.size(); j++){
//                         point = points[cell.index( connections[j] )];
//                         surfPointsOut.push_back(PointF< 3 >( point[0], point[1], point[2] ));
//                         indexes.push_back(j + index);
//                     }
//                     index += connections.size();
//                     break;}
//                 case Cell::Type::PYRAMID:{
//                     std::vector<size_t> connections = {0,1,2, 0,2,3, 
//                                                        0,1,4, 1,2,4, 2,3,4, 3,0,4};
//                     for(size_t j = 0; j < connections.size(); j++){
//                         point = points[cell.index( connections[j] )];
//                         surfPointsOut.push_back(PointF< 3 >( point[0], point[1], point[2] ));
//                         indexes.push_back(j + index);
//                     }
//                     index += connections.size();
//                     break;}
//                 case Cell::Type::LINE:{
//                     std::vector<size_t> connections = {0,1};
//                     for(size_t j=0; j < connections.size(); j++){
//                         point = points[cell.index( connections[j] )];
//                         linePointsOut.push_back(PointF< 3 >( point[0], point[1], point[2] ));
//                         line_indexes.push_back(j + index_line);
//                     }                    
//                     index_line += connections.size();
//                     break;}
//                 default: printf("unknown cell type error");
//                 break;
//                 }
//             }
//             drawLines(resourcePath, system, linePointsOut);
//             drawSurfaces(resourcePath, system, surfPointsOut, indexes, color);
//         }

//         static void drawLines(const std::string resourcePath, 
//                               auto const& system,
//                               std::vector<PointF< 3 >>& pointsOut) {
//             //make bounding sphere around points
//             auto bs = graphics::computeBoundingSphere(pointsOut);
 
//             std::shared_ptr< graphics::Drawable > lineDrawable
//                 = system.makePrimitive( graphics::PrimitiveConfig{ graphics::RenderPrimitives::LINES }
//                                             .vertexBuffer( "position", system.makeBuffer(pointsOut) )
//                                             .boundingSphere( bs ),
//                                             system.makeProgramFromFiles(resourcePath + "shader/line/noShading/multiColor/fragment.glsl",
//                                                                         resourcePath + "shader/line/noShading/multiColor/geometry.glsl",
//                                                                         resourcePath + "shader/line/noShading/multiColor/vertex.glsl" ) );
//                                             // system.makeProgramFromFiles( resourcePathLocal + "editvertex.glsl",
//                                             //                              resourcePathLocal + "editfragment.glsl",
//                                             //                              resourcePathLocal + "editgeometry.glsl" ) );
//             setGraphics("graphics", lineDrawable);
//         }

//         static void drawSurfaces(const std::string resourcePath, 
//                                  auto const& system,
//                                  std::vector<PointF< 3 >>& pointsOut,
//                                  std::vector<unsigned int>& indexes,
//                                  Color color) {
//             // The BoundingSphere should contain all elements of the drawable and is needed for its creation.
//             auto bs = graphics::computeBoundingSphere( pointsOut );
//             // For the used Phong-Shading, the calculation of surface normals is necessary.
//             auto norm = graphics::computeNormals( pointsOut, indexes );
//             // The Drawable object defines the input streams for the shaders.
//             // Vertex- and IndexBuffers as well as Uniforms can be defined as seen below.
//             std::shared_ptr< graphics::Drawable > surfDrawable = system.makePrimitive(
//                 graphics::PrimitiveConfig{ graphics::RenderPrimitives::TRIANGLES }
//                     .vertexBuffer( "position", system.makeBuffer( pointsOut ) )
//                     .vertexBuffer( "normal", system.makeBuffer( norm ) )
//                     .indexBuffer( system.makeIndexBuffer( indexes ) )
//                     .uniform( "color", color )
//                     .boundingSphere( bs ),
//                 system.makeProgramFromFiles(resourcePath + "shader/surface/phong/singleColor/vertex.glsl",
//                                             resourcePath + "shader/surface/phong/singleColor/fragment.glsl"));
//             setGraphics("graphics", surfDrawable);
//         }

//         virtual void execute( const Algorithm::Options& options, const volatile bool& /*abortFlag*/ ) override {
//             // The GraphicsSystem is needed to create Drawables, which represent the to be rendererd objects.
//             auto const& system = graphics::GraphicsSystem::instance();
//             // // This is the path to the general resources folder, where you can find the shaders.
//             std::string resourcePath = PluginRegistrationService::getInstance().getResourcePath( "utils/Graphics" );
//             // // It is also possible to address a "resources" folder within a plugin.
//             // // This method is used if the shaders are plugin specific and not used otherwise;
//             // std::string resourcePathLocal
//             //     = PluginRegistrationService::getInstance().getResourcePath( "general/Tutorial" );

//             // get options
//             std::shared_ptr< const Grid< 3 > > grid = options.get< const Grid < 3 > >("grid");
//             Color color = options.get<Color>("color");
//             size_t cell_index = options.get<size_t>("index");
//             bool showSingle = options.get<bool>("showSingle");
//             bool show_surface = options.get<bool>("showSurface");
//             bool showGrid = options.get<bool>("showGrid");

//             if(show_surface){
//                 makeSurface(resourcePath, system, grid, color, cell_index, showSingle);
//             }
//             if (showGrid) {
//                 makeGrid(resourcePath, system, grid, color, cell_index, showSingle);
//             }
//         }
//     };

//     AlgorithmRegister< GraphicsTask > dummy( "Tasks/Task2", "Show Houses as grids and surfaces" );
// }
