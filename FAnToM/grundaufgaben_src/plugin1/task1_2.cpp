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
                add< int >( "durchschnittHöhe", "", 5 );
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
 
        virtual void execute( const Algorithm::Options& options, const volatile bool& /*abortFlag*/ ) override
        {
            int nHouses = options.get<int>("nHouses");

            std::random_device dev;
            std::mt19937 rng(dev());
            std::uniform_int_distribution<std::mt19937::result_type> dist(-(std::sqrt(nHouses)),std::sqrt(nHouses)); // distribution in range [-anzahlHäuser/2, anzahlHäuser/2]
            std::cout << dist(rng) << std::endl;

            std::vector<std::vector<short>> coords;
            
            for(int i = 0; i < nHouses; i++) {
                std::vector <short> v1;
                for(short j = 0; j < 2; j++) {
                    v1.push_back(dist(rng));
                }
                coords.push_back(v1);
            }

            size_t extent[] = { (size_t)2,
                                (size_t)10,
                                (size_t)2};
            double origin[] = { -0.5,
                                -0.5,
                                -0.5};
            double spacing[]= {1,1,1};

            std::shared_ptr< const Grid< 3 > > grid;
            std::shared_ptr< DataObjectBundle > bundle = std::make_shared< DataObjectBundle >();
            for(int i = 0; i < nHouses; i++) {
                double pOrigin[] = {origin[0] + coords[i][0], 
                                    origin[1], 
                                    origin[2] + coords[i][1]};
                grid = DomainFactory::makeUniformGrid( extent, pOrigin, spacing );
                bundle->addContent(grid);
            }
            setResult("settlement", bundle);
        }
    };
 
    AlgorithmRegister< CommitTutorialAlgorithm > dummy( "Tasks/Task1_2", "Generate a simple settlement." );
}