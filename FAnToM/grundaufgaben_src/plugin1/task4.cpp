#include <fantom/algorithm.hpp>
#include <fantom/datastructures/interfaces/Field.hpp>
#include <fantom/graphics.hpp>
#include <fantom/register.hpp>
#include <math.h>
#include <fantom/dataset.hpp>

// needed for BoundinSphere-Calculation
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
                add< long >( "nx", "number lines in x-dimension", 5 );
                add< long >( "ny", "number lines in y-dimension", 1 );
                add< long >( "nz", "number lines in z-dimension", 5 );
 
                addSeparator();

                add< double >( "dx", "Width of each cell in x-Direction", 0.5 );
                add< double >( "dy", "Width of each cell in y-Direction", 2.0 );
                add< double >( "dz", "Width of each cell in z-Direction", 0.5 );

                addSeparator();

                add<Field<3, Vector3>>("Field", "A 3D vector field", definedOn<Grid<3>>(Grid<3>::Points));
                add<double>("Step size", "The (initial) distance between each step.", 0.03);
                add<double>("e for adaptive step size", "number for calculating the new step size.", 0.02);
                add<InputChoices>("Method", "Choose calculation method.", std::vector<std::string>{"Euler", "Runge-Kutta"}, "Euler");
                
                addSeparator();

                add<Color>("Color Grid", "The color of the grid.", Color(0.25, 0.0, 0.25));
                add<Color>("Color Stream", "The color of the streamlines.", Color(0.5, 0.0, 0.0));
                add<size_t>("Number of steps", "Define a maximum number of points in the streamline", 100);
            }
        };

        struct VisOutputs : public VisAlgorithm::VisOutputs
        {
            VisOutputs(fantom::VisOutputs::Control &control)
                : VisAlgorithm::VisOutputs(control)
            {
                addGraphics("Streamlines");
                addGraphics( "Grid");
            }
        };

        IntegrateTask(InitData &data)
            : VisAlgorithm(data)
        {
        }

        // Euler function for the calculation for the points of the streamlines
        static void euler(double &h, double &x, double &y, double &z, double &e, std::vector<Point<3>> &points, std::shared_ptr<const Field<3, Vector3>> &field, size_t max_steps)
        {

            while (true)
            {
                if (points.size() >= max_steps)
                {
                    //std::cout << "Maximum number of points reached" << std::endl;
                    return;
                }

                Point3 p = {x, y, z};
                auto evaluator = field->makeEvaluator();

                // if the point is inside the domain
                if (evaluator->reset(p))
                {

                    //calculate value at this point
                    auto v = evaluator->value();

                    //std::cout << "Velocity at this point: " << v << std::endl;

                    //if there is no velocity at this point stop the loop
                    if (v[0] == 0 and v[1] == 0 and v[2] == 0){
                        return;
                    }

                    // normal point with normal h
                    double xN = x + h * v[0];
                    double yN = y + h * v[1];
                    double zN = z + h * v[2];

                    // calculate the dimensions together
                    double pN = abs(xN + yN + zN);

                    // helper point with half h
                    double xH = x + h / 2 * v[0];
                    double yH = y + h / 2 * v[1];
                    double zH = z + h / 2 * v[2];

                    Point3 helper = Point3(xH, yH, zH);
                    double pA;
                    double xA;
                    double yA;
                    double zA;

                    if (evaluator->reset(helper))
                    {
                        auto hv = evaluator->value();

                        // alternative point with twice half h
                        xA = xH + h / 2 * hv[0];
                        yA = yH + h / 2 * hv[1];
                        zA = zH + h / 2 * hv[2];

                        // calculate the dimensions together
                        pA = abs(xA + yA + zA);

                        //std::cout << "zweimal halbes h: " << xA << " " << yA << " " << zA << std::endl;
                    }

                    //std::cout << "normales h: " << xN << " " << yN << " " << zN << std::endl;

                    //std::cout << "Dimensionen addiert: " << pN << ", " << pA << "->" << (pN - pA) << std::endl;

                    // check difference between normal and alternative point
                    if (pN - pA > e)
                    {
                        h = h / 2;
                    }
                    else if (pN - pA < e / 2)
                    {
                        h = 2 * h;
                        points.push_back(p);
                        x = xN;
                        y = yN;
                        z = zN;
                    }
                    else if (pN - pA < e)
                    {
                        points.push_back(p);
                        x = xA;
                        y = yA;
                        z = zA;
                    }

                    //std::cout << "Neuer Punkt: " << x << " " << y << " " << z << std::endl;
                }

                // if the point is outside of the domain, we will stop adding more points to the streamline
                else
                {
                    //std::cout << "Point is outside of the domain" << std::endl;
                    return;
                }
            }
        }

        static void rungeKutta(double &h, double &x, double &y, double &z, std::vector<Point<3>> &points, std::shared_ptr<const Field<3, Vector3>> &field, size_t max_steps)
        {
            while (true)
            {
                if (points.size() >= max_steps)
                {
                    //std::cout << "Maximum number of points reached" << std::endl;
                    return;
                }

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

                    q1x = h * v1[0];
                    q1y = h * v1[1];
                    q1z = h * v1[2];

                    //std::cout << "Velocity at this point: " << v1[0] << std::endl;
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
                    q2x = h * v2[0];
                    q2y = h * v2[1];
                    q2z = h * v2[2];
                }
                if (evaluator->reset({x + 0.5 * q2x, y + 0.5 * q2y, z + 0.5 * q2z}))
                {
                    //calculate value at this point
                    auto v3 = evaluator->value();
                    q3x = h * v3[0];
                    q3y = h * v3[1];
                    q3z = h * v3[2];
                }
                if (evaluator->reset({x + 0.5 * q3x, y + 0.5 * q3y, z + 0.5 * q3z}))
                {
                    //calculate value at this point
                    auto v4 = evaluator->value();
                    q4x = h * v4[0];
                    q4y = h * v4[1];
                    q4z = h * v4[2];
                }

                // std::cout << "calculated q1: " << q1x << " " << q1y << " " << q1z << " " << q4x << std::endl;
                // std::cout << "calculated q2: " << q2x << " " << q2y << " " << q2z << " " << q4y << std::endl;
                // std::cout << "calculated q3: " << q3x << " " << q3y << " " << q3z << " " << q4z << std::endl;

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
        }

        static std::shared_ptr<graphics::Drawable> drawLines(std::vector<PointF<3>> pointsFList,std::vector<VectorF<3>> vertices, Color color)
        {
            auto const &system = graphics::GraphicsSystem::instance(); // The GraphicsSystem is needed to create Drawables, which represent the to be rendererd objects.
            auto bs = graphics::computeBoundingSphere(pointsFList);
            std::string resourcePath = PluginRegistrationService::getInstance().getResourcePath("utils/Graphics"); // path to the shaders

            std::shared_ptr<graphics::Drawable> graphic = system.makePrimitive(graphics::PrimitiveConfig{graphics::RenderPrimitives::LINES}
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
            size_t extent[] = { (size_t)options.get< long >( "nx" ),
                                (size_t)options.get< long >( "ny" ),
                                (size_t)options.get< long >( "nz" ) };
            double origin[] = { -0.5 * options.get< double >( "dx" ) * ( options.get< long >( "nx" ) - 1 ),
                                -0.5 * options.get< double >( "dy" ) * ( options.get< long >( "ny" ) - 1 ),
                                -0.5 * options.get< double >( "dz" ) * ( options.get< long >( "nz" ) - 1 ) };
            double spacing[] = { options.get< double >( "dx" ), options.get< double >( "dy" ), options.get< double >( "dz" ) };
  
            std::shared_ptr< const Grid< 3 > > grid = DomainFactory::makeUniformGrid( extent, origin, spacing );

            Color colorGrid = options.get<Color>("Color Grid");

            //--------------------------------------------------------------------------------------------------------------------

            //make grid where streamlines will start
            std::vector<VectorF<3>> verticesGrid;
            std::vector<PointF<3>> pointsFListGrid = {};
            const ValueArray< Point3 >& pointsGrid = grid->points();

            
            // for each cell in grid: 
            for (size_t i = 0; i < grid->numCells(); i++)
            {
                Cell cell = grid->cell( i );
                std::vector<size_t> connections = { 0,1,1,2,2,3,3,0,0,7,7,6,6,1,6,5,5,2,5,4,4,3,4,7 };

                // transform normal points list to PointF list
                for (size_t j = 0; j < 8; j++) 
                {
                    Point3 p = pointsGrid[cell.index( j )];
                    pointsFListGrid.push_back(PointF<3>(p[0], p[1], p[2]));

                    std::cout << "Point " << j << " of Cell " << i << ": " << p << std::endl;
                }

                // make vertices list with correct connections
                for (size_t k = 0; k < connections.size(); k++)
                {
                    Point3 p = pointsGrid[cell.index( connections[k] )];
                    verticesGrid.push_back(VectorF<3>(p));
                }
            }

            //--------------------------------------------------------------------------------------------------------------------

            // get all other options and check validity of them

            double h = options.get<double>("Step size");
            double e = options.get<double>("e for adaptive step size");
            std::string method = options.get<std::string>("Method");
            Color colorStream = options.get<Color>("Color Stream");
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
                    euler(h, x, y, z, e, points, field, max_steps);
                }

                if (method == "Runge-Kutta")
                {
                    rungeKutta(h, x, y, z, points, field, max_steps);
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