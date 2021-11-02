#include <iostream>
#include <memory>

/*
 * Memory management
 *
 * Important stuff:
 *     - know difference between heap and stack
 *     - know about scopes and destructors
 *     - know about shared_ptr
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

        // Quiz: what will be printed?
    } // Quiz: what happens with childOnStack, objectOnHeap and objectOnHeap2 now?


















    // Answer: all objects are destroyed as they go out of scope.
    // End of scope is end of lifetime! Unlike Java! -> no garbage collection needed!
    //
    // Answer: objectOnHeap2 is leaked, because we forgot to write a delete! When pointer is destructed, only pointer goes away, not target!


    {
        Base* objectOnHeap2 = new Child();

        delete objectOnHeap2;

        print( *objectOnHeap2 ); // Quiz: what happens?
    }

    // IMPORTANT RULE OF THUMB:
    // NEVER EVER USE RAW new AND delete
    // Always use smart pointers, e.g., std::shared_ptr and std::unique_ptr
    // together with std::make_shared and std::make_unique (C++14)

    return 0;
}
