#version 330

in vec2 in_position;

out vec2 frag_texCoords;

void main()
{
    frag_texCoords = in_position * 0.5 + 0.5;
    gl_Position = vec4( in_position, 0.0, 1.0 );
}
