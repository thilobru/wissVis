#version 330

uniform mat4 model;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 proj_inv;
uniform mat4 view_inv;
uniform int screenWidth;
uniform int screenHeight;
uniform vec4 clippingPlane[6];

uniform vec3 center;

in vec2 positions;
in vec2 texCoords;

out vec2 frag_tex;
out float gl_ClipDistance[6];

void main()
{
     vec4 ss_center = proj * view * model * vec4(center, 1.0);
     ss_center /= ss_center.w;

     frag_tex = texCoords;
     vec4 screenPos = ss_center + vec4( 2.0 * positions / vec2( screenWidth, screenHeight ), 0.0, 0.0 );
     vec4 worldPos = view_inv * proj_inv * screenPos;
     gl_Position = screenPos;
     for( int i=0; i<6; ++i ) gl_ClipDistance[i] = dot( worldPos, clippingPlane[i] );
}
