#version 330 core

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform vec4 clippingPlane[6];

in vec3 in_vertex;
in vec4 in_color;

out vec4 gColor;
out clipDists_t
{
    float clipDistance[6];
};

void main()
{
    gl_Position = proj * view * model * vec4( in_vertex, 1.0 );
    gColor = in_color;
    
    vec4 gPosWorld =  model * vec4( in_vertex, 1.0 );
    for( int planeId = 0; planeId < 6; ++planeId )
    {
        clipDistance[ planeId ] = dot( gPosWorld, clippingPlane[ planeId ] );
    }
}
