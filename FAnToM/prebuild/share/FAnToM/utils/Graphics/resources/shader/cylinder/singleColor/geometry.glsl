#version 330 core

layout( points ) in;

in vec3 geom_normal[];
in vec2 pass_radius[];
in mat4 geom_transform[]; 
in mat4 geom_transform_inv[];  

uniform mat4 view_inv;
uniform mat4 view;
uniform mat4 model_inv; 
uniform mat4 proj;
uniform mat4 proj_inv; 
uniform vec4 clippingPlane[6];


const vec3 billboard_vertices[4]
    = vec3[]( vec3( -1., 1., 0. ), vec3( 1., 1., 0. ), vec3( -1., -1., 0. ), vec3( 1., -1., 0. ) );

out vec4 fragment_norm;
out vec2 frag_radius;
out vec3 frag_rayStart; 
out vec3 frag_rayEnd; 
out mat4 frag_transform; 
out float gl_ClipDistance[6];

layout( triangle_strip, max_vertices = 4 ) out;


vec3 homog( vec4 v ) 
{
    return v.xyz / v.w;
}


void main( void )
{
	vec4 pos = gl_in[0].gl_Position;
	mat4 screenToWorld = geom_transform_inv[0] * model_inv * view_inv * proj_inv;
	
	fragment_norm = geom_transform_inv[0] * model_inv * view_inv * vec4(geom_normal[0], 0.);	
	frag_radius = pass_radius[0];
	frag_transform = geom_transform[0];
	
	float radius = max( pass_radius[0].x , pass_radius[0].y );	
	
	for (int cornerId = 0; cornerId < 4; ++cornerId)
	{		
		vec4 vertexPos = 1.2 * radius * vec4( billboard_vertices[ cornerId ], 0.0 ) + pos;
			
		for( int planeId = 0; planeId < 6; ++planeId )
        {
            gl_ClipDistance[ planeId ] = dot( view_inv * vertexPos, clippingPlane[ planeId ] );
        }
        
        vec4 screenPos = proj * vertexPos;
        
        frag_rayStart = homog( screenToWorld * vec4( screenPos.xy / screenPos.w, -1.0, 1.0 ) );
        frag_rayEnd = homog( screenToWorld * vec4( screenPos.xy / screenPos.w, 1.0, 1.0 ) );

        gl_Position = screenPos;
        EmitVertex();
	}
	
	EndPrimitive();
}
