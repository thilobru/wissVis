#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec4 clippingPlane[6];

in vec3 in_vertex;

out clipDists_t
{
    float clipDistance[6];
};

void main( void )
{
    gl_Position = proj * view * model * vec4( in_vertex, 1.0 );
    
    vec4 gPosWorld =  model * vec4( in_vertex, 1.0 );
    for( int planeId = 0; planeId < 6; ++planeId )
    {
        clipDistance[ planeId ] = dot( gPosWorld, clippingPlane[ planeId ] );
    }
}
