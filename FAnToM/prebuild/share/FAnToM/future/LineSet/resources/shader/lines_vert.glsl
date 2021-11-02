#version 330

uniform mat4 proj;
uniform mat4 view;
uniform mat4 view_inv;
uniform mat4 model;
uniform mat4 model_inv;

in vec3 in_position;
in vec3 in_tangent;
in vec3 in_color;

out vec3 frag_color;
out vec3 frag_normal;
out vec3 frag_tangent;
out float frag_zoom;

void main()
{
    vec3 c = ( view * model * vec4( in_position, 1.0 ) ).xyz;
    vec3 t = ( view * model * vec4( in_tangent, 0.0 ) ).xyz;

    frag_normal = normalize( proj * model_inv * view_inv * vec4( normalize( cross( cross( c, t ), t ) ), 0.0 ) ).xyz;

    frag_tangent = normalize( ( proj * vec4( in_tangent, 0.0 ) ).xyz );

    frag_zoom = length( ( proj * view * model * vec4( vec3( sqrt( 3.0 ) ), 0.0 ) ).xyz );

    frag_color = in_color;

    gl_Position = proj * view * model * vec4( in_position, 1.0 );
}
