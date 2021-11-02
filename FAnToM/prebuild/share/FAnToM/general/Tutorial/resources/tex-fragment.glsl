#version 330 core

in vec3 fragTexCoords;

uniform sampler3D inTexture;

out vec4 out_color;

void main()
{
    out_color = texture( inTexture, fragTexCoords );
}
