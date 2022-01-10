// Heeenlo
#include <fantom/algorithm.hpp>
#include <fantom/graphics.hpp>
#include <fantom/register.hpp>
#include <fantom/dataset.hpp>

// for BoundingSphere calculation
#include <fantom-plugins/utils/Graphics/HelperFunctions.hpp>
#include <fantom-plugins/utils/Graphics/ObjectRenderer.hpp>

#include <vector>
#include <math.h>

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
                add<InputChoices>("Surface", "surface?", std::vector<std::string>{"Yes", "No"}, "Yes");
                addSeparator();
                add< double >( "ox", "origin grid in x-dimension", -4.0 );
                add< double >( "oy", "origin grid in y-dimension", 0.5 );
                add< double >( "oz", "origin grid in z-dimension", 1.0 );
                addSeparator();
                add< size_t >( "nx", "number lines in x-dimension", 1 );
                add< size_t >( "ny", "number lines in y-dimension", 7 );
                add< size_t >( "nz", "number lines in z-dimension", 5 );
                addSeparator();
                add< double >( "dx", "block width in x-dimension", 1.0 );
                add< double >( "dy", "block width in y-dimension", 1.0 );
                add< double >( "dz", "block width in z-dimension", 1.0 );
                addSeparator();
                add<Field<3, Vector3>>("Field", "3D vector field", definedOn<Grid<3>>(Grid<3>::Points));
                add<InputChoices>("Method", "calculation method.", std::vector<std::string>{"Euler", "Runge-Kutta"}, "Euler");
                add<double>("dStep", "distance between steps", 0.05);
                add<double>("adStep", "for calculating new step size", 0.02);
                add<size_t>("nStep", "max number of steps", 100);
                addSeparator();
                add<Color>("colorGrid", "The color of the grid", Color(1.0, 1.0, 1.0));
                add<Color>("colorStream", "The color of the streamlines", Color(1.0, 0.0, 0.0));
            }
        };

        struct VisOutputs : public VisAlgorithm::VisOutputs
        {
            VisOutputs(fantom::VisOutputs::Control &control)
                : VisAlgorithm::VisOutputs(control)
            {
                addGraphics("streams");
                addGraphics("grid");
            }
        };

        IntegrateTask(InitData &data)
            : VisAlgorithm(data)
        {
        }

        // function for euler lines
        static void makeEuler(double &dStep, double &adStep, size_t nStep,
                          double &x, double &y, double &z, 
                          std::vector<Point<3>> &points, 
                          std::shared_ptr<const Field<3, Vector3>> &field) {
            while (points.size() < nStep) {
                Point3 p = {x, y, z};
                auto evaluator = field->makeEvaluator();

                // check if in domain
                if (evaluator->reset(p)) {
                    //get value at point
                    auto v = evaluator->value();
                    //return if velocity is 0
                    if (v[0] == 0 && v[1] == 0 && v[2] == 0){
                        return;
                    }

                    // normal point with normal dStep
                    double xS = x + dStep * v[0];
                    double yS = y + dStep * v[1];
                    double zS = z + dStep * v[2];

                    // single step with half dStep size
                    double pS = abs(xS + yS + zS);

                    // helper point with half dStep
                    double xT = x + dStep / 2 * v[0];
                    double yT = y + dStep / 2 * v[1];
                    double zT = z + dStep / 2 * v[2];

                    double pD;
                    double xD;
                    double yD;
                    double zD;

                    //if in domain still
                    if (evaluator->reset(Point3(xT, yT, zT))) {
                        auto hv = evaluator->value();
                        // alternative point full dStep
                        xD = xT + dStep / 2 * hv[0];
                        yD = yT + dStep / 2 * hv[1];
                        zD = zT + dStep / 2 * hv[2];
                        // double step with dStep size
                        pD = abs(xD + yD + zD);
                    }

                    // if difference between one step point and two step
                    if (pS - pD > adStep) {
                        dStep = dStep / 2;
                    }
                    else if (pS - pD < adStep / 2) {
                        dStep = 2 * dStep;
                        points.push_back(p);
                        x = xS;
                        y = yS;
                        z = zS;
                    }
                    else if (pS - pD < adStep) {
                        points.push_back(p);
                        x = xD;
                        y = yD;
                        z = zD;
                    }
                }
                // stop if the point is out of area
                else {
                    return;
                }
            }
            return;
        }

        // function for runge kutta lines
        static void makeRungeKutta(double &dStep, size_t nStep,
                               double &x, double &y, double &z, 
                               std::vector<Point<3>> &points, 
                               std::shared_ptr<const Field<3, Vector3>> &field) {
            while (points.size() < nStep) {
                auto evaluator = field->makeEvaluator();
                Point3 p = {x, y, z};

                double q1x, q1y, q1z;
                double q2x, q2y, q2z;
                double q3x, q3y, q3z;
                double q4x, q4y, q4z;

                // calculate the 4 q-values
                if (evaluator->reset(p))
                {
                    //calculate value at this point
                    auto v1 = evaluator->value();

                    //if there is no velocity at this point stop the loop
                    if (v1[0] == 0 and v1[1] == 0 and v1[2] == 0){
                        return;
                    }

                    q1x = dStep * v1[0];
                    q1y = dStep * v1[1];
                    q1z = dStep * v1[2];
                }
                // if point is not in the domain, exit the loop
                else
                {
                    //std::cout << "Point is outside of the domain" << std::endl;
                    return;
                }


                if (evaluator->reset({x + 0.5 * q1x, y + 0.5 * q1y, z + 0.5 * q1z})) {
                    //calculate value at this point
                    auto v2 = evaluator->value();
                    q2x = dStep * v2[0];
                    q2y = dStep * v2[1];
                    q2z = dStep * v2[2];
                }
                if (evaluator->reset({x + 0.5 * q2x, y + 0.5 * q2y, z + 0.5 * q2z})) {
                    //calculate value at this point
                    auto v3 = evaluator->value();
                    q3x = dStep * v3[0];
                    q3y = dStep * v3[1];
                    q3z = dStep * v3[2];
                }
                if (evaluator->reset({x + 0.5 * q3x, y + 0.5 * q3y, z + 0.5 * q3z})) {
                    //calculate value at this point
                    auto v4 = evaluator->value();
                    q4x = dStep * v4[0];
                    q4y = dStep * v4[1];
                    q4z = dStep * v4[2];
                }

                //calculate other side 
                double f_x = (q1x + 2 * q2x + 2 * q3x + q4x) / 6.0;
                double f_y = (q1y + 2 * q2y + 2 * q3y + q4y) / 6.0;
                double f_z = (q1z + 2 * q2z + 2 * q3z + q4z) / 6.0;

                // make point
                points.push_back(p);
                // add to original
                x = x + f_x;
                y = y + f_y;
                z = z + f_z;
            }
            return;
        }

        static void makeSurface(){
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


        virtual void execute(const Algorithm::Options &options, const volatile bool & /*abortFlag*/) override
        {
            // make a structured grid to start lines at edges
            double origin[] = { options.get< double >("ox"),
                                options.get< double >("oy"),
                                options.get< double >("oz")};
            size_t extent[] = { options.get< size_t >("nx"),
                                options.get< size_t >("ny"),
                                options.get< size_t >("nz")};
            double spacing[] = {options.get< double >("dx"), 
                                options.get< double >("dy"), 
                                options.get< double >("dz")};
            std::shared_ptr< const Grid< 3 > > grid = DomainFactory::makeUniformGrid( extent, origin, spacing );
            const ValueArray< Point3 >& pointsGrid = grid->points();
            std::vector<VectorF<3>> connectGrid;
            std::vector<PointF<3>> pointFGrid;
            
            // for cell in grid
            for (size_t i = 0; i < grid->numCells(); i++) {
                Cell cell = grid->cell( i );
                std::vector<size_t> connections = {0,1, 1,2, 2,3, 3,0,
                                                   7,6, 6,5, 5,4, 4,7,
                                                   0,7, 1,6, 2,5, 3,4 };

                // std::vector<size_t> connections = { 0,1,1,2,2,3,3,0,0,7,7,6,6,1,6,5,5,2,5,4,4,3,4,7 };

                // make points array to pointf vector
                for (size_t j = 0; j < 8; j++) {
                    Point3 p = pointsGrid[cell.index( j )];
                    pointFGrid.push_back(PointF<3>(p[0], p[1], p[2]));
                }

                // make connections of grid to connect vector
                for (size_t k = 0; k < connections.size(); k++) {
                    Point3 p = pointsGrid[cell.index( connections[k] )];
                    connectGrid.push_back(VectorF<3>(p));
                }
            }

            std::string surface = options.get<std::string>("Surface");
            std::string method = options.get<std::string>("Method");
            double dStep = options.get<double>("dStep");
            double adStep = options.get<double>("adStep");
            size_t nStep = options.get<size_t>("nStep");
            Color colorGrid = options.get<Color>("colorGrid");
            Color colorStream = options.get<Color>("colorStream");

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

            // prepare for surface
            std::vector<std::vector<Point<3>>> streamList;
            
            // prepare for the streams
            std::vector<VectorF<3>> connectStream;
            std::vector<PointF<3>> pointFStream;

            // all points of the grid make a stream
            for (size_t i = 0; i < functionDomainGrid->numPoints(); i++) {
                // get starting coords
                Point3 p = grid->points()[i];
                double x = p[0], y = p[1], z = p[2];
                std::vector<Point<3>> points;

                if (method == "Euler") {
                    makeEuler(dStep, adStep, nStep, x, y, z, points, field);
                }
                else if (method == "Runge-Kutta") {
                    makeRungeKutta(dStep, nStep, x, y, z, points, field);
                }
                else {
                    std::cout << "Something went wrong" << std::endl;
                }

                // fill vector with all stream points and make connections between them in vectorF vector
                for (size_t i = 0; i < points.size(); i++) {
                    if (points.size() < 2) {
                        break;
                    }
                    pointFStream.push_back(PointF<3>(points[i][0], points[i][1], points[i][2]));
                    if (i != 0 && i != points.size() - 1) {
                        connectStream.push_back(VectorF<3>(points[i]));
                    }
                    connectStream.push_back(VectorF<3>(points[i]));
                }
                if (surface == "Yes") {
                    streamList.push_back(points);
                }
            }
            

            // making the visualization
            std::shared_ptr<graphics::Drawable> gridLines = drawLines(pointFGrid, connectGrid, colorGrid);
            std::shared_ptr<graphics::Drawable> streamlines = drawLines(pointFStream, connectStream, colorStream);
            setGraphics("grid", gridLines);   
            setGraphics("streams", streamlines);
        }
    };
    AlgorithmRegister<IntegrateTask> dummy("Tasks/Task4", "Show the streamlines for an input vector field");
}