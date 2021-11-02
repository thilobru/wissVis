#version 330

in vec3 frag_color;
in vec3 frag_normal;
in vec3 frag_tangent;
in float frag_zoom;

out vec4 out_color;
out vec3 out_normal;
out vec3 out_tangent;
out float out_zoom;

void main()
{
    out_normal = frag_normal * 0.5 + 0.5;
    out_zoom = frag_zoom;
    out_color = vec4( frag_color, 1.0 );
    out_tangent = frag_tangent * 0.5 + 0.5;
}
