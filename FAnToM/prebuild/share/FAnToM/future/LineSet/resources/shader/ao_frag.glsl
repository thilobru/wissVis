#version 330

in vec2 frag_texCoords;

out vec4 out_color;

uniform mat4 view_inv;
uniform sampler2D u_colorTex;
uniform sampler2D u_normalTex;
uniform sampler2D u_tangentTex;
uniform sampler2D u_zoomTex;
uniform sampler2D u_depthTex;
uniform sampler2D u_noiseTex;

uniform int u_useGauss;

uniform float u_colorSizeX;
uniform float u_colorSizeY;

uniform float u_lineAORadius = 2.0f;
uniform float u_lineAODensityWeight = 1.0f;
uniform float u_lineAOTotalStrength = 1.0f;

vec3 u_lightPosition = vec3( view_inv * vec4( 0.0, 0.0, 0.0, 1.0 ) );

#define SCALES 3
#define SAMPLES 8

vec3 getTangent( vec2 where, float lod )
{
    return textureLod( u_tangentTex, where, lod ).xyz * 2.0 - 1.0;
}

vec4 getColor( vec2 where )
{
    return texture( u_colorTex, where ).rgba;
}

vec3 getNormal( vec2 where, float lod )
{
    return textureLod( u_normalTex, where, lod ).xyz * 2.0 - 1.0;
}

vec3 getNormal( vec2 where )
{
    return getNormal( where, 0.0 );
}

float getDepth( vec2 where, float lod )
{
    return textureLod( u_depthTex, where, lod ).r;
}

float getDepth( vec2 where )
{
    return getDepth( where, 0.0 );
}

float getLineAO( vec2 where )
{
    const float invSamples = 1.0 / float( SAMPLES );

    // Fall-off for SSAO per occluder. This hould be zero (or nearly zero) since it defines what is counted as before,
    // or behind.
    const float falloff = 0.00001;

    // grab a random normal for reflecting the sample rays later on
    vec3 randNormal = normalize( texture( u_noiseTex, where ).rgb * 2.0 - 1.0 );

    // grab the current pixel's normal and depth
    float currentPixelDepth = getDepth( where );

    // current fragment coords in screen space
    vec3 ep = vec3( where, currentPixelDepth );

    // get the normal of current fragment
    vec3 normal = getNormal( where );

    // the radius of the sphere is, in screen-space, half a pixel. So the hemisphere covers nearly one pixel. Scaling by
    // depth somehow makes it
    // more invariant for zooming
    float maxPixels = max( float( u_colorSizeX ), float( u_colorSizeY ) );
    float radius = ( texture( u_zoomTex, where ).r * u_lineAORadius / maxPixels ) / ( 1.0 - currentPixelDepth );

    // some temporaries needed inside the loop
    vec3 ray;             // the current ray inside the sphere
    vec3 hemispherePoint; // the point on the hemisphere
    vec3 occluderNormal;  // the normal at the point on the hemisphere

    float occluderDepth;    // the depth of the potential occluder on the hemisphere
    float depthDifference;  // the depth difference between the current point and the occluder (point on hemisphere)
    float normalDifference; // the projection of the occluderNormal to the surface normal

    // accumulated occlusion
    float occlusion = 0.0;
    float radiusScaler = 0.0; // we sample with multiple radii, so use a scaling factor here

    // sample for different radii
    for( int l = 0; l < SCALES; ++l )
    {
        float occlusionStep = 0.0; // this variable accumulates the occlusion for the current radius

// this allows an adaptive radius
// NOTE: we do not exactly use the linear scaling from the paper here. Although they look very similar, the non-linear
// radius, which
// increases non-linearly with the level l, improves visual quality a bit. The drawback of this method is that
// increasing SCALERS causes
// larger structures to become more important than local structures.
#define radScaleMin 1.5
        radiusScaler += radScaleMin + l;
        // Alternatvie, linear way -- more coherent AO when changing the number of hemispheres:
        // radiusScaler = radScaleMin + l;    // increment radius each time.

        // Get SAMPLES-times samples on the hemisphere and check for occluders
        int numSamplesAdded = 0; // used to count how many samples really got added to the occlusion term
        for( int i = 0; i < SAMPLES; ++i )
        {
            // grab a rand normal from the noise texture
            vec3 randSphereNormal
                = texture( u_noiseTex, vec2( float( i ) / float( SAMPLES ), float( l + 1 ) / float( SCALES ) ) ).rgb
                      * 2.0
                  - 1.0;

            // get a vector (randomized inside of a sphere with radius 1.0) from a texture and reflect it
            vec3 hemisphereVector = reflect( randSphereNormal, randNormal );
            ray = radiusScaler * radius * hemisphereVector;
            ray = sign( dot( ray, normal ) ) * ray;

            // get the point in texture space on the hemisphere
            hemispherePoint = ray + ep;

            // we need to handle the case where a hemisphere point is outside the texture space. The correct solution
            // would be to ensure that
            // no hemisphere point ever gets outside the texture space. This can be achieved by rendering the scene to a
            // larger texture and than
            // process only the required viewport in this texture. Unfortunately OpenWalnut does not provide an easy to
            // use mechanism for this.
            // So, we skip hemispherePoints outside the texture. This yields nearly no or only small artifacts at the
            // borders if zoomed in.
            if( hemispherePoint.x < 0.0 || hemispherePoint.x > 1.0 || hemispherePoint.y < 0.0
                || hemispherePoint.y > 1.0 )
            {
                continue;
            }

            // Count the samples we really use.
            ++numSamplesAdded;

            // use LOD based on current hemisphere?
            float lod = 0.0;

            if( u_useGauss == 1 ) // true
            {
                lod = float( l );
            }
            // get the depth of the occluder fragment
            occluderDepth = getDepth( hemispherePoint.xy, lod );

            // get the normal of the occluder fragment
            occluderNormal = getNormal( hemispherePoint.xy, lod );

            // if depthDifference is negative = occluder is behind the current fragment -> occluding this fragment
            depthDifference = currentPixelDepth - occluderDepth;

            // calculate the difference between the normals as a weight. This weight determines how much the occluder
            // occludes the fragment
            float pointDiffuse = max( dot( hemisphereVector, normal ),
                                      0.0 ); // this equals the diffuse term in Phong if lightDir is the
                                             // occluder's surface

            // calculate the diffuse reflected light of the occluder, which might contribute to the current pixels
            // brightness
            vec3 t = getTangent( hemispherePoint.xy, lod );
            vec3 newnorm = normalize( cross( normalize( cross( t, normalize( hemisphereVector ) ) ), t ) );

            // vec3 newnorm = normalize(texture( gNDMap, where ).rgb * 2.0 - 1.0);
            float occluderDiffuse = max( dot( newnorm, u_lightPosition ), 0.0 );

            // you can disable this effect.
            // float occluderDiffuse = 0.0;

            // incorporate specular reflection
            vec3 H = normalize( u_lightPosition + normalize( hemisphereVector ) );
            float occluderSpecular = pow( max( dot( H, occluderNormal ), 0.0 ), 100.0 );

            // this second is as proposed for AO, the omega (hemisphere vector) and surface normal
            normalDifference = pointDiffuse * ( occluderSpecular + occluderDiffuse );
            normalDifference
                = 1.5 - normalDifference; // we use 2 here as occluderSpecular + occluderDiffuse might get larger than 1

            // the higher the depth difference, the less it should influence the occlusion value since large space
            // between geometry normally allows
            // many light. It somehow can be described with "shadowiness". In other words, it describes the density of
            // geometry and its influence to
            // the occlusion.
            float scaler = 1.0 - ( l / ( float( SCALES - 1 ) ) );
            float densityInfluence = scaler * scaler * u_lineAODensityWeight;

            //  This reduces the occlusion if the occluder is far away
            float densityWeight = 1.0 - smoothstep( falloff, densityInfluence, depthDifference );
            // This is the same as:
            // float e0 = falloff;
            // float e1 = densityInfluence;
            // float r = ( depthDifference - e0 ) / ( e1 - e0 );
            // float densityWeight = 1.0 - smoothstep( 0.0, 1.0, r );

            // the following step function ensures that negative depthDifference values get clamped to 0, meaning that
            // this occluder should have NO
            // influence on the occlusion value. A positive value (fragment is behind the occluder) increases the
            // occlusion factor according to the
            // normal weight and density weight
            occlusionStep += normalDifference * densityWeight * step( falloff, depthDifference );
        }

        // for this radius, add to total occlusion. Keep in mind to normalize the occlusion term according to the number
        // of samples taken
        occlusion += ( 1.0 / float( numSamplesAdded ) ) * occlusionStep; /** occlusionStep*/
    }

    // we need to take care when the number of hemispheres is increased. This is done indirectly by calculation of the
    // densityInfluence variable.
    // But we need some additional scaling to keep the overall image intensity (overall energy) when changing the numbr
    // of hemispheres.
    // This usually would be done by using 1/SCALERS.
    float occlusionScalerFactor = 1.0 / ( SCALES );

    // also allow the user to modify the strength if he likes
    occlusionScalerFactor *= u_lineAOTotalStrength;

    // output the result
    return clamp( ( 1.0 - ( occlusionScalerFactor * occlusion ) ), 0, 1 );
}

void main()
{
    vec4 color = getColor( frag_texCoords );

    if( color.a < 0.01 )
    {
        discard;
    }

    out_color = vec4( color.rgb * getLineAO( frag_texCoords ), 1.0 );
    gl_FragDepth = getDepth( frag_texCoords );
}
