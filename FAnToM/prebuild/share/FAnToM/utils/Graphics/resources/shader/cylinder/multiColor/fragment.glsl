#version 330 core
#define infinite 100000000000.0
#define DEPTH_PEELING 0

in vec4 fragment_color_i;
in vec4 fragment_color_j;
in vec4 fragment_norm;
in vec2 frag_radius;
in vec3 frag_rayStart; 
in vec3 frag_rayEnd; 
in mat4 frag_transform;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

#if DEPTH_PEELING
uniform sampler2D u_depthPeelingTexture;
#endif

out vec4 out_color;

vec4 phong( vec4 color, vec3 n, vec3 l, vec3 material, mat3 l_color, float s )
{
    vec3 ambient = material.x * color.rgb * l_color[0];
    vec3 diffuse = material.y * abs( dot( n, l ) ) * color.rgb * l_color[1];
    vec3 specular = material.z * pow( abs( dot( l, n ) ), s ) * l_color[2];

    return vec4( ambient + diffuse + specular, color.a );
}

float applyDepthRange( float depth ) 
{
    float far = gl_DepthRange.far;
    float near = gl_DepthRange.near;
    return ( ( gl_DepthRange.diff * depth ) + near + far ) / 2.0;
}


void main( void )
{
	
    float t1 = infinite;
    float t2 = infinite;
    float t3 = infinite;
    float t4 = infinite;

    bool isec_tube = false;
    bool isec_cup = false;

	vec3 rayDir = normalize( frag_rayEnd - frag_rayStart ); 
	vec3 aim = rayDir;	
	vec3 eye = frag_rayStart; 


    vec3 norm_part = frag_radius.y * normalize(fragment_norm.xyz);    
    vec3 p_i3 = vec3( 0.0,0.0,0.0 ) + norm_part * 0.5;
    vec3 p_j3 = p_i3 - norm_part;
    float rad = frag_radius.x;
    vec3 stem = normalize( p_j3 - p_i3 );

    float a = dot( aim - dot( aim, stem ) * stem, aim - dot( aim, stem ) * stem );
    float b = 2.0 * dot( aim - dot( aim, stem ) * stem, eye - p_i3 - dot( eye - p_i3, stem ) * stem );
    float c = dot( eye - p_i3 - dot( eye - p_i3, stem ) * stem,
                   eye - p_i3 - dot( eye - p_i3, stem ) * stem )
              - pow( rad, 2.0 );


    float d = pow( b, 2.0 ) - ( 4.0 * a * c );

    if( d < 0 )
        discard;

    t1 = ( -b - sqrt( d ) ) / ( 2.0 * a );
    t2 = ( -b + sqrt( d ) ) / ( 2.0 * a );

    if( t1 < 0.0 || !( dot( stem, eye + aim * t1 - p_i3 ) > 0.0 && dot( stem, eye + aim * t1 - p_j3 ) < 0.0 ) )
    {
        t1 = infinite;
    }
    if( t2 < 0.0 || !( dot( stem, eye + aim * t2 - p_i3 ) > 0.0 && dot( stem, eye + aim * t2 - p_j3 ) < 0.0 ) )
    {
        t2 = infinite;
    }
    isec_tube = min( t1, t2 ) != infinite;

    if( dot( stem, aim ) != 0.0 )
    {
        t3 = ( dot( stem, p_i3 ) - dot( stem, eye ) ) / dot( stem, aim );
        t4 = ( dot( -stem, p_j3 ) - dot( -stem, eye ) ) / dot( -stem, aim );

        if( t3 < 0.0 || !( dot( eye + aim * t3 - p_i3, eye + aim * t3 - p_i3 ) <= pow( rad, 2.0 ) ) )
        {
            t3 = infinite;
        }

        if( t4 < 0.0 || !( dot( eye + aim * t4 - p_j3, eye + aim * t4 - p_j3 ) <= pow( rad, 2.0 ) ) )
        {
            t4 = infinite;
        }

        isec_cup = min( t3, t4 ) != infinite;
    }

    if( !isec_tube && !isec_cup )
        discard;

    if( isec_tube && isec_cup )
    {
        if( min( t1, t2 ) < min( t3, t4 ) )
        {
            isec_cup = false;
        }
        else
        {
            isec_tube = false;
        }
    }

    vec3 tube_normal = vec3( 0., 0., 0. );
    vec3 intersection = vec3( 0., 0., 0. );
    vec4 tube_color = vec4( 0., 0., 0., 1. );

    if( isec_cup )
    {
        intersection = eye + min( t3, t4 ) * aim;
        if( t3 < t4 )
        {
            tube_normal = normalize( p_i3 - p_j3 );
            tube_color = fragment_color_j;
        }
        else
        {
            tube_normal = normalize( p_j3 - p_i3 );
            tube_color = fragment_color_i;
        }
    }
    else
    {
        intersection = eye.xyz + min( t1, t2 ) * aim;
        float dist_i = dot( stem, intersection - p_i3 );
        float dist_j = dot( -stem, intersection - p_j3 );

        float stem_len = pow( length( intersection - p_i3 ), 2.0 ) - pow( rad, 2.0 );

        if( stem_len < 0.0 )
        {
            stem_len = 0.0;
        }

        tube_normal = normalize( intersection - ( p_i3 + stem * sqrt( stem_len ) ) );


        vec3 interpol_color = fragment_color_i.rgb * dist_i + fragment_color_j.rgb * dist_j;
        interpol_color /= ( dist_i + dist_j );
        tube_color = vec4( interpol_color, 1. );
    }

    vec4 screenPoint = proj * view * model * frag_transform * vec4( intersection, 1.0 );
    float depth = applyDepthRange( screenPoint.z / screenPoint.w );

    #if DEPTH_PEELING
    if( depth <= texture( u_depthPeelingTexture, (screenPoint.xy / screenPoint.w) / 2 + 0.5 ).r )
    {
        discard;
    }
    #endif

    gl_FragDepth = depth;

	vec3 l = -rayDir;
	vec3 n = tube_normal;

    mat3 light_source;
    light_source[0] = vec3( 0.5, 0.5, 0.5 );
    light_source[1] = vec3( 0.5, 0.5, 0.5 );
    light_source[2] = vec3( 1.0, 1.0, 1.0 );
    vec3 material = vec3( 0.3, 1.0, 0.3 );
    
    out_color = phong( tube_color, n, l, material, light_source, 60.0 );
	
}