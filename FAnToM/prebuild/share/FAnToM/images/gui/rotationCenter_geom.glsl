#version 330

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform int screenWidth;
uniform int screenHeight;

in vec4 geom_color[2];

flat out vec4 frag_color;
noperspective out vec2 frag_coords;
flat out float totalLength;


const vec3 billboard_vertices[4] = vec3[](
        vec3(-1., 1., 0.),
        vec3(1., 1., 0.),
        vec3(-1., -1., 0.),
        vec3(1., -1., 0.)
);

#define WIDTH 10.0

vec3 homog( vec4 v )
{
    return v.xyz / v.w;
}

vec3 screen2px( vec3 screen ) {
     return ( screen.xyz * 0.5 + 0.5 ) * vec3( screenWidth, screenHeight, 1 );
}

vec4 px2screen( vec3 px ) {
     return vec4( ( px / vec3( screenWidth, screenHeight, 1 ) - 0.5 ) * 2.0, 1.0 );
}

void main(void)
{
    vec3 start = homog( gl_in[0].gl_Position );
    vec3 end = homog( gl_in[1].gl_Position );
    vec3 start_px = screen2px( start );
    vec3 end_px = screen2px( end );

    totalLength = distance( start_px.xy, end_px.xy );

    vec3 dir = ( end_px - start_px ) / totalLength;
    vec3 normal = vec3( -dir.y, dir.x, 0 );

    frag_color = geom_color[0];

    frag_coords = vec2( -WIDTH, -WIDTH );
    gl_Position = px2screen( start_px - normal * WIDTH - dir * WIDTH );
    EmitVertex();
    frag_coords = vec2( WIDTH , -WIDTH );
    gl_Position = px2screen( start_px + normal * WIDTH - dir * WIDTH );
    EmitVertex();
    frag_coords = vec2( -WIDTH, WIDTH + totalLength );
    gl_Position = px2screen( end_px - normal * WIDTH + dir * WIDTH );
    EmitVertex();
    frag_coords = vec2( WIDTH, WIDTH + totalLength );
    gl_Position = px2screen( end_px + normal * WIDTH + dir * WIDTH );
    EmitVertex();

    EndPrimitive();
}
