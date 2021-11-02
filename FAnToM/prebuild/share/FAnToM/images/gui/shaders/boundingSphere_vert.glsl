#version 330

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

uniform mat4 proj_inv;

in vec3 in_vertex;
in vec3 in_normal;
in vec3 in_coord;

out vec3 frag_coord;
out float frag_direction;

vec3 homogenize( vec4 v )
{
    return v.xyz / v.w;
}

void main()
{
    vec3 vertex = homogenize( proj * view * model * vec4( in_vertex, 1.0 ) );

    vec3 rayStart = homogenize( proj_inv * vec4( vertex.xy, -0.9, 1.0 ) );
    vec3 rayEnd = homogenize( proj_inv * vec4( vertex.xy, 0.9, 1.0 ) );

    frag_direction = dot( ( view * model * vec4( in_normal, 0.0 ) ).xyz, rayEnd - rayStart );

    frag_coord = in_coord;

    gl_Position = vec4( vertex, 1.0 );
}
