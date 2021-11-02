#version 330 core

// vertex attributes
in vec3 position;
in vec3 normal;
in vec4 color_i;
in vec4 color_j;
in vec2 glyph_radius;

// FAnToM uniforms
uniform mat4 model;
uniform mat4 view;

// attributes forwarded to geometry shader
out vec4 vertex_color_i;
out vec4 vertex_color_j;
out vec2 pass_radius;
out vec3 geom_normal;
out mat4 geom_transform;
out mat4 geom_transform_inv;

mat4 scaleMatrix( float scale )
{
    return mat4(
           scale,     0.0,     0.0, 0.0,
               0.0, scale,     0.0, 0.0,
               0.0,     0.0, scale, 0.0,
               0.0,     0.0,     0.0, 1.0
    );
}

mat4 translationMatrix( vec3 pos )
{
    return mat4(
             1.0,   0.0,   0.0, 0.0,
             0.0,   1.0,   0.0, 0.0,
             0.0,   0.0,   1.0, 0.0,
           pos.x, pos.y, pos.z, 1.0
    );
}

void main( void )
{
	gl_Position = view * model * vec4( position, 1.0 );
	geom_normal = vec3( view * model * vec4( normal, 0.0 ) );
	vertex_color_i = color_i;
    vertex_color_j = color_j;
    pass_radius = glyph_radius;
                
    geom_transform = translationMatrix(position);
    geom_transform_inv = translationMatrix( -position );
}
