#version 330 core
#define DEPTH_PEELING 0

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform vec4 in_color;

#if DEPTH_PEELING
uniform sampler2D u_depthPeelingTexture;
#endif

in vec3 frag_rayStart;
in vec3 frag_rayEnd;
in mat4 frag_transform;
in float radius;

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
    float q = dot( frag_rayStart, frag_rayStart ) - radius * radius;
    float discriminant = 4.0 * ( p * p - q );

    if( discriminant < 0.0 )
    {
        discard;
    }

    float t = -p - 0.5 * sqrt( discriminant );

    vec3 point = frag_rayStart + t * rayDir;

    vec4 screenPoint = proj * view * model * frag_transform * vec4( point, 1.0 );
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
    float diffuse = 0.5 * abs( dot( point/radius, lightDir ) );
    float specular = 0.3 * pow( abs( dot( point/radius, lightDir ) ), 60.0 );

    out_color = vec4(
        ( ambient + diffuse ) * in_color.xyz
        + specular * vec3( 1.0, 1.0, 1.0 ),
        in_color.a
    );
}
