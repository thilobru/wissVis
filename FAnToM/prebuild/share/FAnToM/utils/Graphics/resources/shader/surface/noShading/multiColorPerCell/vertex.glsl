#version 330 core

in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec4 clippingPlane[6];
out float gl_ClipDistance[6];

void main( void )
{
    gl_Position = proj * view * model * vec4( position, 1.0 );
    
    vec4 fragPosWorld = model * vec4( position, 1.0 );
    
    for( int planeId = 0; planeId < 6; ++planeId )
    {
        gl_ClipDistance[ planeId ] = dot( fragPosWorld, clippingPlane[ planeId ] );
    }
}
