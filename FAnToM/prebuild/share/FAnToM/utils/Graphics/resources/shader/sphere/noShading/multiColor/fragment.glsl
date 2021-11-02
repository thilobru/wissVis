#version 330 core

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

in vec4 frag_color;
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
    gl_FragDepth = applyDepthRange( screenPoint.z / screenPoint.w );

    out_color = frag_color;
}
