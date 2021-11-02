#version 330

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

in vec3 in_vertex;
in vec4 in_color;

out vec4 geom_color;

void main()
{
    geom_color = in_color;

    gl_Position = proj * view * model * vec4( in_vertex, 1.0 );
}
