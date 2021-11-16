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
                add< int > ( "nHouses", "", 1);
                addSeparator();
                add< int >( "nlevels", "", 5 );
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
        static void makeCoords(std::vector<std::vector<double>>& coords,
                               int& nHouses){
            std::random_device dev;
            std::mt19937 rng(dev());
            std::uniform_int_distribution<std::mt19937::result_type> dist(-(std::sqrt(nHouses)),std::sqrt(nHouses)); // distribution in range [-anzahlHäuser/2, anzahlHäuser/2]
            std::cout << dist(rng) << std::endl;

            for(int i = 0; i < nHouses; i++) {
                redo:;
                std::vector <double> v1;
                for(short j = 0; j < 2; j++) {
                    v1.push_back(dist(rng));
                }
                for(int j = 0; j < (int) coords.size()-1; j++) {
                    if(v1[0] == coords[j][0] && v1[1] == coords[j][1]){
                        goto redo;
                    }
                }
                coords.push_back(v1);
            }
        }
        static void makeHouse(std::vector<Point<3>>& points,
                              std::vector<size_t>& indexes,
                              int& levels,
                              std::vector<double>& coords){
            for(double i = 0; i < levels+1; i++) {
                points.push_back({coords[0]    ,i,coords[1]});
                points.push_back({coords[0] + 1,i,coords[1]});
                points.push_back({coords[0] + 1,i,coords[1] + 1});
                points.push_back({coords[0]    ,i,coords[1] + 1});
            }
            points.push_back({coords[0] + 0.5, levels + 1.0, coords[1] + 0.5});
            points.push_back({coords[0] + 0.5, levels + 2.0, coords[1] + 0.5});
            //Geschosse (Tetrahedron)
            for(int i = 0; i < levels; i++){
                for(int j = 0; j < 4; j++){
                    indexes.push_back(j+4*i);
                }
                for(int j = 0; j < 4; j++) {
                    indexes.push_back(7-j+4*i);
                }
            }
            // Dach (Pyramid)
            for(int i = points.size() - 6; i < (int) points.size() - 1; i++){
                indexes.push_back(i);
            }
            // Antenne (Line)
            for(int i = points.size() - 2; i < (int) points.size(); i++){
                indexes.push_back(i);
            }
        }
        virtual void execute( const Algorithm::Options& options, const volatile bool& /*abortFlag*/ ) override
        {
            int nHouses = options.get<int>("nHouses");
            int levels = options.get<int>("nlevels");
            std::vector<std::vector<double>> coords;
            makeCoords(coords, nHouses);

            std::shared_ptr< const Grid< 3 > > grid;
            std::shared_ptr< DataObjectBundle > bundle = std::make_shared< DataObjectBundle >();
            for(int i = 0; i < nHouses; i++) {
                std::vector < Point< 3 > > points;
                size_t numCellTypes = (size_t) 3;
                const std::pair< Cell::Type, size_t > cellCounts[] 
                            = {std::make_pair(Cell::Type::HEXAHEDRON, (size_t) levels), 
                               std::make_pair(Cell::Type::PYRAMID, (size_t) 1),
                               std::make_pair(Cell::Type::LINE, (size_t) 1)};
                std::vector< size_t > indexes;
                makeHouse(points, indexes, levels, coords[i]);
                infoLog() << "Number of Points: " << points.size() << std::endl;
                infoLog() << "Number of Indices: " << indexes.size() << std::endl;
                grid = DomainFactory::makeGrid( points, numCellTypes, cellCounts, indexes);
                bundle->addContent(grid);
            }
            setResult("settlement", bundle);
        }
    };
 
    AlgorithmRegister< CommitTutorialAlgorithm > dummy( "Tasks/TaskTest", "Generate a simple settlement." );
}