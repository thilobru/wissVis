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

in vec4 fragPos;
in vec3 fragNorm;
in vec4 fragColor;

out vec4 out_color;

#if DEPTH_PEELING
    uniform int screenWidth;
    uniform int screenHeight;
    uniform sampler2D u_depthPeelingTexture;
#endif

vec4 phong( vec4 color, vec3 n, vec3 l, vec3 material, mat3x3 l_color, float s )
{
    vec3 ambient = material.x * color.rgb * l_color[0];
    vec3 diffuse = material.y * abs( dot( n, l ) ) * color.rgb * l_color[1];
    vec3 specular = material.z * pow( abs( dot( l, n ) ), s ) * l_color[2];

    return vec4( ambient + diffuse + specular, color.a );
}

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

    vec3 n = normalize( fragNorm );
    vec3 l = normalize( -fragPos.xyz / fragPos.w );
    vec3 mat = vec3( 0.3, 1.0, 0.3 );
    mat3 l_col;
    l_col[0] = vec3( 0.5, 0.5, 0.5 );
    l_col[1] = vec3( 0.5, 0.5, 0.5 );
    l_col[2] = vec3( 1.0, 1.0, 1.0 );
    out_color = phong( fragColor, n, l, mat, l_col, 60.0 );
}
