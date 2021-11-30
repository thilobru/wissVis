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
                add< int >( "nlevels", "", 5 );
            }
        };
 
        struct DataOutputs : public DataAlgorithm::DataOutputs
        {
            DataOutputs( fantom::DataOutputs::Control& control )
                : DataAlgorithm::DataOutputs( control )
            {
                add< const Grid< 3 > >( "settlement" );
            }
        };
 
 
        CommitTutorialAlgorithm( InitData& data )
            : DataAlgorithm( data )
        {
        }
        static void makeCoords(std::vector<std::vector<double>>& coords,
                               int& nHouses){
            std::random_device dev;
            std::default_random_engine rng(dev());
            std::uniform_int_distribution<int> dist(-(std::sqrt(nHouses)*3),std::sqrt(nHouses) * 3); // distribution in range [-anzahlHäuser/2, anzahlHäuser/2]
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
        static void makeHouse(std::shared_ptr< const Grid< 3 > >& grid,
                              int& levels,
                              std::vector<double>& coords){
            std::vector < Point< 3 > > points = {};
            const std::pair< Cell::Type, size_t > cellCounts[] 
                        = {std::make_pair(Cell::Type::HEXAHEDRON, (size_t) levels), 
                            std::make_pair(Cell::Type::PYRAMID, (size_t) 1),
                            std::make_pair(Cell::Type::LINE, (size_t) 1)};
            size_t numCellTypes = (size_t) 3;
            std::vector< size_t > indexes = {};
            
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
            grid = DomainFactory::makeGrid( points, numCellTypes, cellCounts, indexes);
        }
        virtual void execute( const Algorithm::Options& options, const volatile bool& /*abortFlag*/ ) override
        {
            int nHouses = options.get<int>("nHouses");
            int levels = options.get<int>("nlevels");
            std::shared_ptr< const Grid< 3 > > grid;
            std::vector<std::vector<double>> coords;
            makeCoords(coords, nHouses);
            for(int i = 0; i < nHouses; i++) {
                makeHouse(grid, levels, coords[i]);
            }
            setResult("settlement", grid);
        }
    };
 
    AlgorithmRegister< CommitTutorialAlgorithm > dummy( "Tasks/TaskFinalFinal", "Generate a simple settlement." );
}