#version 330 core

in vec3 position;
in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec4 fragPos;
out vec3 fragNorm;

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

    fragPos = view * fragPosWorld;
    
    mat4 normalMatrix = transpose( inverse( view * model ) );
    fragNorm = normalize( normalMatrix * vec4( normal, 0.0 ) ).xyz;
}
