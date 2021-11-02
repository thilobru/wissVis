#version 330 core

uniform mat4 view_inv;
uniform mat4 model_inv;
uniform mat4 proj;
uniform mat4 proj_inv;
uniform vec4 clippingPlane[6];

uniform u_size;

layout( points ) in;
in vec3 geom_position[];

layout( triangle_strip, max_vertices = 4 ) out;
out vec3 frag_rayStart;
out vec3 frag_rayEnd;
out vec3 frag_position;


const vec3 billboardVertices[ 4 ] = vec3[](
     vec3( -1.0, 1.0, 0.0 ),
     vec3( 1.0, 1.0, 0.0 ),
     vec3( -1.0, -1.0, 0.0 ),
     vec3( 1.0, -1.0, 0.0 )
);


vec3 homog( vec4 v )
{
    return v.xyz / v.w;
}


void main( void )
{
    vec4 pos = gl_in[ 0 ].gl_Position;

    frag_position = geom_position[ 0 ];

    mat4 screenToWorld = model_inv * view_inv * proj_inv;

    for( int cornerId = 0; cornerId < 4; ++cornerId )
    {
        vec4 vertexPos = 1.3 * u_size * vec4( billboardVertices[ cornerId ], 0.0 ) + pos;

        for( int planeId = 0; planeId < 6; ++planeId )
        {
            gl_ClipDistance[ planeId ] = dot( view_inv * vertexPos, clippingPlane[ planeId ] );
        }

        vec4 screenPos = proj * vertexPos;

        frag_rayStart = homog( screenToWorld * vec4( screenPos.xy / screenPos.w, -0.1, 1.0 ) ) - geom_position[ 0 ];
        frag_rayEnd = homog( screenToWorld * vec4( screenPos.xy / screenPos.w, 0.1, 1.0 ) ) - geom_position[ 0 ];

        gl_Position = screenPos;
        EmitVertex();
    }

    EndPrimitive();
}
