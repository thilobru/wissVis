#include <fantom/algorithm.hpp>
#include <fantom/dataset.hpp>
#include <fantom/register.hpp>
#include <map>
 
#define OPTION_NUMBEROFHOUSES "Number of Houses"

using namespace fantom;
 
namespace
{
    class CommitTutorialAlgorithm : public DataAlgorithm
    {
 
    public:
        struct Options : public DataAlgorithm::Options
        {
            //make options and change number if too small
            Options( fantom::Options::Control& control )
                : DataAlgorithm::Options( control )
            {
                addTab( "Options" );
                add< int >( OPTION_NUMBEROFHOUSES, "The number of houses in the city.", 1, &acceptNumber );
            }
            
            static int acceptNumber( const int& i )
            {
                return std::max( i, 1 );
            }
        };

        //define type of output
        struct DataOutputs : public DataAlgorithm::DataOutputs
        {
            DataOutputs( fantom::DataOutputs::Control& control )
                : DataAlgorithm::DataOutputs( control )
            {
                add< const Grid< 3 > >( "grid" );
            }
        };
 
        //constructor
        CommitTutorialAlgorithm( InitData& data )
            : DataAlgorithm( data )
        {
        }

        //create a house level by level
        static void createHouse(int x, int y, int z, int dist, std::vector < Point< 3 > >& points, std::vector< size_t >& indices) //, std::map <std::vector<int>, int>& allPointsMap)
        {
            int levels = 1; //rand() % 7;
            std::map <std::vector <int>, int > allPointsMap = {};
            std::cout << "house";
            for(int level = 0; level < levels; level++)
            {
                createCube(x, y+level, z, dist, points, indices, allPointsMap);
            }
        }

        //create the cube for the given level
        static void createCube(int x, int y, int z, int dist, std::vector < Point< 3 > >& points, std::vector< size_t >& indices, std::map <std::vector<int>, int >& allPointsMap)
        {
            std::vector <int> p0 = { x, y, z };
            std::vector <int> p1 = { x+dist, y, z };
            std::vector <int> p2 = { x+dist, y, z+dist };
            std::vector <int> p3 = { x, y, z+dist };
            std::vector <int> p4 = { x, y+dist, z+dist };
            std::vector <int> p5 = { x+dist, y+dist, z+dist };
            std::vector <int> p6 = { x+dist, y+dist, z };
            std::vector <int> p7 = { x, y+dist, z };

            std::cout << "cube";

            std::vector < std::vector<int>> newPoints = {p0, p1, p2, p3, p4, p5, p6, p7};
            for(size_t i = 0; i < newPoints.size(); ++i) 
            {
                Point <3> p (newPoints[i][0], newPoints[i][1], newPoints[i][2]);
                //std::cout << "Size of map: " << allPointsMap.size();          
                //if new point is not yet in map: create p, add to points and to map
                if (allPointsMap.find(newPoints[i]) == allPointsMap.end())
                {
                    Point <3> p (newPoints[i][0], newPoints[i][1], newPoints[i][2]);
                    std::cout<<"not in map yet: " << p<< std::endl;

                    points.push_back(p);
                    allPointsMap.insert({newPoints[i], points.size()-1});

                    std::cout << "Size of points: " << points.size() << std::endl;

                    indices.push_back(points.size()-1);
                }
                // if new point is already in map: add old index again
                else
                {
                    std::cout<< "already in map: " << p << std::endl;
                    indices.push_back(allPointsMap[newPoints[i]]);
                } 
            }
        }
 
        //main function
        virtual void execute( const Algorithm::Options& options, const volatile bool& /*abortFlag*/ ) override
        {
            //write infolog with selected number of houses
            int numHouses = options.get< int >( OPTION_NUMBEROFHOUSES );
            infoLog() << "Number " << numHouses << std::endl;

            std::cout << "execute";

            //define characteristics of the grid
            std::vector < Point< 3 > > points = {};
            size_t numCellTypes = (size_t) 1;
            const std::pair< Cell::Type, size_t > cellCounts[] = {std::make_pair(Cell::Type::HEXAHEDRON, (size_t) 8)};
            std::vector< size_t > indices = {};            

            //create house and add points to the points-array
            int x = 0;
            int y = 0;
            int z = 0;
            int dist = 1;

            for( int i = 0; i < numHouses; i++ )
            {
                std::vector<size_t> newIndices;
                createHouse(x, y, z, dist, points, indices); //, allPointsMap);

                x = x+2;
            }

            infoLog() << "Number of Points: " << points.size() << std::endl;
            infoLog() << "Number of Indices: " << indices.size() << std::endl;

            for (size_t i:indices)
            {
            infoLog() << i << std::endl;
            }
            
            //create grid with positions of points and their relation
            std::shared_ptr< const Grid< 3 > > grid = DomainFactory::makeGrid( points, numCellTypes, cellCounts, indices );

            //announce the field, usable for other code
            setResult( "grid", grid );
        }
    };
    AlgorithmRegister< CommitTutorialAlgorithm > dummy( "Aufgaben/Build City 3", "Generates the houses after choosing the number of houses");
}