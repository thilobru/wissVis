// #include <fantom/algorithm.hpp>
// #include <fantom/dataset.hpp>
// #include <fantom/register.hpp>
// #include <random>
// #include<array> 

// using namespace fantom;
 
// namespace
// {
//     class Task1 : public DataAlgorithm
//     {
 
//     public:
//         struct Options : public DataAlgorithm::Options
//         {
//             Options( fantom::Options::Control& control )
//                 : DataAlgorithm::Options( control )
//             {
//                 add< int >( "Number of Houses", "", 15 );
 
//                 addSeparator();
//             }
//         };
 
//         struct DataOutputs : public DataAlgorithm::DataOutputs
//         {
//             DataOutputs( fantom::DataOutputs::Control& control )
//                 : DataAlgorithm::DataOutputs( control )
//             {
//                 addBundle< const Grid< 3 > >( "grids" );
//                 //add< const Grid< 3 > >( "grid" );
//             }
//         };
 
 
//         Task1( InitData& data )
//             : DataAlgorithm( data )
//         {
//         }
 
//         bool duplicateTest(int *d, int *e, int j) {
//             std::cout << "xval:" << std::endl;
//             std::cout << "\t[";
//             for( int i=0; i<=j; ++i )
//             {
//                 std::cout << " " << d[i];
//             }
//             std::cout << " ]" << std::endl;

//             std::cout << "zval:" << std::endl;
//             std::cout << "\t[";
//             for( int i=0; i<=j; ++i )
//             {
//                 std::cout << " " << e[i];
//             }
//             std::cout << " ]" << std::endl;

//             std::cout << " j: " << j << std::endl;

//             int length = j+1;
//             for (int i = 0; i < length; ++i)
//                 for (int l = i + 1; l < length; ++l)
//                     if (d[i] == d[l] ||  e[i] == e[l]){ //d[i]+1 == d[l] || d[i] == d[l]+1 ||  d[i]-1 == d[l] || d[i] == d[l]-1) && (e[i] == e[l] || e[i]+1 == e[l] || e[i] == e[l]+1 || e[i]-1 == e[l] || e[i] == e[l]-1)) {
//                         return true;
//                     }
//             return false;
//         }
//         virtual void execute( const Algorithm::Options& options, const volatile bool& /*abortFlag*/ ) override
//         {
//             int n = options.get< int >("Number of Houses");
//             int xval[n];
//             int height[n];
//             int zval[n];

//             int c;
//             for(int j = 0; j != n; ++j){
//                 int MIN = -n;
//                 int MAX = n;

//                 std::random_device rd;
//                 std::default_random_engine eng(rd());
//                 std::uniform_int_distribution<int> distr(MIN, MAX);

//                 xval[j] = distr(eng);
//                 zval[j] = distr(eng);


//                 MIN = 1;
//                 MAX = 10;
//                 std::uniform_int_distribution<int> distri(MIN, MAX);
//                 height[j] = distri(eng);                
                
//                 c=j;
//                 if(duplicateTest(xval, zval, c) == true){
//                     std::cout << "duplicate!" << std::endl;
//                     //std::cout << " j: " << j << std::endl;
//                     --j;
//                     //std::cout << " j: " << j << std::endl;
//                 }
//             }
//             std::shared_ptr< DataObjectBundle > bundle = std::make_shared< DataObjectBundle >();
 
            

//             for(int i = 0; i != n; ++i){
//                 size_t extent[] = { (size_t)2,
//                                     (size_t)height[i],
//                                     (size_t)2 };
//                 double origin[] = { xval[i],
//                                     0,
//                                     zval[i]};
//                 double spacing[]
//                     = { (double)1, (double)1.5, (double)1 };

//                 auto grid = DomainFactory::makeUniformGrid( extent, origin, spacing );
                
//                 std::cout << "origin:" << std::endl;
//                 std::cout << "\t[";
//                 for( int i=0; i<3; ++i )
//                 {
//                     std::cout << " " << origin[i];
//                 }
//                 std::cout << " ]" << std::endl;


//                 bundle->addContent( grid );
//             }
//             setResult( "grids", bundle );
//         }
//     };
 
//     AlgorithmRegister< Task1 > dummy( "Tasks/House", "Select number of Houses" );
// }
