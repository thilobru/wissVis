#include <iostream> // needed for printing
#include <vector>   // needed for collection

/*
 * functions
 *
 * Important stuff:
 *     - definition of a function
 */

void printList( const std::vector<int>& list, const std::string& name )
{
    std::cout << "Printing out the list " << name << std::endl;
    std::cout << "\t[";
    for( auto& i : list )
    {
        std::cout << " " << i;
    }
    std::cout << " ]" << std::endl;
}

int main( int, char** )
{
    std::vector< int > list1( 5 /* length */, -1 /* default value of elements */ );
    std::vector< int > list2; // empty list
    for( int i = -2; i < 5; ++i ) // growing the list with elements
    {
        list2.push_back( i ); // note: no "new"!
    }

    printList( list1, "List 1" );
    printList( list2, "List 2" );
    // Quiz: how are the arguments passed to the function?

    std::cout << "Comparing lists: " << ( list1 == list2 ? "equal" : "not equal" ) << std::endl;

    return 0;
}
