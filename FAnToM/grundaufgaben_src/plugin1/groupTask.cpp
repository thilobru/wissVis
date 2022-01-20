#include <fantom/algorithm.hpp>
#include <fantom/graphics.hpp>
#include <fantom/register.hpp>
#include <fantom/dataset.hpp>

// for BoundingSphere calculation
#include <fantom-plugins/utils/Graphics/HelperFunctions.hpp>
#include <fantom-plugins/utils/Graphics/ObjectRenderer.hpp>

#include <vector>
#include <math.h>
#include <cmath>
//#include <algorithm>

using namespace fantom;

namespace
{

    class IntegrateTask : public VisAlgorithm
    {

    public:
        struct Options : public VisAlgorithm::Options
        {
            Options(fantom::Options::Control &control)
                : VisAlgorithm::Options(control)
            {
                add< double >( "sx", "start point in x-dimension", -4.0 );
                add< double >( "sy", "start point in y-dimension", 1.0 );
                add< double >( "sz", "start point in z-dimension", 1.0 );
                addSeparator();
                add< double >( "ex", "end point in x-dimension", -4.0 );
                add< double >( "ey", "end point in y-dimension", 1.0 );
                add< double >( "ez", "end point in z-dimension", 7.0 );
                addSeparator();
                add<Field<3, Vector3>>("Field", "3D vector field", definedOn<Grid<3>>(Grid<3>::Points));
                add<InputChoices>("Method", "calculation method.", std::vector<std::string>{"Euler", "Runge-Kutta"}, "Euler");
                add<double>("dStep", "distance between steps", 0.05);
                add<double>("adStep", "for calculating new step size", 0.02);
                add<size_t>("nStep", "max number of steps", 100);
                addSeparator();
                add<Color>("colorStartLine", "The color of the start line", Color(1.0, 1.0, 0.0));
                add<Color>("colorStream", "The color of the streamlines", Color(1.0, 0.0, 0.0));
                add<Color>("colorSurface", "The color of the surface", Color(0.0, 1.0, 0.0));
            }
        };

        struct VisOutputs : public VisAlgorithm::VisOutputs
        {
            VisOutputs(fantom::VisOutputs::Control &control)
                : VisAlgorithm::VisOutputs(control)
            {
                addGraphics("startline");
                addGraphics("streamlines");
                addGraphics("surface");
            }
        };

        IntegrateTask(InitData &data)
            : VisAlgorithm(data)
        {
        }

        static float euclidDist(Point<3> p, Point<3> q) {
            return (float) sqrt(pow(p[0] - q[0], 2) 
                              + pow(p[1] - q[1], 2)
                              + pow(p[2] - q[2], 2));
        }

        static void makeTriangle(std::vector<PointF<3>> &surfacePoints, 
                                std::vector<unsigned int> &surfaceIndexes, 
                                Point<3> &p1, Point<3> &p2, Point<3> &p3) {
            PointF<3> addPoint[] = {PointF<3> (p1), 
                                    PointF<3> (p2), 
                                    PointF<3> (p3)};
            for(size_t i = 0; i < 3; i++) {
                surfacePoints.push_back(addPoint[i]);
                surfaceIndexes.push_back(surfacePoints.size()-1);
            }
            return;
        }

        static void makeStep(std::vector<Point<3>>& vec,
                             Point<3> p,
                             std::unique_ptr<FieldEvaluator<3UL, Vector3>>& evaluator) {
            stepRungeKutta(vec, p, evaluator);
        }

        static void stepRungeKutta(std::vector<Point<3>>& vec,
                                   Point<3> p,
                                   std::unique_ptr<FieldEvaluator<3UL, Vector3>>& evaluator) {
            Point<3> n = {0, 0, 0};
            std::vector<Point<3>> q = {n, n, n, n};
            double dStep = 0.1;
            if (evaluator->reset(p)) {
                auto v = evaluator->value();
                //if there is no velocity at this point stop the loop
                if (v[0] == 0 and v[1] == 0 and v[2] == 0) {
                    std::cout << "1error" << std::endl;
                    return;
                }
                q[0] = (dStep * v);
            } else {
                std::cout << "2error" << std::endl;
                return;
            }

            for(int i = 0; i < 3; i++) {
                if (evaluator->reset(p + 0.5 * q[i])){
                    auto v = evaluator->value();
                    q[i + 1] = (dStep * v);
                }
            }

            n = p + (q[0] + 2 * q[1] + 2 * q[2] + q[3]) / 6;
            std::cout << "added " << n << " by way of " << p << (q[0] + 2 * q[1] + 2 * q[2] + q[3]) / 6 << std::endl;
            vec.push_back(n);
        }

        static void addParticle(std::vector<std::vector<Point<3>>> &streamList, 
                                std::vector<std::vector<size_t>> &posFront,
                                //size_t nL,
                                Point<3> l0, Point<3> l1,
                                Point<3> r0, Point<3> r1) {
            // check if double distance on line is shorter than between lines
            if (euclidDist(l0, l1) * 2 > euclidDist(l1, r1)) return;
            
        }

        static void remParticle(){
            
        }

        static void ripRibbon(){
            
        }

        static void advanceSimpleRibbon(std::vector<std::vector<Point<3>>> &streamList,
                                std::vector<std::vector<size_t>>& posFront,
                                double& dStep,
                                double& adStep,
                                unsigned int& nStep,
                                size_t nL,
                                std::vector<PointF<3>> &surfacePoints, 
                                std::vector<unsigned int> &surfaceIndexes,
                                std::unique_ptr<FieldEvaluator<3UL, Vector3>>& evaluator) {
            float prevDiag = INFINITY;
            bool caughtUp = false;
            if (nL >= streamList.size() - 2) {return;}
            while(true) {
                // define quad to determine shortest diagonal
                Point<3> l0 = streamList[nL]  [posFront[nL][0]];
                Point<3> l1 = streamList[nL]  [posFront[nL][0]+1];
                Point<3> r0 = streamList[nL+1][posFront[nL+1][1]];
                Point<3> r1 = streamList[nL+1][posFront[nL+1][1]+1];

                std::cout << l0 << l1 << r0 << r1 << std::endl;

                // addParticle(streamList, posFront, nL, l0, l1, r0, r1);
                // remParticle();
                // ripRibbon();

                float lDiag = euclidDist(l1, r0);
                float rDiag = euclidDist(l0, r1);
                float minDiag = std::min(lDiag, rDiag);
                bool advanceOnLeft = (lDiag == minDiag);

                if(caughtUp && (advanceOnLeft || rDiag > prevDiag)) {    
                    std::cout << "Caught Up" << std::endl;
                    return;
                }
                if (advanceOnLeft) {
                    makeTriangle(surfacePoints, 
                                 surfaceIndexes, 
                                 l0, r0, l1);
                    std::cout << "Added " << streamList[nL].size() << "tes Triangle L " << nL << " immernoch < " << streamList.size() << std::endl;
                    posFront[nL][0]++;
                    if (streamList[nL].size() < nStep) {
                        makeStep(streamList[nL], l1, evaluator);
                    }
                    caughtUp = true;
                } else {
                    makeTriangle(surfacePoints, 
                                 surfaceIndexes, 
                                 l0, r0, r1);
                    std::cout << "Added " << streamList[nL].size() << "tes Triangle R " << nL << " immernoch < " << streamList.size() << std::endl;
                    posFront[nL + 1][1]++;
                    if (streamList[nL + 1].size() < nStep) {
                        makeStep(streamList[nL+1], r1, evaluator);
                    }
                    if (nL > streamList.size() - 2) {
                        std::cout << "exiting at end" << std::endl;
                        return;
                    }
                    advanceSimpleRibbon(streamList, posFront, dStep, adStep, nStep, nL + 1, surfacePoints, surfaceIndexes, evaluator);
                }
                prevDiag = minDiag;
            }
            return;
        }

        // static void advanceRibbon(std::vector<std::vector<Point<3>>> &streamList,
        //                         std::vector<std::vector<size_t>>& posFront,
        //                         double& dStep,
        //                         double& adStep,
        //                         unsigned int& nStep,
        //                         size_t nL,
        //                         std::vector<PointF<3>> &surfacePoints, 
        //                         std::vector<unsigned int> &surfaceIndexes,
        //                         std::unique_ptr<FieldEvaluator<3UL, Vector3>>& evaluator) {
        //     float prevDiag = INFINITY;
        //     bool caughtUp = false;
        //     if (nL > streamList.size() - 2) {return;}
        //     while(true) {
        //         // define quad to determine shortest diagonal
        //         Point<3> l0 = streamList[nL].rbegin()[1];
        //         Point<3> l1 = streamList[nL].rbegin()[0];
        //         Point<3> r0 = streamList[nL + 1].rbegin()[1];
        //         Point<3> r1 = streamList[nL + 1].rbegin()[0];

        //         // addParticle(streamList, posFront, nL, l0, l1, r0, r1);
        //         // remParticle();
        //         // ripRibbon();

        //         float lDiag = euclidDist(l1, r0);
        //         float rDiag = euclidDist(l0, r1);
        //         float minDiag = std::min(lDiag, rDiag);
        //         bool advanceOnLeft = (lDiag == minDiag);

        //         if(streamList[nL].size() >= nStep && posFront[nL][0] >= nStep){
        //             std::cout << "Finished" << nL << std::endl;
        //             return;
        //         }
        //         if(caughtUp && (advanceOnLeft || rDiag > prevDiag)) {    
        //             std::cout << "Caught Up" << std::endl;
        //             return;
        //         }
        //         if (advanceOnLeft) {
        //             makeTriangle(surfacePoints, 
        //                          surfaceIndexes, 
        //                          l0, r0, l1);
        //             //std::cout << "Added Triangle L" << std::endl;
        //             // makeStep(streamList[nL], l1, evaluator);
        //             caughtUp = true;
        //         } else {
        //             makeTriangle(surfacePoints, 
        //                          surfaceIndexes, 
        //                          l0, r0, r1);
        //             std::cout << "Added " << streamList[nL+1].size() << "tes Triangle R " << nL << " immernoch < " << streamList.size() << std::endl;
        //             // makeStep(streamList[nL + 1], r1, evaluator);
        //             if (nL > streamList.size() - 2) {
        //                 std::cout << "exiting" << std::endl;
        //                 return;
        //             }
        //             // advanceRibbon(streamList, 
        //             //               nStep,
        //             //               nL + 1,
        //             //               surfacePoints,
        //             //               surfaceIndexes,
        //             //               evaluator);
        //         }
        //         prevDiag = minDiag;
        //     }
        //     return;
        // }

        static std::shared_ptr<graphics::Drawable> drawLines(std::vector<PointF<3>> pointsFList,std::vector<VectorF<3>> vertices, Color color)
        {
            auto const &system = graphics::GraphicsSystem::instance(); // The GraphicsSystem is needed to create Drawables, which represent the to be rendererd objects.
            auto bs = graphics::computeBoundingSphere(pointsFList);
            std::string resourcePath = PluginRegistrationService::getInstance().getResourcePath("utils/Graphics"); // path to the shaders

            std::shared_ptr<graphics::Drawable> graphic = 
                system.makePrimitive(graphics::PrimitiveConfig{graphics::RenderPrimitives::LINES}
                        .vertexBuffer("position", system.makeBuffer(pointsFList))
                        .vertexBuffer("in_vertex", system.makeBuffer(vertices))
                        //.indexBuffer( system.makeIndexBuffer( vertices ) )
                        .uniform("u_lineWidth", 3.0f)
                        .uniform("u_color", color)
                        .boundingSphere(bs),
                    system.makeProgramFromFiles(resourcePath + "shader/line/noShading/singleColor/vertex.glsl",
                                                resourcePath + "shader/line/noShading/singleColor/fragment.glsl",
                                                resourcePath + "shader/line/noShading/singleColor/geometry.glsl"));

            return graphic;
        }

        static std::shared_ptr<graphics::Drawable> drawSurface(std::vector<PointF<3>> points, std::vector<unsigned int> indexes, Color color) {
            auto const &system = graphics::GraphicsSystem::instance(); // The GraphicsSystem is needed to create Drawables, which represent the to be rendererd objects.
            std::string resourcePath = PluginRegistrationService::getInstance().getResourcePath("utils/Graphics"); // path to the shaders
            // The BoundingSphere should contain all elements of the drawable and is needed for its creation.
            auto bs = graphics::computeBoundingSphere(points);
            // For the used Phong-Shading, the calculation of surface normals is necessary.
            auto norm = graphics::computeNormals(points, indexes);
            // The Drawable object defines the input streams for the shaders.
            // Vertex- and IndexBuffers as well as Uniforms can be defined as seen below.
            std::shared_ptr< graphics::Drawable> graphic = 
                system.makePrimitive(graphics::PrimitiveConfig{ graphics::RenderPrimitives::TRIANGLES }
                    .vertexBuffer( "position", system.makeBuffer(points ) )
                    .vertexBuffer( "normal", system.makeBuffer( norm ) )
                    .indexBuffer( system.makeIndexBuffer(indexes))
                    .uniform( "color", color)
                    //.renderOption( graphics::RenderOption::LineSmooth, true )
                    .boundingSphere( bs ),
                system.makeProgramFromFiles(resourcePath + "shader/surface/phong/singleColor/vertex.glsl",
                                            resourcePath + "shader/surface/phong/singleColor/fragment.glsl" ) );
            return graphic;
        }

        virtual void execute(const Algorithm::Options &options, const volatile bool & /*abortFlag*/) override
        {
            // make a structured grid to start lines at edges
            Point<3> startcoord = {options.get< double >("sx"),
                                   options.get< double >("sy"),
                                   options.get< double >("sz")};
            Point<3> endcoord =   {options.get< double >("ex"), 
                                   options.get< double >("ey"), 
                                   options.get< double >("ez")};

            std::string method = options.get<std::string>("Method");
            double dStep = options.get<double>("dStep");
            double adStep = options.get<double>("adStep");
            unsigned int nStep = options.get<size_t>("nStep") + 1;
            Color colorStartLine = options.get<Color>("colorStartLine");
            Color colorStream = options.get<Color>("colorStream");
            Color colorSurface = options.get<Color>("colorSurface");

            std::shared_ptr<const Field<3, Vector3>> field = options.get<Field<3, Vector3>>("Field");
            std::shared_ptr<const Function<Vector3>> function = options.get<Function<Vector3>>("Field");
            auto evaluator = field->makeEvaluator();

            // if there is no input, do nothing
            if (!field) {
                debugLog() << "Input Field not set." << std::endl;
                return;
            }
            // sanity check that interpolated fields really use the correct grid type. This should never fail
            std::shared_ptr<const Grid<3>> functionDomainGrid = std::dynamic_pointer_cast<const Grid<3>>(function->domain());
            if (!functionDomainGrid) {
                throw std::logic_error("Wrong type of grid!");
            }
            //make vectors for startline
            std::vector<PointF<3>> startPoints = {(PointF<3>)startcoord, (PointF<3>)endcoord};
            std::vector<VectorF<3>> startVectors = {(VectorF<3>)startcoord, (VectorF<3>)endcoord};
            
            //start by making points from start to end coordinates
            // amount of lines in first step
            int nTracer = euclidDist(startcoord, endcoord) / dStep;
            std::vector<std::vector<Point<3>>> allTracerPoints;

            for(size_t i = 0; i < nTracer; i++) {
                Point<3> p = startcoord + i * ((endcoord - startcoord) / nTracer);
                std::vector<Point<3>> oneTracerPoints;
                oneTracerPoints.push_back(p);
                makeStep(oneTracerPoints, p, evaluator);
                // for(int i = 1; i< 100; i++) {
                //     makeStep(oneTracerPoints, oneTracerPoints[i], evaluator);
                // }
                allTracerPoints.push_back(oneTracerPoints);
            }

            std::cout << allTracerPoints.size() << std::endl;
            std::cout << allTracerPoints[0].size() << std::endl;

            std::vector<PointF<3>> surfacePoints;
            std::vector<unsigned int> surfaceIndexes;
            std::vector<std::vector<size_t>> posFront(allTracerPoints.size(),{0,0});
            int nL = 0;

            while(nL < nTracer - 2 && (posFront[0][0] < nStep || posFront.back()[1] < nStep)) {
                if (posFront[nL][0] >= nStep) {
                    nL++;
                }
                std::cout << "restarting loop at " << nL << std::endl;
                advanceSimpleRibbon(allTracerPoints, posFront, dStep, adStep, nStep, nL, surfacePoints, surfaceIndexes, evaluator);
            }

            // preparing points to draw streamLines
            std::vector<PointF<3>> streamPoints;
            std::vector<VectorF<3>> streamVectors;
            for (size_t i = 0; i < allTracerPoints.size(); i++) {
                for (size_t j = 0; j < allTracerPoints[i].size(); j++) {
                    if (allTracerPoints[i].size() < 2) break;
                    streamPoints.push_back((PointF<3>) allTracerPoints[i][j]);
                    if (j!= 0 && j != allTracerPoints[i].size() - 1) {
                        streamVectors.push_back((VectorF<3>) allTracerPoints[i][j]);
                    }
                    streamVectors.push_back((VectorF<3>) allTracerPoints[i][j]);
                }
            }

            std::cout << surfacePoints.size() << surfaceIndexes.size() << std::endl;

            // making the visualization
            std::shared_ptr<graphics::Drawable> startLine = drawLines(startPoints, startVectors, colorStartLine);
            std::shared_ptr<graphics::Drawable> streamlines = drawLines(streamPoints, streamVectors, colorStream);
            std::shared_ptr<graphics::Drawable> surface = drawSurface(surfacePoints, surfaceIndexes, colorSurface);
            setGraphics("startline", startLine);   
            setGraphics("streamlines", streamlines);
            setGraphics("surface", surface);
        }
    };
    AlgorithmRegister<IntegrateTask> dummy("Tasks/GroupTask", "Show the streamlines for an input vector field");
}