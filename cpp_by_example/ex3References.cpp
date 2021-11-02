#include <iostream> // needed for printing

/*
 * references
 *
 * Important stuff:
 *     - existence and meaning of references
 *     - constness
 */

int main( int, char** )
{
    int i = 2;
    int j = i;
    int& k = i;

    std::cout << "i: " << i << " j: " << j << " k: " << k << std::endl;

    i = 1;
    j = 2;
    k = 3;

    // Quiz: what values do  i, j, and k have?
     std::cout << "i: " << i << " j: " << j << " k: " << k << std::endl;

    





























    // Answer: j is a COPY of i, k is an ALIAS of i
    





    // Quiz: does this compile?
     const int& l = i;
     std::cout << "l: " << l << " j: " << j << " i: " << i << std::endl;
     //l = 4;

    return 0;
}
