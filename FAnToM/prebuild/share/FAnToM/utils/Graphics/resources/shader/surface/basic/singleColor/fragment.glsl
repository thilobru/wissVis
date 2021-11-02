#version 330 core
#define DEPTH_PEELING 0

in float coloring;

uniform vec4 color;

out vec4 out_color;

#if DEPTH_PEELING
    uniform int screenWidth;
    uniform int screenHeight;
    uniform sampler2D u_depthPeelingTexture;
#endif


void main()
{
    #if DEPTH_PEELING
        vec2 texelCoord = gl_FragCoord.xy / vec2(screenWidth, screenHeight);
        float previousLayerDepth = texture(u_depthPeelingTexture, texelCoord).r;

        float depth = gl_FragCoord.z;

        // Check if the fragment is in front of the fragment of the previous
        // "peeled" layer
        if (depth <= previousLayerDepth)
        {
            discard;
        }

        gl_FragDepth = depth;
    #endif

    out_color = vec4((coloring * color).xyz, color.a);
}
