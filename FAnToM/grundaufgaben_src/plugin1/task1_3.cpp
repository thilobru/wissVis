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
        static void makeHouses(int nHouses,
                                std::vector<std::vector<short>>& coords,
                                std::vector<std::vector<std::vector<int>>>& housesPoints){
            int levels = 6;//rand(10);
            std::vector<std::vector<std::vector<int>>> housesPoints;
            for(int h; h < nHouses; h++){
                std::vector<std::vector<int>> housePoints;
                for(int i; i > levels; i++){
                    std::vector<int> v1;
                    for(int j; j < 3; j++){
                        v1.push_back(coords[h][0] + j);
                        v1.push_back(coords[h][1] + j);
                        v1.push_back(coords[h][2] + j);
                    }
                    housePoints.push_back(v1);
                }
                housesPoints.push_back(housePoints);
            }
        }
        virtual void execute( const Algorithm::Options& options, const volatile bool& /*abortFlag*/ ) override
        {
            // bool duplicate ; //todo
            int nHouses = options.get<int>("nHouses");

            std::random_device dev;
            std::mt19937 rng(dev());
            std::uniform_int_distribution<std::mt19937::result_type> dist(-(std::sqrt(nHouses)),std::sqrt(nHouses)); // distribution in range [-anzahlHäuser/2, anzahlHäuser/2]
            std::cout << dist(rng) << std::endl;

            std::vector<std::vector<short>> coords;
            
            for(int i = 0; i < nHouses; i++) {
                redo:;
                std::vector <short> v1;
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

            double origin[] = { -0.5,
                                -0.5,
                                -0.5};

            std::shared_ptr< const Grid< 3 > > grid;
            std::shared_ptr< DataObjectBundle > bundle = std::make_shared< DataObjectBundle >();
            for(int i = 0; i < nHouses; i++) {
                double pOrigin[] = {origin[0] + coords[i][0], 
                                    origin[1], 
                                    origin[2] + coords[i][1]};

                std::vector< Point < 3 >> pointset;
                // ValueArray<points> points;
                // points = {1,2,3};
                // pointset = DomainFactory::makePointSet(& points);
                

                grid = DomainFactory::makeGrid( housesPoints, numCellTypes, cellCounts, indices);
                bundle->addContent(grid);
            }
            setResult("settlement", bundle);
        }
    };
 
    AlgorithmRegister< CommitTutorialAlgorithm > dummy( "Tasks/Task1_3", "Generate a simple settlement." );
}