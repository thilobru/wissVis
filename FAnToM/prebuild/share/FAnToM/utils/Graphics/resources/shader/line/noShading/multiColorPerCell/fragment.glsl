#version 330 core
#define DEPTH_PEELING 0

uniform sampler3D colormap;

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

    ivec3 texSize = textureSize( colormap, 0);
    
    ivec3 texMap;
    texMap.x = gl_PrimitiveID % texSize.x;
    texMap.y = ( ( gl_PrimitiveID - texMap.x) / texSize.x ) % texSize.y;
    texMap.z = ( ( gl_PrimitiveID - texMap.x - texMap.y * texSize.x ) / ( texSize.x * texSize.y ) ) % texSize.z;
    
    out_color = texelFetch( colormap, texMap, 0 );
}
