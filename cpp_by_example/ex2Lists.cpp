#include <iostream> // needed for printing
#include <vector>   // needed for container

/*
 * encapsulate data of same type
 *
 * Important stuff:
 *     - usage of containers
 *     - usage of templates
 */

int main( int, char** )
{
    // std::vector is a template:  template< typename T > class std::vector< T >;
    // use it, by specifying a TYPE in angle brackets
    // meaning: declare a list of ints
        
    std::vector< int > list1( 5 /* length */, -1 /* default value of elements */ );
    
    // Quiz: why no "new"?
    // Quiz: where is the data? Heap or stack?


    std::cout << "Created a list of size " << list1.size() << std::endl;

    std::cout << "Printing out the list:" << std::endl;
    std::cout << "\t[";
    for( int i : list1 )
    {
        std::cout << " " << i;
    }
    std::cout << " ]" << std::endl;

    std::vector< int > list2; // empty list
    for( int i = 0; i < 5; ++i ) // growing the list with elements
    {
        list2.push_back( -1 ); // note: no "new"!
    }
    

    std::cout << "Comparing lists: " << ( list1 == list2 ? "equal" : "not equal" ) << std::endl;

    return 0;
}
