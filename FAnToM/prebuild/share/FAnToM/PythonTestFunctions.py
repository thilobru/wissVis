from types import *

def equalTestDouble( double1, double2, epsilon ):
    assert type( double1 ) is float, "" 
    assert type( double2 ) is float, "" 
    return abs( double2 - double1 ) < epsilon

def equalTestVector1(vector1, vector2,epsilon):
    assert type(vector1) is fantom.math.Vector1, "The 1st parameter isn't a fantom.math.Vector1"
    assert type(vector2) is fantom.math.Vector1, "The 2nd parameter isn't a fantom.math.Vector1"
    assert type(epsilon) is float, "The 3rd parameter isn't a float"
    result= (vector1.get(0) + epsilon >= vector2.get(0)) and (vector1.get(0) - epsilon <= vector2.get(0))
    return result
    
def equalTestVector2(vector1, vector2,epsilon):
    assert type(vector1) is fantom.math.Vector2, "The 1st parameter isn't a fantom.math.Vector2"
    assert type(vector2) is fantom.math.Vector2, "The 2nd parameter isn't a fantom.math.Vector2"
    assert type(epsilon) is float, "The 3rd parameter isn't a float"
    result= True
    for x in range(0,2):
        result= result and ((vector1.get(x) + epsilon >= vector2.get(x)) and (vector1.get(x) - epsilon <= vector2.get(x)))
        #print "+epsilon"
        #print (vector1.get(x) + epsilon >= vector2.get(x))
        #print "-epsilon"
        #print (vector1.get(x) - epsilon <= vector2.get(x))        
    return result
    
def equalTestVector3(vector1, vector2,epsilon):
    assert type(vector1) is fantom.math.Vector3, "The 1st parameter isn't a fantom.math.Vector3"
    assert type(vector2) is fantom.math.Vector3, "The 2nd parameter isn't a fantom.math.Vector3"
    assert type(epsilon) is float, "The 3rd parameter isn't a float"
    result= True
    for x in range(0,3):
        result= result and (vector1.get(x) + epsilon >= vector2.get(x)) and (vector1.get(x) - epsilon <= vector2.get(x))
    return result

def equalTestVector4(vector1, vector2,epsilon):
    assert type(vector1) is fantom.math.Vector4, "The 1st parameter isn't a fantom.math.Vector4"
    assert type(vector2) is fantom.math.Vector4, "The 2nd parameter isn't a fantom.math.Vector4"
    assert type(epsilon) is float, "The 3rd parameter isn't a float"
    result= True
    for x in range(0,4):
        result= result and (vector1.get(x) + epsilon >= vector2.get(x)) and (vector1.get(x) - epsilon <= vector2.get(x))
    return result
    
def equalTestMatrix1(matrix1, matrix2, epsilon):
    assert type(matrix1) is fantom.math.Matrix1, "The 1st parametert isn't a fantom.math.Matrix1"
    assert type(matrix2) is fantom.math.Matrix1, "The 1st parametert isn't a fantom.math.Matrix1"
    assert type(epsilon) is float, "The 3rd parameter isn't a float"
    result= (matrix1.get(0,0)+ epsilon >= matrix2.get(0,0)) and (matrix1.get(0,0) - epsilon <= matrix2.get(0,0))
    return result

def equalTestMatrix2(matrix1, matrix2, epsilon):
    assert type(matrix1) is fantom.math.Matrix2, "The 1st parametert isn't a fantom.math.Matrix2"
    assert type(matrix2) is fantom.math.Matrix2, "The 1st parametert isn't a fantom.math.Matrix2"
    assert type(epsilon) is float, "The 3rd parameter isn't a float"
    result= True
    for x in range(0,2):
        for y in range(0,2):
            result= result and ((matrix1.get(x,y) + epsilon >= matrix2.get(x,y)) and (matrix1.get(x,y) - epsilon <= matrix2.get(x,y)))
    return result

def equalTestMatrix3(matrix1, matrix2, epsilon):
    assert type(matrix1) is fantom.math.Matrix3, "The 1st parametert isn't a fantom.math.Matrix3"
    assert type(matrix2) is fantom.math.Matrix3, "The 1st parametert isn't a fantom.math.Matrix3"
    assert type(epsilon) is float, "The 3rd parameter isn't a float"
    result= True
    for x in range(0,3):
        for y in range(0,3):
           result= result and ((matrix1[x][y] + epsilon >= matrix2[x][y]) and (matrix1[x][y] - epsilon <= matrix2[x][y]))
    return result

def equalTestMatrix4(matrix1, matrix2, epsilon):
    assert type(matrix1) is fantom.math.Matrix4, "The 1st parametert isn't a fantom.math.Matrix4"
    assert type(matrix2) is fantom.math.Matrix4, "The 1st parametert isn't a fantom.math.Matrix4"
    assert type(epsilon) is float, "The 3rd parameter isn't a float"
    result= True
    for x in range(0,4):
        for y in range(0,4):
            result= result and ((matrix1[x][y] + epsilon >= matrix2[x][y]) and (matrix1[x][y] - epsilon <= matrix2[x][y]))
    return result

def testFunction( name, function, expectedResult, equalFunction = None ):
    if ( equalFunction and equalFunction( function(), expectedResult ) ) or function() == expectedResult:
        print( "test_" + str( name ) + " ... ok" )     
    else:
        print( "test_" + str( name ) + " ... failed" )

def testConstructor(name,classType,constructorLambda):
    assert type(name) is StringType, "The 1st parameter isn't a string!"
    assert isinstance(classType,type), "The 2nd parameter isn't a class."
    assert type(constructorLambda) is LambdaType,"The 3rd parameter isn't a lambda, which calls a constructor!"
    try:
        element = constructorLambda()
        assert element is not None
        assert element.__class__.__name__ == classType.__name__
        print "test_"+str(name)+"_constructor ... ok"
    except AssertionError:
        print "test_"+str(name)+"_constructor ... failed"
