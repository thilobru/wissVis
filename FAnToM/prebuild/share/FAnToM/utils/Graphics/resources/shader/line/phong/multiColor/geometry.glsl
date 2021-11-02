#version 330 core

uniform float u_lineWidth;
uniform int screenWidth;
uniform int screenHeight;

layout( lines ) in;

layout( triangle_strip, max_vertices = 4 ) out;

in vec4 gColor[];
in vec4 gPos[];
in vec3 gNorm[];
in clipDists_t
{
   float clipDistance[6];
} clipDists[];

out vec4 fColor;
out vec4 fPos;
out vec3 fNorm;
out float gl_ClipDistance[6];


vec3 homog( vec4 v )
{
    return v.xyz / v.w;
}

vec2 screen2px( vec2 screen )
{
     return ( screen * 0.5 + 0.5 ) * vec2( screenWidth, screenHeight );
}

vec2 px2screen( vec2 px )
{
     return ( px / vec2( screenWidth, screenHeight ) - 0.5 ) * 2.0;
}


void main()
{
    vec3 start = homog( gl_in[ 0 ].gl_Position );
    vec3 end = homog( gl_in[ 1 ].gl_Position );

    vec2 start_px = screen2px( start.xy );
    vec2 end_px = screen2px( end.xy );

    vec2 offset = normalize( vec2(
         end_px.y - start_px.y,
         start_px.x - end_px.x ) )
       * u_lineWidth / 2;

    fColor = gColor[0];
    fPos = gPos[0];
    fNorm = gNorm[0];

    fColor = gColor[1];
    fPos = gPos[1];
    fNorm = gNorm[1];

    gl_Position = vec4( px2screen( start_px.xy + offset ) * gl_in[0].gl_Position.w, start.z * gl_in[0].gl_Position.w, gl_in[0].gl_Position.w );
    for( int planeId = 0; planeId < 6; ++planeId )
    {
        gl_ClipDistance[planeId] = clipDists[0].clipDistance[planeId];
    }
    EmitVertex();

    gl_Position = vec4( px2screen( start_px.xy - offset ) * gl_in[0].gl_Position.w, start.z * gl_in[0].gl_Position.w, gl_in[0].gl_Position.w );
    for( int planeId = 0; planeId < 6; ++planeId )
    {
        gl_ClipDistance[planeId] = clipDists[0].clipDistance[planeId];
    }
    EmitVertex();

    gl_Position = vec4( px2screen( end_px.xy + offset ) * gl_in[1].gl_Position.w, end.z * gl_in[1].gl_Position.w, gl_in[1].gl_Position.w );
    for( int planeId = 0; planeId < 6; ++planeId )
    {
        gl_ClipDistance[planeId] = clipDists[1].clipDistance[planeId];
    }
    EmitVertex();
    
    gl_Position = vec4( px2screen( end_px.xy - offset ) * gl_in[1].gl_Position.w, end.z * gl_in[1].gl_Position.w, gl_in[1].gl_Position.w );
    for( int planeId = 0; planeId < 6; ++planeId )
    {
        gl_ClipDistance[planeId] = clipDists[1].clipDistance[planeId];
    }
    EmitVertex();
    EndPrimitive();
}
