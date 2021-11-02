#version 330 core

in vec3 position;
in vec4 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec4 fragPos;
out vec4 fragColor;

uniform vec4 clippingPlane[6];
out float gl_ClipDistance[6];

void main( void )
{
    gl_Position = proj * view * model * vec4( position, 1.0 );
    fragColor = color;
    
    vec4 fragPosWorld = model * vec4( position, 1.0 );
    
    for( int planeId = 0; planeId < 6; ++planeId )
    {
        gl_ClipDistance[ planeId ] = dot( fragPosWorld, clippingPlane[ planeId ] );
    }
}
