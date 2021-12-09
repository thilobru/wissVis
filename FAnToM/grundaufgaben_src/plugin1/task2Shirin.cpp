// //#include <fantom/datastructures/interfaces/Field.hpp>
// #include <fantom/dataset.hpp>
 

// #include <fantom/algorithm.hpp>
// #include <fantom/graphics.hpp>
// #include <fantom/register.hpp>
// //#include <math.h>
 
// // needed for BoundinSphere-Calculation and normal calculation
// #include <fantom-plugins/utils/Graphics/HelperFunctions.hpp>
// #include <fantom-plugins/utils/Graphics/ObjectRenderer.hpp>
 
// //#include <stdexcept>
// //#include <vector>

// using namespace fantom;
 
// namespace
// {
 
//     class GraphicsTutorialAlgorithm : public VisAlgorithm
//     {
 
//     public:
//         struct Options : public VisAlgorithm::Options
//         {
//             Options( fantom::Options::Control& control )
//                 : VisAlgorithm::Options( control )
//             {
//                 addBundle< Grid <3> >( "Grid", "A 3D vector field", definedOn< Grid< 3 > >(Grid< 3 >::Points) );
//                 //add< Grid <3> >( "Grid", "A 3D vector field");//, definedOn< Grid< 3 > >(Grid< 3 >::Points) );
//                 add< Color >( "Color", "The color of the graphics.", Color( 0.9, 0.7, 0.3 ) );
//                 add< int >( "Indec Single Cell", "Index of Cell if Show Single Cell is selected.", 0 );
//                 add< bool >( "Show Single Cell", "Show only Cell selected above.", false );
//                 add< bool >( "Show surface", "Show surface of full grid.", true );
//                 add< bool >( "Show grid", "Show full grid.", false);
//             }
//         };
// // addBundle< const Function< Vector3 > >( "velocity fields" ); // test: add "velocity" as a bundle
// // addBundle< const Grid< 3 > >( "grids" );
//         struct VisOutputs : public VisAlgorithm::VisOutputs
//         {
//             // These are the graphic outputs which can be toggled on and off in the interface.
//             VisOutputs( fantom::VisOutputs::Control& control )
//                 : VisAlgorithm::VisOutputs( control )
//             {
//                 addGraphics( "ownGrid" );
//                 // addGraphics( "y" );
//                 // addGraphics( "z" );
//                 // addGraphics( "ellipsoid" );
//                 // addGraphics( "manySpheres" );
//                 // addGraphics( "text" );
//                 // addGraphics( "simpleDrawable" );
//                 // addGraphics( "simpleDrawable2" );
//                 // addGraphics( "geometryDrawable" );
//                 // addGraphics( "textureDrawable" );
//             }
//         };
 
//         GraphicsTutorialAlgorithm( InitData& data )
//             : VisAlgorithm( data )
//         {
//         }
 
//         virtual void execute( const Algorithm::Options& options, const volatile bool& /*abortFlag*/ ) override
//         {
//          // The GraphicsSystem is needed to create Drawables, which represent the to be rendererd objects.
//             auto const& system = graphics::GraphicsSystem::instance();
 
//             // This is the path to the general resources folder, where you can find the shaders.
//             std::string resourcePath = PluginRegistrationService::getInstance().getResourcePath( "utils/Graphics" );
 
//             // It is also possible to address a "resources" folder within a plugin.
//             // This method is used if the shaders are plugin specific and not used otherwise;
//             std::string resourcePathLocal
//                 = PluginRegistrationService::getInstance().getResourcePath( "general/Tutorial" );

//             //get house grid
//             std::shared_ptr< const DataObjectBundle > bundle = options.get< DataObjectBundle >("Bundle");
//             std::shared_ptr< const Grid< 3 > > grid = bundle.getContent(0);
//             debugLog() << "Grid Point Count: " << grid->numPoints() << std::endl;
            
//             //get chosen color
//             Color col = options.get<Color>("Color");

//             const ValueArray< Point3 >& points = grid->points();
//             for(size_t i =0; i<points.size(); i++){
//                 debugLog() << i << "; Point: " << points[i] << std::endl;
//             }
            
//             std::vector< PointF< 3 > > points_for_drawing = {};
//             std::vector< unsigned int > indices = {};
//             int index_count = 0;


//             for( size_t i = 0; i < grid->numCells(); ++i )
//                 {
//                     Cell cell = grid->cell( i );
//                     debugLog() << "Zelle: " << i << " " << cell.type() << std::endl;
//                     Point3 point;
                    

//                     if(cell.type() == Cell::Type::HEXAHEDRON){
                        
//                         //ToDo: verbessern/vereinfachen -> Punkt nur zufügen wenn noch nicht vorhanden und über index steuern welcher Punkt für dreieck benutzt wird
//                         const int SIZE = 36;
//                         //folgende dreiecke müssen erstellt werden um ein hexahedron zu zeichnen
//                         size_t reihenfolge[SIZE] = {0,1,2, 0,2,3, 0,1,6, 0,6,7, 0,3,4, 0,4,7, 7,4,6, 6,4,5, 3,4,5, 3,2,5, 1,2,5, 1,6,5};

                    
//                         for(int j = 0; j != SIZE; ++j){
//                             point = points[cell.index( reihenfolge[j] )];
//                             points_for_drawing.push_back(PointF< 3 >( point[0], point[1], point[2] ));
//                             indices.push_back(j + index_count);
//                         }

//                         index_count += SIZE;
//                     }

//                     else if(cell.type() == Cell::Type::PYRAMID){
                        
//                         //ToDo: verbessern/vereinfachen -> Punkt nur zufügen wenn noch nicht vorhanden und über index steuern welcher Punkt für dreieck benutzt wird
//                         const int SIZE = 18;
//                         size_t reihenfolge[SIZE] = {0,1,2 ,0,2,3, 0,1,4, 1,2,4, 2,3,4, 3,0,4};
                      
//                         for(int j = 0; j != SIZE; ++j){
//                             point = points[cell.index( reihenfolge[j] )];
//                             points_for_drawing.push_back(PointF< 3 >( point[0], point[1], point[2] ));
//                             indices.push_back(j + index_count);
//                         }

//                         index_count += SIZE;
//                     }

//                     else if(cell.type() == Cell::Type::LINE){
//                         //alles sehr uncool :( -> könnte linie erstellen wie bei gerüst, aber wie wird das dann der Grid Fasade zugeteilt????
//                         // man könnte auch eine linie aus zwei dreiecken machen ...
//                         const int SIZE = 3;
//                         size_t reihenfolge[SIZE] = {1,1,0};

//                         Point3 point1 = points[cell.index( reihenfolge[0] )];
//                         Point3 point3 = points[cell.index( reihenfolge[2] )];

//                         points_for_drawing.push_back(PointF< 3 >( point1[0], point1[1], point1[2] ));
//                         points_for_drawing.push_back(PointF< 3 >( point1[0]+0.01, point1[1], point1[2]));
//                         points_for_drawing.push_back(PointF< 3 >( point3[0], point3[1], point3[2] ));
//                         indices.push_back(0 + index_count);
//                         indices.push_back(1 + index_count);
//                         indices.push_back(2 + index_count);
                      
//                         index_count += SIZE;
//                     }
//                     else if(cell.type() == Cell::Type::QUAD){
                        
//                         //alles sehr uncool :( -> könnte linie erstellen wie bei gerüst, aber wie wird das dann der Grid Fasade zugeteilt????
//                         // man könnte auch eine linie aus zwei dreiecken machen ...
//                         const int SIZE = 6;
//                         size_t reihenfolge[SIZE] = {1,2,3, 1,3,0};

//                         for(int j = 0; j != SIZE; ++j){
//                             point = points[cell.index( reihenfolge[j] )];
//                             points_for_drawing.push_back(PointF< 3 >( point[0], point[1], point[2] ));
//                             indices.push_back(j + index_count);
//                         }
//                         index_count += SIZE;
//                     }

//                 }
 
//             // The BoundingSphere should contain all elements of the drawable and is needed for its creation.
//             auto bs = graphics::computeBoundingSphere( points_for_drawing );
 
//             // For the used Phong-Shading, the calculation of surface normals is necessary.
//             auto norm = graphics::computeNormals( points_for_drawing, indices );
 
//             // The Drawable object defines the input streams for the shaders.
//             // Vertex- and IndexBuffers as well as Uniforms can be defined as seen below.
//             std::shared_ptr< graphics::Drawable > simpleDrawable = system.makePrimitive(
//                 graphics::PrimitiveConfig{ graphics::RenderPrimitives::TRIANGLES }
//                     .vertexBuffer( "position", system.makeBuffer( points_for_drawing ) )
//                     .vertexBuffer( "normal", system.makeBuffer( norm ) )
//                     .indexBuffer( system.makeIndexBuffer( indices ) )
//                     .uniform( "color", col )
//                     //.renderOption( graphics::RenderOption::LineSmooth, true )
//                     .boundingSphere( bs ),
//                 system.makeProgramFromFiles( resourcePath + "shader/surface/phong/singleColor/vertex.glsl",
//                                              resourcePath + "shader/surface/phong/singleColor/fragment.glsl" ) );
 
//             setGraphics( "ownGrid", simpleDrawable );
 
//             // std::vector< PointF< 3 > > tri( 3 );
//             // tri[0] = PointF< 3 >( 1.5, 1.0, 0.0 );
//             // tri[1] = PointF< 3 >( 0.0, 1.0, 0.0 );
//             // tri[2] = PointF< 3 >( 1.5, 0.0, 0.0 );

 
//             // // The BoundingSphere should contain all elements of the drawable and is needed fot its creation.
//             // auto bs = graphics::computeBoundingSphere( tri );

//             // // For the used Phong-Shading, the calculation of surface normals is necessary.
//             // std::vector< unsigned int > indices = { 0, 1, 2 };
//             // auto norm = graphics::computeNormals( tri, indices );
 
//             // // The Drawable object defines the input streams for the shaders.
//             // // Vertex- and IndexBuffers as well as Uniforms can be defined as seen below.
//             // std::shared_ptr< graphics::Drawable > simpleDrawable = system.makePrimitive(
//             //     graphics::PrimitiveConfig{ graphics::RenderPrimitives::TRIANGLES }
//             //         .vertexBuffer( "position", system.makeBuffer( tri ) )
//             //         .vertexBuffer( "normal", system.makeBuffer( norm ) )
//             //         .indexBuffer( system.makeIndexBuffer( indices ) )
//             //         .uniform( "color", Color( 0.5, 0.7, 0.1 ) )
//             //         .renderOption( graphics::RenderOption::Blend, true )
//             //         .boundingSphere( bs ),
//             //     system.makeProgramFromFiles( resourcePath + "shader/surface/phong/singleColor/vertex.glsl",
//             //                                  resourcePath + "shader/surface/phong/singleColor/fragment.glsl" ) );
 
//             // setGraphics( "simpleDrawable", simpleDrawable );

//             // std::vector< PointF< 3 > > tri2( 3 );
//             // tri2[0] = PointF< 3 >( 0.0, 0.0, 0.0 );
//             // tri2[1] = PointF< 3 >( 1.5, 0.0, 0.0 );
//             // tri2[2] = PointF< 3 >( 0.0, 1.0, 0.0 );

//             // // The BoundingSphere should contain all elements of the drawable and is needed fot its creation.
//             // auto bs2 = graphics::computeBoundingSphere( tri2 );

//             //             // For the used Phong-Shading, the calculation of surface normals is necessary.
//             // std::vector< unsigned int > indices2 = { 0, 1, 2 };
//             // auto norm2 = graphics::computeNormals( tri2, indices2 );

//             // // The Drawable object defines the input streams for the shaders.
//             // // Vertex- and IndexBuffers as well as Uniforms can be defined as seen below.
//             // std::shared_ptr< graphics::Drawable > simpleDrawable2 = system.makePrimitive(
//             //     graphics::PrimitiveConfig{ graphics::RenderPrimitives::TRIANGLES }
//             //         .vertexBuffer( "position", system.makeBuffer( tri2 ) )
//             //         .vertexBuffer( "normal", system.makeBuffer( norm2 ) )
//             //         .indexBuffer( system.makeIndexBuffer( indices2 ) )
//             //         .uniform( "color", Color( 1.0, 0.5, 0.1 ) )
//             //         .renderOption( graphics::RenderOption::Blend, true )
//             //         .boundingSphere( bs2 ),
//             //     system.makeProgramFromFiles( resourcePath + "shader/surface/phong/singleColor/vertex.glsl",
//             //                                  resourcePath + "shader/surface/phong/singleColor/fragment.glsl" ) );
            
//             // setGraphics( "simpleDrawable2", simpleDrawable2 );
 
//             // //----------------------------
//             // // With vertex, fragment AND GEOMETRY shader:
//             // // Example geometry shader usage: each line is duplicated swapping x- and y-coordinates
 
//             // std::vector< PointF< 3 > > v( 2 );
//             // v[0] = PointF< 3 >( -1.5, -1.0, 0.0 );
//             // v[1] = PointF< 3 >( 1.5, 1.0, 0.0 );
 
//             // bs = graphics::computeBoundingSphere( v );
 
//             // std::shared_ptr< graphics::Drawable > geometryDrawable
//             //     = system.makePrimitive( graphics::PrimitiveConfig{ graphics::RenderPrimitives::LINES }
//             //                                 .vertexBuffer( "position", system.makeBuffer( v ) )
//             //                                 .boundingSphere( bs ),
//             //                             system.makeProgramFromFiles( resourcePathLocal + "swizzle-vertex.glsl",
//             //                                                          resourcePathLocal + "swizzle-fragment.glsl",
//             //                                                          resourcePathLocal + "swizzle-geometry.glsl" ) );
 
//             // auto testObjectRenderer = std::make_shared< graphics::ObjectRenderer >( system );
 
//             // testObjectRenderer->addTextLabel( Point3( -1.5, -1.0, 0.0 ), "-1.5, -1.0", 20, Color( 1.0, 0.0, 0.0 ) );
//             // testObjectRenderer->addTextLabel( Point3( 1.5, 1.0, 0.0 ), "1.5, 1.0", 20, Color( 1.0, 0.0, 0.0 ) );
//             // testObjectRenderer->addTextLabel( Point3( -1.0, -1.5, 0.0 ), "-1.0, -1.5", 20, Color( 0.0, 1.0, 0.0 ) );
//             // testObjectRenderer->addTextLabel( Point3( 1.0, 1.5, 0.0 ), "1.0, 1.5", 20, Color( 0.0, 1.0, 0.0 ) );
 
//             // setGraphics( "geometryDrawable",
//             //              graphics::makeCompound( { geometryDrawable, testObjectRenderer->commit() } ) );
 
 
//             //-----------------------------------------------example texture-----------------------------------------
//             // This example loads a 2D-Texture from a picture and fills a 3D-Texture with it.
//             // A plane through the 3D-Texture is then drawn in the scene.
 
//             // // Load the 2D-Texture from a picture.
//             // std::shared_ptr< graphics::Texture2D > texture2D
//             //     = system.makeTextureFromFile( resourcePathLocal + "FAnToM-logo.jpg", graphics::ColorChannel::RGBA );
 
//             // // Create an empty 3D-Texture with the same width and height as the 2D-Texture.
//             // Size3D sizeTest( texture2D->width(), texture2D->height(), 32 );
//             // std::shared_ptr< graphics::Texture3D > texture3D
//             //     = system.makeTexture( sizeTest, graphics::ColorChannel::RGBA );
 
//             // // Fill the 3D-Texture slice per slice with the color-data of the 2D-Texture.
//             // std::vector< Color > tempColors = texture2D->range( Pos2D( 0, 0 ), texture2D->size() );
//             // for( size_t k = 0; k != texture3D->depth(); ++k )
//             // {
//             //     texture3D->range( Pos3D( 0, 0, k ), Size3D( texture2D->width(), texture2D->height(), 1 ), tempColors );
//             // }
 
//             // // These are the corners of the plane that lies diagonally in the 3D-Texture.
//             // std::vector< PointF< 3 > > verticesTex( 8 );
//             // verticesTex[0] = PointF< 3 >( -0.5, -0.5, 0.5 );
//             // verticesTex[1] = PointF< 3 >( 0.5, -0.5, 0.5 );
//             // verticesTex[2] = PointF< 3 >( -0.5, 0.5, -0.5 );
//             // verticesTex[3] = PointF< 3 >( 0.5, 0.5, -0.5 );
 
//             // // These are the 3D-TextureCoordinates describing the boarders of the 3D-Texture.
//             // std::vector< PointF< 3 > > texCoords( 8 );
//             // texCoords[0] = PointF< 3 >( 0.0, 0.0, 1.0 );
//             // texCoords[1] = PointF< 3 >( 1.0, 0.0, 1.0 );
//             // texCoords[2] = PointF< 3 >( 0.0, 1.0, 0.0 );
//             // texCoords[3] = PointF< 3 >( 1.0, 1.0, 0.0 );
 
//             // // Because we cannot draw QUADS in FAnToM, we have to draw two triangles.
//             // std::vector< unsigned int > indicesTex( 6 );
//             // indicesTex[0] = 0;
//             // indicesTex[1] = 1;
//             // indicesTex[2] = 2;
//             // indicesTex[3] = 2;
//             // indicesTex[4] = 1;
//             // indicesTex[5] = 3;
 
//             // //Now we can create a Drawable with the 3D-Texture and then set it as a graphical output.
//             // bs = graphics::computeBoundingSphere( verticesTex );
//             // std::shared_ptr< graphics::Drawable > textureDrawable
//             //     = system.makePrimitive( graphics::PrimitiveConfig{ graphics::RenderPrimitives::TRIANGLES }
//             //                                 .vertexBuffer( "position", system.makeBuffer( verticesTex ) )
//             //                                 .vertexBuffer( "texCoords", system.makeBuffer( texCoords ) )
//             //                                 .indexBuffer( system.makeIndexBuffer( indicesTex ) )
//             //                                 .texture( "inTexture", texture3D )
//             //                                 .boundingSphere( bs ),
//             //                             system.makeProgramFromFiles( resourcePathLocal + "tex-vertex.glsl",
//             //                                                          resourcePathLocal + "tex-fragment.glsl" ) );
 
//             //  setGraphics( "textureDrawable", textureDrawable );
//         }
//     };
 
//     AlgorithmRegister< GraphicsTutorialAlgorithm > dummy( "Shirin/Graphics", "Show some example graphics." );
// } // namespace
