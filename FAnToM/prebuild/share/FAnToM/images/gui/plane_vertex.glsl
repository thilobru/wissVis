#version 330

uniform mat4 model;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 proj_inv;
uniform mat4 view_inv;
uniform int screenWidth;
uniform int screenHeight;

in vec3 position;

#define sizeRatio 0.8

void main()
{
    vec4 rotCenter_world = model * vec4( 0, 0, 0, 1 );
    vec4 rotCenter_screen = proj * view * rotCenter_world;
    rotCenter_screen /= rotCenter_screen.w;
    vec4 rotOffset_world = view_inv * proj_inv * (rotCenter_screen + vec4( 0.5 * sizeRatio, 0, 0, 0 ));
    rotOffset_world /= rotOffset_world.w;

    float scale = length(rotCenter_world - rotOffset_world);

    gl_Position = proj * view * model * vec4( scale * position, 1.0 );
}