#include <fantom/algorithm.hpp>
#include <fantom/dataset.hpp>
#include <fantom/register.hpp>

#include <random>
#include <vector>
#include <math.h>

using namespace fantom;
 
namespace
{
    class CommitTutorialAlgorithm : public DataAlgorithm
    {
 
    public:
        struct Options : public DataAlgorithm::Options
        {
            Options( fantom::Options::Control& control )
                : DataAlgorithm::Options( control )
            {
                add< int > ( "nHouses", "", 4);
                addSeparator();
                add< int >( "durchschnittHeight", "", 5 );
            }
        };
 
        struct DataOutputs : public DataAlgorithm::DataOutputs
        {
            DataOutputs( fantom::DataOutputs::Control& control )
                : DataAlgorithm::DataOutputs( control )
            {
                addBundle< const Grid< 3 > >( "settlement" );
            }
        };
 
 
        CommitTutorialAlgorithm( InitData& data )
            : DataAlgorithm( data )
        {
        }
        static void makeHouse(std::vector<Point<3>>& points,
                              std::vector<size_t>& indexes){
            int levels = 3;
            for(double i = 0; i < levels+1; i++) {
                points.push_back({0,i,0});
                points.push_back({1,i,0});
                points.push_back({1,i,1});
                points.push_back({0,i,1});
            }
            points.push_back({0.5, levels + 1.0, 0.5});
            points.push_back({0.5, levels + 2.0, 0.5});
            //Geschosse (Tetrahedron)
            for(int i = 0; i < levels; i++){
                for(int j = 0; j < 4; j++){
                    indexes.push_back(j+4*i);
                }
                for(int j = 0; j < 4; j++) {
                    indexes.push_back(7-j+4*i);
                }
            }
            //Dach (Pyramid)
            for(int i = points.size() - 5; i < points.size(); i++){
                indexes.push_back(i);
            }
        }
        virtual void execute( const Algorithm::Options& options, const volatile bool& /*abortFlag*/ ) override
        {
            int nHouses = options.get<int>("nHouses");
            std::vector < Point< 3 > > points = {};
            size_t numCellTypes = (size_t) 1;
            const std::pair< Cell::Type, size_t > cellCounts[] = {std::make_pair(Cell::Type::HEXAHEDRON, (size_t) 2)};
            std::vector< size_t > indexes = {};            

            std::shared_ptr< const Grid< 3 > > grid;
            std::shared_ptr< DataObjectBundle > bundle = std::make_shared< DataObjectBundle >();
            
            for(int i = 0; i < nHouses; i++) {
                makeHouse(points, indexes);
                grid = DomainFactory::makeGrid( points, numCellTypes, cellCounts, indexes);
                bundle->addContent(grid);
            }
            setResult("settlement", bundle);
        }
    };
 
    AlgorithmRegister< CommitTutorialAlgorithm > dummy( "Tasks/TaskTest", "Generate a simple settlement." );
}