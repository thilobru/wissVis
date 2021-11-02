#include <iostream>
#include <memory>
#include <stdexcept>

/*
 * Exceptions
 *
 * Important stuff:
 *     - know how to throw and catch exceptions
 *     - know about exception safety and stack unrolling
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

std::shared_ptr< Base > makeObject()
{
    return std::make_shared< Child >( /* constructor arguments */ );
}

void print( const Base& obj )
{
    static int i = 3;
    if( i == 0 )
    {
        throw std::runtime_error( "Planned obsolescence" );
    }
    else
    {
        --i;
    }
    std::cout << "Got an object of name " << obj.getName() << std::endl;
}

int main( int, char** )
{
    Base baseOnStack;
    {
        Child childOnStack;
        auto objectOnHeap = makeObject();
        Base* objectOnHeap2 = new Child();

        print( childOnStack );
        print( *objectOnHeap );
        print( *objectOnHeap2 );

        delete objectOnHeap2;
    }
    // Quiz: do we have a memory leak? What leaked?













    // Answer: objectOnHeap2 is leaked, because we exception is thrown before delete
    // Answer: objects on stack are not leaked, because they go out of scope and are destroyed
    // Answer: objectOnHeap is NOT leaked, because smart pointer deletes object when it goes out of scope



    // IMPORTANT RULE OF THUMB:
    // NEVER EVER USE RAW new AND delete
    // Always use smart pointers, e.g., std::shared_ptr and std::unique_ptr
    // together with std::make_shared and std::make_unique (C++14)





    // catching exceptions
    Child childOnStack; // Quiz: why no name collision?

    try
    {
        print( childOnStack );
    }
    catch( std::exception& e )
    {
        std::cout << "we got an exception: " << e.what() << std::endl;
    }


    return 0;
}
