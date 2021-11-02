import functools
from types import *
from math import sqrt
import fantom.math
import fantom.ui

# =============================================================
# utilities
# =============================================================

def colorize( text, color ):
    return u"\u001b[{}m{}\u001b[0m".format( color, text )


# =============================================================
# value comparison
# =============================================================

# dictionary containing all functions used to compare for equality
equal_functions = {}

def equal( lhs, rhs, **kwargs ):
    equal_function = equal_functions.get( ( type( lhs ), type( rhs ) ) )
    if equal_function:
        return equal_function( lhs, rhs, **kwargs )
    else:
        raise TypeError( "Invalid types given to equal function");

# booleans can simply be compared with ==
equal_functions[ ( bool, bool ) ] = lambda lhs, rhs: lhs == rhs

# integers can simply be compared with ==
equal_functions[ ( int, int ) ] = lambda lhs, rhs: lhs == rhs

# stirngs can simply be comppared with ==
equal_functions[ ( str, str ) ] = lambda lhs, rhs: lhs == rhs


# floating point numbers should be compared with a threshold
def equal_epsilon( lhs, rhs, **kwargs ):
    epsilon = kwargs.get( 'eps' ) or 1e-10
    return abs( lhs - rhs ) < epsilon

equal_functions[ ( float, float ) ] = equal_epsilon
equal_functions[ ( int, float ) ] = equal_epsilon
equal_functions[ ( float, int ) ] = equal_epsilon


# comparison of colors
def equal_color( lhs, rhs ):
    if ( lhs.r == rhs.r and lhs.g == rhs.g and lhs.b == rhs.b and lhs.a == rhs.a ):
        return True
    return False
    
equal_functions[ ( fantom.math.Color, fantom.math.Color ) ] = equal_color


# comparison of scalars
def equal_scalar( lhs, rhs ):
    if ( lhs == rhs ):
        return True
    return False

equal_functions[ ( fantom.math.Scalar, fantom.math.Scalar ) ] = equal_scalar


#comparison of vectors
def equal_vector( lhs, rhs, **kwargs ):
    if( len( lhs ) != len( rhs ) ):
        return False
    for x in range( len( lhs ) ):
        if not equal_epsilon( lhs.get( x ), rhs.get( x ), **kwargs ):
            return False
    return True
    
equal_functions[ ( fantom.math.Vector1, fantom.math.Vector1 ) ] = equal_vector
equal_functions[ ( fantom.math.Vector2, fantom.math.Vector2 ) ] = equal_vector
equal_functions[ ( fantom.math.Vector3, fantom.math.Vector3 ) ] = equal_vector
equal_functions[ ( fantom.math.Vector4, fantom.math.Vector4 ) ] = equal_vector

#comparison of matrices
def equal_matrix( lhs, rhs, **kwargs ):
    if( len( lhs ) != len( rhs ) ):
        return False
    for x in range( int( sqrt( len( lhs ) ) ) ):
        for y in range( int( sqrt( len( lhs ) ) ) ):
            if not equal_epsilon( lhs.get( x, y ), rhs.get( x, y ), **kwargs ):
                    return False
    return True

equal_functions[ ( fantom.math.Matrix1, fantom.math.Matrix1 ) ] = equal_matrix
equal_functions[ ( fantom.math.Matrix2, fantom.math.Matrix2 ) ] = equal_matrix
equal_functions[ ( fantom.math.Matrix3, fantom.math.Matrix3 ) ] = equal_matrix
equal_functions[ ( fantom.math.Matrix4, fantom.math.Matrix4 ) ] = equal_matrix



# =============================================================
# test framework
# =============================================================

test_functions = []

def test_function( func ):
    @functools.wraps( func )
    def wrapper( *args, **kargs ):
        return func( *args, **kargs )
    test_functions.append( wrapper )
    return wrapper

def run_tests():
    numberTests = 0
    numberPassedTests = 0
    
    for func in test_functions:
        numberTests += 1
        try:
            func()
            print( "{} Test:      {}".format( colorize( "PASSED", 32 ), func.__name__ ) )
            numberPassedTests += 1
        except AssertionError as e:
            print( "{} Test:      {}".format( colorize( "FAILED", 31 ), func.__name__ )  )       
            print( "       Assertion: {}".format( e ) )
            
    print( "{} out of {} Tests passed! ({:.2f}%)". format( numberPassedTests, numberTests, ( float( numberPassedTests ) / float( numberTests ) * 100 ) ) )
