#pragma once // only include this file once in a compilation unit

class Vector
{
public:
    inline Vector() // default constructor (can be skipped)
        : mX( -1 ) // initialization of data members
        , mY( 2 ) // by calling their "constructors"
    {
        // Note: this is a bad constructor.
        // An empty constructor should create a null vector!
    }

    inline Vector( double x, double y )
        : mX( x )
        , mY( y )
    {
    }

    inline Vector( const Vector& rhs ) // copy constructor
        : mX( rhs.mX )
        , mY( rhs.mY )
    {
    }

    // accessor functions ---------------------------------------

    inline double x() const // Quiz: what does const mean here?
    {
        return mX;
    }

    inline double y() const
    {
        return mY;
    }

    // operators -----------------------------------------------
    
    inline Vector operator+( const Vector& rhs ) const
    {
        return Vector( mX + rhs.mX, mY + rhs.mY );
    }

    inline Vector& operator=( const Vector& rhs )
    {
        mX = rhs.mX;
        mY = rhs.mY;
        return *this;
    }

    inline bool operator==( const Vector& rhs ) const
    {
        return mX == rhs.mX && mY == rhs.mY;
    }

private:
    double mX; // good practice: naming convention for data members
    double mY;
};

inline double operator*( const Vector& v1, const Vector& v2 )
{
    double result = 0;
    result += v1.x() * v2.x();
    result += v1.y() * v2.y();
    return result;
}
