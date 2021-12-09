#include <fantom/algorithm.hpp>
#include <fantom/datastructures/interfaces/Field.hpp>
#include <fantom/graphics.hpp>
#include <fantom/register.hpp>
#include <math.h>
#include <fantom/dataset.hpp>

// for BoundingSphere calculation
#include <fantom-plugins/utils/Graphics/HelperFunctions.hpp>
#include <fantom-plugins/utils/Graphics/ObjectRenderer.hpp>

#include <stdexcept>
#include <vector>
#include <cstdlib>
using namespace std;
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
                add< double >( "ox", "origin of grid in x-dimension", -5.0 );
                add< double >( "oy", "origin of grid in y-dimension", 1.0 );
                add< double >( "oz", "origin of grid in z-dimension", 1.0 );
                addSeparator();
                add< size_t >( "nx", "number lines in x-dimension", 4 );
                add< size_t >( "ny", "number lines in y-dimension", 4 );
                add< size_t >( "nz", "number lines in z-dimension", 4 );
                addSeparator();
                add< double >( "dx", "block width in x-dimension", 1.0 );
                add< double >( "dy", "block width in y-dimension", 2.0 );
                add< double >( "dz", "block width in z-dimension", 1.0 );
                addSeparator();
                add<Field<3, Vector3>>("Field", "3D vector field", definedOn<Grid<3>>(Grid<3>::Points));
                add<double>("Step size", "distance between steps", 0.05);
                add<double>("adaptive step size", "number for calculating the new step size.", 0.02);
                add<InputChoices>("Method", "calculation method.", std::vector<std::string>{"Euler", "Runge-Kutta"}, "Euler");
                addSeparator();
                add<Color>("colorGrid", "The color of the grid.", Color(1.0, 1.0, 1.0));
                add<Color>("colorStream", "The color of the streamlines.", Color(1.0, 0.0, 0.0));
                add<size_t>("Number of steps", "Define a maximum number of points in the streamline", 100);
            }
        };

        struct VisOutputs : public VisAlgorithm::VisOutputs
        {
            VisOutputs(fantom::VisOutputs::Control &control)
                : VisAlgorithm::VisOutputs(control)
            {
                addGraphics("Streamlines");
                addGraphics("Grid");
            }
        };

        IntegrateTask(InitData &data)
            : VisAlgorithm(data)
        {
        }

        // Euler function for the calculation for the points of the streamlines
        static void euler(double &stepSize, double &e, 
                          double &x, double &y, double &z, 
                          std::vector<Point<3>> &points, 
                          std::shared_ptr<const Field<3, Vector3>> &field,
                          size_t max_steps) {
            while (points.size() < max_steps) {
                Point3 p = {x, y, z};
                auto evaluator = field->makeEvaluator();

                // if point is in domain
                if (evaluator->reset(p)) {
                    //calculate value at this point
                    auto v = evaluator->value();
                    //return if velocity is 0
                    if (v[0] == 0 && v[1] == 0 && v[2] == 0){
                        return;
                    }

                    // normal point with normal stepSize
                    double xN = x + stepSize * v[0];
                    double yN = y + stepSize * v[1];
                    double zN = z + stepSize * v[2];

                    // Manhattan dist of normals
                    double pN = abs(xN + yN + zN);

                    // helper point with half h
                    double xH = x + stepSize / 2 * v[0];
                    double yH = y + stepSize / 2 * v[1];
                    double zH = z + stepSize / 2 * v[2];

                    Point3 helper = Point3(xH, yH, zH);
                    double pA;
                    double xA;
                    double yA;
                    double zA;

                    if (evaluator->reset(helper))
                    {
                        auto hv = evaluator->value();

                        // alternative point with twice half h
                        xA = xH + stepSize / 2 * hv[0];
                        yA = yH + stepSize / 2 * hv[1];
                        zA = zH + stepSize / 2 * hv[2];

                        // calculate the dimensions together
                        pA = abs(xA + yA + zA);
                    }

                    // check difference between normal and alternative point
                    if (pN - pA > e) {
                        stepSize = stepSize / 2;
                    }
                    else if (pN - pA < e / 2) {
                        stepSize = 2 * stepSize;
                        points.push_back(p);
                        x = xN;
                        y = yN;
                        z = zN;
                    }
                    else if (pN - pA < e) {
                        points.push_back(p);
                        x = xA;
                        y = yA;
                        z = zA;
                    }
                }

                // if the point is outside of the domain, we will stop adding more points to the streamline
                else
                {
                    //std::cout << "Point is outside of the domain" << std::endl;
                    return;
                }
            }
            std::cout << "Max steps reached" << std::endl;
            return;
        }

        static void rungeKutta(double &stepSize, 
                               double &x, double &y, double &z, 
                               std::vector<Point<3>> &points, 
                               std::shared_ptr<const Field<3, Vector3>> &field, 
                               size_t max_steps)
        {
            while (points.size() < max_steps) {
                auto evaluator = field->makeEvaluator();
                Point3 p = {x, y, z};

                double q1x;
                double q1y;
                double q1z;
                double q2x;
                double q2y;
                double q2z;
                double q3x;
                double q3y;
                double q3z;
                double q4x;
                double q4y;
                double q4z;

                // calculate the 4 q-values
                if (evaluator->reset(p))
                {
                    //calculate value at this point
                    auto v1 = evaluator->value();

                    //if there is no velocity at this point stop the loop
                    if (v1[0] == 0 and v1[1] == 0 and v1[2] == 0){
                        return;
                    }

                    q1x = stepSize * v1[0];
                    q1y = stepSize * v1[1];
                    q1z = stepSize * v1[2];
                }
                // if point is not in the domain, exit the loop
                else
                {
                    //std::cout << "Point is outside of the domain" << std::endl;
                    return;
                }


                if (evaluator->reset({x + 0.5 * q1x, y + 0.5 * q1y, z + 0.5 * q1z}))
                {
                    //calculate value at this point
                    auto v2 = evaluator->value();
                    q2x = stepSize * v2[0];
                    q2y = stepSize * v2[1];
                    q2z = stepSize * v2[2];
                }
                if (evaluator->reset({x + 0.5 * q2x, y + 0.5 * q2y, z + 0.5 * q2z}))
                {
                    //calculate value at this point
                    auto v3 = evaluator->value();
                    q3x = stepSize * v3[0];
                    q3y = stepSize * v3[1];
                    q3z = stepSize * v3[2];
                }
                if (evaluator->reset({x + 0.5 * q3x, y + 0.5 * q3y, z + 0.5 * q3z}))
                {
                    //calculate value at this point
                    auto v4 = evaluator->value();
                    q4x = stepSize * v4[0];
                    q4y = stepSize * v4[1];
                    q4z = stepSize * v4[2];
                }

                //calculate right side of formula
                double formula_x = (1.0/6.0) * (q1x + 2 * q2x + 2 * q3x + q4x);
                double formula_y = (1.0/6.0) * (q1y + 2 * q2y + 2 * q3y + q4y);
                double formula_z = (1.0/6.0) * (q1z + 2 * q2z + 2 * q3z + q4z);

                // calculate new coordinates
                //std::cout << "Right side formula: " << formula_x << " " << formula_y << " " << formula_z << std::endl;

                points.push_back(p);
                x = x + formula_x;
                y = y + formula_y;
                z = z + formula_z;

                //std::cout << "New coordinates: " << x << " " << y << " " << z << std::endl << std::endl;
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
                        .uniform("u_lineWidth", 1.0f)
                        .uniform("u_color", color)
                        .boundingSphere(bs),
                    system.makeProgramFromFiles(resourcePath + "shader/line/noShading/singleColor/vertex.glsl",
                                                resourcePath + "shader/line/noShading/singleColor/fragment.glsl",
                                                resourcePath + "shader/line/noShading/singleColor/geometry.glsl"));

            return graphic;
        }


        virtual void execute(const Algorithm::Options &options, const volatile bool & /*abortFlag*/) override
        {
            // create the grid where the streamlines will start
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

            Color colorGrid = options.get<Color>("colorGrid");

            //--------------------------------------------------------------------------------------------------------------------

            //make grid where streamlines will start
            std::vector<VectorF<3>> verticesGrid;
            std::vector<PointF<3>> pointsFListGrid = {};
            const ValueArray< Point3 >& pointsGrid = grid->points();

            
            // for each cell in grid: 
            for (size_t i = 0; i < grid->numCells(); i++) {
                Cell cell = grid->cell( i );
                std::vector<size_t> connections = { 0,1,1,2,2,3,3,0,0,7,7,6,6,1,6,5,5,2,5,4,4,3,4,7 };

                // transform normal points list to PointF list
                for (size_t j = 0; j < 8; j++) {
                    Point3 p = pointsGrid[cell.index( j )];
                    pointsFListGrid.push_back(PointF<3>(p[0], p[1], p[2]));

                    std::cout << "Point " << j << " of Cell " << i << ": " << p << std::endl;
                }

                // make vertices list with correct connections
                for (size_t k = 0; k < connections.size(); k++) {
                    Point3 p = pointsGrid[cell.index( connections[k] )];
                    verticesGrid.push_back(VectorF<3>(p));
                }
            }

            //--------------------------------------------------------------------------------------------------------------------

            // get all other options and check validity of them

            double stepSize = options.get<double>("Step size");
            double e = options.get<double>("adaptive step size");
            std::string method = options.get<std::string>("Method");
            Color colorStream = options.get<Color>("colorStream");
            size_t max_steps = options.get<size_t>("Number of steps");

            std::shared_ptr<const Field<3, Vector3>> field = options.get<Field<3, Vector3>>("Field");
            std::shared_ptr<const Function<Vector3>> function = options.get<Function<Vector3>>("Field");

            // if there is no input, do nothing
            if (!field)
            {
                debugLog() << "Input Field not set." << std::endl;
                return;
            }

            // sanity check that interpolated fields really use the correct grid type. This should never fail
            std::shared_ptr<const Grid<3>> gridStream = std::dynamic_pointer_cast<const Grid<3>>(function->domain());

            if (!gridStream)
            {
                throw std::logic_error("Wrong type of grid!");
            }

            //--------------------------------------------------------------------------------------------------------------------

            // generate the points of the streamlines

            std::vector<VectorF<3>> verticesStream;
            std::vector<PointF<3>> pointsFListStream = {};

            // for each point in the generated grid a streamline is created
            for (size_t i = 0; i < gridStream->numPoints(); i++) 
            {
                // get each starting point
                Point3 p = grid->points()[i];
                double x = p[0];
                double y = p[1];
                double z = p[2];

                std::cout << "Starting point: " << x << " " << y << " " << z << std::endl;

                // calculate streamline points and add them to points 
                std::vector<Point<3>> points;

                if (method == "Euler")
                {
                    euler(stepSize, e, x, y, z, points, field, max_steps);
                }

                if (method == "Runge-Kutta")
                {
                    rungeKutta(stepSize, x, y, z, points, field, max_steps);
                }

                // transform normal points list to PointF list and make vertices list

                for (size_t i = 0; i < points.size(); i++)
                {
                    pointsFListStream.push_back(PointF<3>(points[i][0], points[i][1], points[i][2]));

                    if (i != 0 && i != points.size() - 1)
                    {
                        verticesStream.push_back(VectorF<3>(points[i]));
                    }
                    verticesStream.push_back(VectorF<3>(points[i]));
                }
            }

            //--------------------------------------------------------------------------------------------------------------------

            // drawing lines between the points for the grid and the streamlines

            std::shared_ptr<graphics::Drawable> gridLines = drawLines(pointsFListGrid, verticesGrid, colorGrid);  
            setGraphics("Grid", gridLines);   

            std::shared_ptr<graphics::Drawable> streamlines = drawLines(pointsFListStream, verticesStream, colorStream);  
            setGraphics("Streamlines", streamlines);
        }
    };
    AlgorithmRegister<IntegrateTask> dummy("Tasks/Task4", "Show the streamlines for an input vector field");
}