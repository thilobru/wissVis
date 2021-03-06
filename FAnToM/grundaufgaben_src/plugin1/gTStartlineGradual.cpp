#include <fantom/algorithm.hpp>
#include <fantom/graphics.hpp>
#include <fantom/register.hpp>
#include <fantom/dataset.hpp>

// for BoundingSphere calculation
#include <fantom-plugins/utils/Graphics/HelperFunctions.hpp>
#include <fantom-plugins/utils/Graphics/ObjectRenderer.hpp>

#include <vector>
#include <math.h>
#include <unistd.h>
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
                add<InputChoices>("Method", "calculation method.", std::vector<std::string>{"Euler", "Runge-Kutta"}, "Runge-Kutta");
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

        static Point<3> makeStep(Point<3> p,
                             std::string method,
                                double& dStep,
                                double& adStep,
                             std::unique_ptr<FieldEvaluator<3UL, Vector3>>& evaluator) {
            if (method == "Euler") return stepEuler(p, dStep, adStep, evaluator);
            else return stepRungeKutta(p, dStep, evaluator);
        }

        static Point<3> stepEuler(Point<3> p,
                              double& dStep,
                              double& adStep,
                              std::unique_ptr<FieldEvaluator<3UL, Vector3>>& evaluator) {
            if (evaluator->reset(p)) {
                auto v = evaluator->value();
                //if there is no velocity at this point stop the loop
                if (v[0] == 0 and v[1] == 0 and v[2] == 0) {
                    std::cout << "1error" << std::endl;
                    return p;
                }
                Point<3> s = p + dStep * v;
                double pS = abs(s[0] + s[1] + s[2]);
                //if in domain still
                Point<3> d;
                Point<3> t = p + dStep / 2 * v;
                double pD = 0.0;
                if (evaluator->reset(t)) {
                    auto hv = evaluator->value();
                    // alternative point full dStep
                    d = t + dStep / 2 * hv;
                    // double step with dStep size
                    pD = abs(d[0] + d[1] + d[2]);
                }
                if (pS - pD > adStep) {
                    dStep = dStep / 2;
                } else if (pS - pD < adStep / 2) {
                    dStep = dStep * 2;
                    return s;
                } else if (pS - pD < adStep) {
                    return d;
                }
            }
            return p;
        }

        static Point<3> stepRungeKutta(Point<3> p,
                                       double& dStep,
                                       std::unique_ptr<FieldEvaluator<3UL, Vector3>>& evaluator) {
            Point<3> n = {0, 0, 0};
            std::vector<Point<3>> q = {n, n, n, n};
            if (evaluator->reset(p)) {
                auto v = evaluator->value();
                // if there is no velocity at this point stop the loop
                if (v[0] == 0 and v[1] == 0 and v[2] == 0) {
                    std::cout << "1error" << std::endl;
                    return p;
                }
                q[0] = (dStep * v);
            } else {
                std::cout << "2error" << std::endl;
                return p;
            }

            for(int i = 0; i < 3; i++) {
                if (evaluator->reset(p + 0.5 * q[i])){
                    auto v = evaluator->value();
                    q[i + 1] = (dStep * v);
                }
            }

            n = p + (q[0] + 2 * q[1] + 2 * q[2] + q[3]) / 6;
            // std::cout << "added " << n << " by way of " << p << (q[0] + 2 * q[1] + 2 * q[2] + q[3]) / 6 << std::endl;
            if (evaluator->reset(n)) {
                return n;
            }
            return p;
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

        static bool addParticle(std::vector<std::vector<Point<3>>> &streamList, 
                                std::vector<std::vector<size_t>> &posFront,
                                size_t nL,
                                size_t posL0, size_t posR0,
                                Point<3> l0, Point<3> l1,
                                Point<3> r0, Point<3> r1,
                                std::string method,
                                double& dStep,
                                double& adStep,
                                unsigned int& nStep,
                                std::unique_ptr<FieldEvaluator<3UL, Vector3>>& evaluator) {
            if (posFront[nL][0] > posFront[nL][4] - 10 || streamList.size() > 1000) {
                return false;
            }
            if (euclidDist(l1, r1) > 2 * euclidDist(l0, l1)) {
                Point<3> newP = l1 + ((r1 - l1) / 2);
                std::vector<Point<3>> newTracer;
                newTracer.push_back(newP);
                for ( size_t j = 0; j < 1; j++) {
                    newTracer.push_back(makeStep(newTracer[j], method, dStep, adStep, evaluator));
                }
                streamList.push_back(newTracer);
                posFront.insert(posFront.begin() + nL + 1, {0,posR0 + 1,
                                                streamList.size() - 1,posFront[nL][3],nStep - posL0, 1});
                posFront[nL][1] = 0;
                posFront[nL][3] = streamList.size() - 1;
                // std::cout << "added" << std::endl;
                return true;
            } else {
                return false;
            }
        }

        static bool remParticle(std::vector<std::vector<Point<3>>> &streamList, 
                                std::vector<std::vector<size_t>> &posFront,
                                std::vector<PointF<3>> &surfacePoints, 
                                std::vector<unsigned int> &surfaceIndexes, 
                                size_t nL,
                                Point<3> l0, Point<3> l1,
                                Point<3> r0, Point<3> r1){
            if (posFront[nL][0] > posFront[nL][4] - 5 || nL < 1) return false;
            size_t strL = posFront[nL - 1][2];
            Point<3> m0 = l0;
            // Point<3> m1 = l1;
            l0 = streamList[strL][posFront[nL-1][0] - 1];
            l1 = streamList[strL][posFront[nL-1][0]];
            double height = (euclidDist(l0,l1) + euclidDist(r0,r1)) / 2;
            double width  = euclidDist(l1,r1);
            // if ((nL == 50 && posFront[nL][0] == 75) || (nL == 90 &&posFront[nL][0] == 75)) {
            if (height > width) {
                posFront[nL - 1][1] = posFront[nL][1];
                posFront[nL - 1][3] = posFront[nL][3];
                // streamList.erase(streamList.begin() + nL);
                posFront.erase(posFront.begin() + nL);
                makeTriangle(surfacePoints, surfaceIndexes, m0,r1,l1);
                makeTriangle(surfacePoints, surfaceIndexes, m0,r0,r1);
                std::cout << "dood" << std::endl;
                return true;
            }
            return false;
        }

        static bool ripRibbon(std::vector<std::vector<size_t>> &posFront,
                              size_t nL,
                              Point<3> l0, Point<3> l1,
                              Point<3> r0, Point<3> r1) {
            // if (nL == 75 && posFront[nL][0] == 75) {
            if (euclidDist((l1 - l0) + (r1 - r0), {0,0,0}) 
                < (euclidDist(l0, l1) + euclidDist(r0, r1)) / 2) {
                posFront[nL][5] = 0;
                return true;
            } else {
                return false;
            }
        }

        static void advanceRibbon(std::vector<std::vector<Point<3>>> &streamList, 
                                std::vector<std::vector<size_t>> &posFront,
                                std::string method,
                                double& dStep,
                                double& adStep,
                                unsigned int& nStep,
                                std::unique_ptr<FieldEvaluator<3UL, Vector3>>& evaluator,
                                size_t nL, int& rem,
                                std::vector<PointF<3>> &surfacePoints, 
                                std::vector<unsigned int> &surfaceIndexes) {
            float prevDiag = INFINITY;
            bool caughtUp = false;
            if (nL > posFront.size() - 2) {return;}
            while(true) {
                // usleep(10000);
                size_t strL = posFront[nL][2];
                size_t strR = posFront[nL][3];
                size_t posL0 = std::min(streamList[strL].size()-2, posFront[nL][0]);
                size_t posR0 = std::min(streamList[strR].size()-2, posFront[nL][1]);
                // define quad to determine shortest diagonal
                Point<3> l0 = streamList[strL][posL0];
                Point<3> l1 = streamList[strL][posL0 + 1];
                Point<3> r0 = streamList[strR][posR0];
                Point<3> r1 = streamList[strR][posR0 + 1];

                if (addParticle(streamList, posFront, nL, posL0, posR0, l0, l1, r0, r1, method, dStep, adStep, nStep, evaluator)) {
                    makeTriangle(surfacePoints, surfaceIndexes, l0, r0, streamList[posFront[nL + 1][2]][0]);
                    posR0 = 0;
                    strR = posFront[nL][3];
                    r0 = streamList[strR][0];
                    r1 = streamList[strR][1];
                    // std::cout << "added r of " << nL << std::endl;
                    // continue;
                }
                // else if (remParticle(streamList, posFront, surfacePoints, surfaceIndexes, nL, l0, l1, r0, r1)) {
                //     std::cout << "removed " << strL << std::endl; 
                //     rem++;
                //     return;
                //     // streamList[posFront[nL - 1][3]].push_back(makeStep(r1, method, dStep, adStep, evaluator));
                //     // advanceRibbon(streamList, 
                //     //               posFront, method, dStep, adStep, nStep, 
                //     //               evaluator,
                //     //               nL + 1, rem,
                //     //               surfacePoints,
                //     //               surfaceIndexes);
                // }
                if (ripRibbon(posFront, nL, l0, l1, r0, r1) && posFront[nL][5] != 0) {
                    // continue;
                    std::cout << "RIPRIPRIP" << std::endl;
                }

                float lDiag = euclidDist(l1, r0);
                float rDiag = euclidDist(l0, r1);
                float minDiag = std::min(lDiag, rDiag);
                bool advanceOnLeft = (lDiag == minDiag);

                if(posL0 > nStep - 1 || l0 == l1 || r0 == r1 || streamList.size() > 1000){// || posFront[nL][5] == 0) {
                    // std::cout << "Finished" << nL << posFront[nL][0] << (l0 == l1) << (r0 == r1) << std::endl;
                    posFront[nL][0] = nStep - 2;
                    posFront[nL][1] = nStep - 2;
                    return;
                }
                if(caughtUp && (advanceOnLeft || rDiag > prevDiag)) {    
                    // std::cout << "Caught Up" << std::endl;
                    return;
                }
                if (advanceOnLeft) {
                    if (posFront[nL][5]) {
                        makeTriangle(surfacePoints, 
                                     surfaceIndexes, 
                                     l0, r0, l1);
                    }
                    // std::cout << "Added Triangle L" << std::endl;
                    if (streamList[strL].size() < nStep - 1
                        && posL0 >= streamList[strL].size() - 2) {
                        streamList[strL].push_back(makeStep(l1, method, dStep, adStep, evaluator));
                    }
                    posFront[nL][0]++;
                    caughtUp = true;
                } else {
                    if (posFront[nL][5]) {
                        makeTriangle(surfacePoints, 
                                     surfaceIndexes, 
                                     l0, r0, r1);
                    }
                    // std::cout << "Added Triangle R" << nL << "immernoch < " << streamList.size() << std::endl;
                    if (streamList[strR].size() < nStep - 1
                        && posR0 >= streamList[strR].size() - 2) {
                        streamList[strR].push_back(makeStep(r1, method, dStep, adStep, evaluator));
                    }
                    posFront[nL][1]++;
                    if (nL >= posFront.size() - 2 ||
                        posR0 > streamList[strR].size() - 2) {
                        // std::cout << "exiting" << std::endl;
                        return;
                    }
                    advanceRibbon(streamList, 
                                  posFront, method, dStep, adStep, nStep, 
                                  evaluator,
                                  nL + 1, rem,
                                  surfacePoints,
                                  surfaceIndexes);
                }
                prevDiag = minDiag;
            }
            return;
        }

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

            auto evaluator = field->makeEvaluator();
            
            //make vectors for startline
            std::vector<PointF<3>> startPoints = {(PointF<3>)startcoord, (PointF<3>)endcoord};
            std::vector<VectorF<3>> startVectors = {(VectorF<3>)startcoord, (VectorF<3>)endcoord};

            //start by making points from start to end coordinates
            // amount of lines in first step
            size_t nTracer = euclidDist(startcoord, endcoord) / dStep + 1;
            std::vector<std::vector<Point<3>>> streamList;

            for(size_t i = 0; i <= nTracer; i++) {
                Point<3> p = startcoord + i * ((endcoord - startcoord) / nTracer);
                if (!(evaluator->reset(p))) continue;
                std::vector<Point<3>> oneTracerPoints;
                oneTracerPoints.push_back(p);
                for ( size_t j = 0; j < 1; j++) {
                    oneTracerPoints.push_back(makeStep(oneTracerPoints[j], method, dStep, adStep, evaluator));
                }
                streamList.push_back(oneTracerPoints);
            }
            nTracer = streamList.size();
            //std::set<PointF<3>> surfacePointsSet;
            std::vector<PointF<3>> surfacePoints;
            std::vector<unsigned int> surfaceIndexes;
            std::vector<std::vector<size_t>> posFront;
            /* posFront describes:
            0,1: Position on left and right Streamline
            2,3: Position of left and right Streamline vector in Streamlist
            4: Amount of Points to be drawn 
            5: 0 if ripped 1 otherwise*/
            for(size_t i = 0; i < streamList.size(); i++) {
                posFront.push_back({0,0,i,i+1,nStep,1});
            }
            //advanceRibbonSimp(streamList, posFront, 0, surfacePoints, surfaceIndexes);
            //position marker for finished streamline
            size_t nL = 0;
            int rem = 0;
            std::cout << streamList.size() << std::endl;
            std::cout << nTracer << std::endl;
            if (streamList.size() > 1){
                while((posFront[0][0] < nStep - 2
                    || posFront[posFront.size()-2][1] < nStep - 2) 
                    // && streamList.size() < 1000
                    && nL <= posFront.size() - 2) {
                    advanceRibbon(streamList, posFront, method, dStep, adStep, nStep, evaluator, nL, rem, surfacePoints, surfaceIndexes);
                    if(posFront[nL][0] >= nStep - 2) {
                        nL++;
                    }
                    // std::cout << nL << std::endl;
                }
            }
            // advanceRibbon(streamList, posFront, method, dStep, adStep, nStep, evaluator, nL, rem, surfacePoints, surfaceIndexes);
            std::cout << streamList.size() << std::endl;
            std::cout << nTracer << std::endl;

            // convert set to vector
            // std::vector<PointF<3>> surfacePoints(surfacePointsSet.begin(), surfacePointsSet.end());

            // preparing points to draw streamLines
            std::vector<PointF<3>> streamPoints;
            std::vector<VectorF<3>> streamVectors;
            for (size_t i = 0; i < streamList.size(); i++) {
                for (size_t j = 0; j < streamList[i].size(); j++) {
                    if (streamList[i].size() < 2) break;
                    streamPoints.push_back((PointF<3>) streamList[i][j]);
                    if (j!= 0 && j != streamList[i].size() - 1) {
                        streamVectors.push_back((VectorF<3>) streamList[i][j]);
                    }
                    streamVectors.push_back((VectorF<3>) streamList[i][j]);
                }
            }
            
            // making the visualization
            std::shared_ptr<graphics::Drawable> startLine = drawLines(startPoints, startVectors, colorStartLine);
            std::shared_ptr<graphics::Drawable> streamlines = drawLines(streamPoints, streamVectors, colorStream);
            std::shared_ptr<graphics::Drawable> surface = drawSurface(surfacePoints, surfaceIndexes, colorSurface);
            setGraphics("startline", startLine);   
            setGraphics("streamlines", streamlines);
            setGraphics("surface", surface);
        }
    };
    AlgorithmRegister<IntegrateTask> dummy("Tasks/GTStartlineGradual", "Show the streamlines for an input vector field");
}