#version 330 core

uniform vec4 textColor;
uniform float weight;

uniform sampler2D font;

in vec2 frag_tex;
out vec4 out_color;

void main()
{
        float smoothing = 0.2;
        float distance = 1.0 - texture(font, frag_tex).a;
        if( distance > weight + smoothing )
            discard;
        float alpha = smoothstep( weight + smoothing, weight, distance );
        out_color = vec4( textColor.rgb, textColor.a * alpha );
}
