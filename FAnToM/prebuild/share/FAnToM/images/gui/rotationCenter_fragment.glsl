#version 330

flat in vec4 frag_color;
noperspective in vec2 frag_coords;
flat in float totalLength;

out vec4 out_color;

float minimum_distance(vec2 v, vec2 w, vec2 p) {
  float l2 = dot(w-v, w-v);
  if (l2 == 0.0) return distance(p, v);
  float t = clamp( dot(p - v, w - v) / l2, 0.0, 1.0 );
  vec2 projection = v + t * (w - v);
  return distance(p, projection);
}

#define WIDTH 10.0

void main()
{
    float dist = minimum_distance( vec2(0, 0), vec2(0, totalLength), frag_coords );
    float dist2 = minimum_distance( vec2(0, totalLength), vec2(-WIDTH*0.75, totalLength - 2*WIDTH), frag_coords );
    float dist3 = minimum_distance( vec2(0, totalLength), vec2(0.75*WIDTH, totalLength - 2*WIDTH), frag_coords );
    float distArrow = min( dist, min( dist2, dist3 ) );

    float alpha = 1 - max( 0, (dist - 0.1 * WIDTH) / (0.1 * WIDTH) );
    float alphaArrow = 1 - max( 0, (distArrow - 0.1 * WIDTH) / (0.1 * WIDTH) );

    // float finalAlpha = max( alpha, alphaArrow * smoothstep( 2*WIDTH, 5*WIDTH, totalLength ) );
    float finalAlpha = mix( alpha, alphaArrow, smoothstep( 2*WIDTH, 5*WIDTH, totalLength ) );

    if( finalAlpha < 0.01 )
        discard;
    out_color = vec4( frag_color.rgb, finalAlpha );
}