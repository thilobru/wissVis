#version 330 core

in vec4 fragColor;
in vec2 fragTexCoords;

uniform sampler2D inTexture;

out vec4 out_color;

void main( void )
{
    out_color = mix(texture(inTexture, fragTexCoords), fragColor, fragColor.a);
}
