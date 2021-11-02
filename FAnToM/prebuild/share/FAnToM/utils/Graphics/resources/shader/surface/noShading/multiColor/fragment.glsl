#version 330 core
#define DEPTH_PEELING 0

/*struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float alpha;
    float sharpness;
};*/

// uniform Material material;

in vec4 fragColor;

out vec4 out_color;

#if DEPTH_PEELING
    uniform int screenWidth;
    uniform int screenHeight;
    uniform sampler2D u_depthPeelingTexture;
#endif

void main( void )
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

    out_color = fragColor;
}
