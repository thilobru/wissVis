#version 330 core

layout( points ) in;
in mat4 geom_transform[];
in mat4 geom_transform_inv[];
in float geom_billboardSize[];

uniform mat4 view_inv;
uniform mat4 view;
uniform mat4 model_inv;
uniform mat4 proj;
uniform mat4 proj_inv;
uniform vec4 clippingPlane[6];

const vec3 billboardVertices[ 4 ] = vec3[ 4 ](
     vec3( -1.0, 1.0, 0.0 ),
     vec3( 1.0, 1.0, 0.0 ),
     vec3( -1.0, -1.0, 0.0 ),
     vec3( 1.0, -1.0, 0.0 )
);

out vec3 frag_rayStart;
out vec3 frag_rayEnd;
out mat4 frag_transform;
out float gl_ClipDistance[6];
out float radius;

layout( triangle_strip, max_vertices = 4 ) out;



vec3 homog( vec4 v )
{
    return v.xyz / v.w;
}


void main( void )
{
    vec4 pos = gl_in[ 0 ].gl_Position;

    mat4 screenToWorld = geom_transform_inv[ 0 ] * model_inv * view_inv * proj_inv;

    radius = geom_billboardSize[ 0 ];
    frag_transform = geom_transform[ 0 ];

    for( int cornerId = 0; cornerId < 4; ++cornerId )
    {
        vec4 vertexPos = 1.3 * geom_billboardSize[ 0 ] * vec4( billboardVertices[ cornerId ], 0.0 ) + pos;

        for( int planeId = 0; planeId < 6; ++planeId )
        {
            gl_ClipDistance[ planeId ] = dot( view_inv * vertexPos, clippingPlane[ planeId ] );
        }

        vec4 screenPos = proj * vertexPos;

        frag_rayStart = homog( screenToWorld * vec4( screenPos.xy / screenPos.w, -1.0, 1.0 ) );
        frag_rayEnd = homog( screenToWorld * vec4( screenPos.xy / screenPos.w, 0.0, 1.0 ) );

        gl_Position = screenPos;
        EmitVertex();
    }

    EndPrimitive();
}
