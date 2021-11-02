#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

#include "Vector.hpp"

/*
 * Algorithms and lambdas
 *
 * Important stuff:
 *     - know about std::algorithms
 *     - know how to pass a function
 *     - know how to define a function inline (lambda)
 */

bool isNegative( const Vector& v )
{
    return v.x() < 0 || v.y() < 0;
}

int main( int, char** )
{
    // randomness... ignore this for now...
    std::default_random_engine gen( 32534 /* seed */ );
    std::uniform_real_distribution< double > dist( -5, 20 );

    std::vector< Vector > list;
    for( int i=0; i < 100; ++i )
    {
        list.push_back( Vector( dist(gen), dist(gen) ) ); // create random vectors
    }


    // count null vectors
    double nulls = std::count( list.begin(), list.end(), Vector(0, 0) );

    // Quiz: are there other ways to achieve this?







    // count negative vectors
    double negs1 = std::count_if( list.begin(), list.end(), isNegative );

    // count negative vectors (inline)
    double negs2 = std::count_if( list.begin(), list.end(), []( const Vector& v ) -> bool
            {
                return v.x() < 0 || v.y() < 0;
            } );

    std::cout << "we have " << nulls << " null vectors and " << negs1 << " negative vectors" << std::endl;
    if( negs1 != negs2 )
    {
        std::cout << "and we have an error in our code!" << std::endl;
    }

    // IMPORTANT RULE OF THUMB:
    // NEVER WRITE RAW LOOPS! There is a std::algorithm for almost anything that you can think of.
    // Advantage of std::algorithm: bug free and declarative

    return 0;
}
