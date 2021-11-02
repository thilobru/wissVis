#version 330 core
#define DEPTH_PEELING 0

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

uniform vec4 u_color;

#if DEPTH_PEELING
uniform sampler2D u_depthPeelingTexture;
#endif

in vec3 frag_rayStart;
in vec3 frag_rayEnd;
in vec3 frag_position;

out vec4 out_color;


float applyDepthRange( float depth )
{
    float far = gl_DepthRange.far;
    float near = gl_DepthRange.near;
    return ( ( gl_DepthRange.diff * depth ) + near + far ) / 2.0;
}


void main()
{
    vec3 rayDir = normalize( frag_rayEnd - frag_rayStart );

    float p = dot( frag_rayStart, rayDir );
    float q = dot( frag_rayStart, frag_rayStart ) - 1.0;
    float discriminant = 4.0 * ( p * p - q );

    if( discriminant < 0.0 )
    {
        discard;
    }

    float t = -p - 0.5 * sqrt( discriminant );

    vec3 point = frag_rayStart + t * rayDir;

    vec4 screenPoint = proj * view * model * vec4( point + frag_position, 1.0 );
    float depth = applyDepthRange( screenPoint.z / screenPoint.w );

    #if DEPTH_PEELING
    vec2 texelCoord = (screenPoint.xy / screenPoint.w) / 2 + 0.5;
    if( depth <= texture( u_depthPeelingTexture, texelCoord ).r )
    {
        discard;
    }
    #endif

    gl_FragDepth = depth;

    // the light currently is emmited directly from the camera
    vec3 lightDir = -rayDir;

    // compute light components
    float ambient = 0.15;
    float diffuse = 0.5 * max( dot( point, lightDir ), 0.0 );
    float specular = 0.3 * pow( abs( dot( point, lightDir ) ), 60.0 );

    out_color = vec4(
        ( ambient + diffuse ) * u_color.xyz
        + specular * vec3( 1.0, 1.0, 1.0 ),
        u_color.a
    );
}