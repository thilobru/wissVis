#version 330


#define BORDER_THICKNESS 0.01


uniform vec4 u_frontLineColor;
uniform vec4 u_backLineColor;
uniform vec4 u_backFaceColor;

in vec3 frag_coord;
in float frag_direction;

out vec4 out_color;


void main()
{
    float value = min( frag_coord.x, min( frag_coord.y, frag_coord.z ) );

    if( frag_direction < 0.0 )
    {
        if( value < BORDER_THICKNESS )
        {
            out_color = u_frontLineColor;
        }
        else
        {
            discard;
        }
    }
    else
    {
        if( value < BORDER_THICKNESS )
        {
            out_color = u_backLineColor;
        }
        else
        {
            out_color = u_backFaceColor;
        }
    }
}
