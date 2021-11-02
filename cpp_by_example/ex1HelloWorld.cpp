#include <iostream> // needed for printing
#include <string>   // needed for string operations

/*
 * Hello World application.
 *
 * Important stuff:
 *     - what is an object?
 *     - usage of objects
 *     - operators for objects
 *     - printing objects
 */

int main( int, char** )
{

//    https://en.cppreference.com/w/
    
    // simple printing
    std::cout << "Hello World!" << std::endl;

    // creation of simple objects
    
        std::string output = "Hello"; // via assignment
        std::string output2( "World!" ); // via constructor
    
    
    // Quiz: why no "new"?
    // Quiz: where is the data located? Heap or stack?

    // Printing the objects to the console
    std::cout << output << " " << output2 << std::endl;

    // Concatenating strings
    std::string fullOutput = output + " " + output2; // Note: no function. Just operators.
    std::cout << fullOutput << std::endl;

    return 0;
}
