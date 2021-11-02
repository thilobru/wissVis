#include <iostream>

/*
 * custom objects
 *
 * Important stuff:
 *     - definition of a class
 *     - definition of custom operators
 */

class Vector
{
public:
    Vector() // default constructor (can be skipped)
        : mX( -1 ) // initialization of data members
        , mY( 2 ) // by calling their "constructors"
    {
        // Note: this is a bad constructor.
        // An empty constructor should create a null vector!
    }

    Vector( double x, double y )
        : mX( x )
        , mY( y )
    {
    }

    Vector( const Vector& rhs ) // copy constructor
        : mX( rhs.mX )
        , mY( rhs.mY )
    {
    }

    // accessor functions ---------------------------------------

    double x() const // Quiz: what does const mean here?
    {
        return mX;
    }

    double y() const
    {
        return mY;
    }

    // operators -----------------------------------------------
    
    Vector operator+( const Vector& rhs ) const
    {
        return Vector( mX + rhs.mX, mY + rhs.mY );
    }

    Vector& operator=( const Vector& rhs ) // Quiz: why NOT const??
    {
        mX = rhs.mX;
        mY = rhs.mY;
        return *this;
    }

private:
    double mX; // good practice: naming convention for data members
    double mY;
};

double operator*( const Vector& v1, const Vector& v2 )
{
    double result = 0;
    result += v1.x() * v2.x();
    result += v1.y() * v2.y();
    return result;
}


int main( int, char** )
{
    Vector v1;
    Vector v2( 2, 3 );
    Vector v3 = v1 + v2;

    auto scalarProduct = v2 * v3;
    // Quiz: what type does scalarProduct have?

    std::cout << "v2*v3 = " << scalarProduct << std::endl;

    return 0;
}
