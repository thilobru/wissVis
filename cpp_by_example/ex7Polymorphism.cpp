#include <iostream>

/*
 * Polymorphism
 *
 * Important stuff:
 *     - know about virtual functions
 *     - know about references to base classes
 */

class Base
{
public:
    virtual ~Base() {} // need a virtual destructor to properly delete child objects

    virtual std::string getName() const
    {
        return "Base";
    }
};

class Child : public Base
{
public:
    std::string getName() const override
    {
        return "Child";
    }
};

void print( const Base& obj )
{
    std::cout << "Got an object of name " << obj.getName() << std::endl;
}

int main( int, char** )
{
    Base base;
    Child child;

    Base& test = child;

    print( base );
    print( child );
    print( test );

    // Quiz: what will be printed?

    return 0;
}
