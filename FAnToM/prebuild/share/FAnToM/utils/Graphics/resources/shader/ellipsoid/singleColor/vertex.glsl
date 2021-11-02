#version 330 core

// FAnToM uniforms
uniform mat4 model;
uniform mat4 view;

// vertex attributes
in vec3 in_position;
in vec3 in_size;
in vec4 in_rotation;

// attributes forwarded to geometry shader
out mat4 geom_transform;
out mat4 geom_transform_inv;
out float geom_billboardSize;
out float geom_minScale;

mat4 quaternionToMatrix( vec4 quat )
{
    float x2 = quat.x * quat.x;
    float y2 = quat.y * quat.y;
    float z2 = quat.z * quat.z;
    float xy = quat.x * quat.y;
    float xz = quat.x * quat.z;
    float yz = quat.y * quat.z;
    float wx = quat.w * quat.x;
    float wy = quat.w * quat.y;
    float wz = quat.w * quat.z;

    return mat4(
           1.0 - 2.0 * ( y2 + z2 ),       2.0 * ( xy + wz ),       2.0 * ( xz - wy ), 0.0, // first column
                 2.0 * ( xy - wz ), 1.0 - 2.0 * ( x2 + z2 ),       2.0 * ( yz + wx ), 0.0, // second column
                 2.0 * ( xz + wy ),       2.0 * ( yz - wx ), 1.0 - 2.0 * ( x2 + y2 ), 0.0, // third column
                               0.0,                     0.0,                     0.0, 1.0  // fourth column
    );
}

mat4 scaleMatrix( vec3 scale )
{
    return mat4(
           scale.x,     0.0,     0.0, 0.0,
               0.0, scale.y,     0.0, 0.0,
               0.0,     0.0, scale.z, 0.0,
               0.0,     0.0,     0.0, 1.0
    );
}

mat4 translationMatrix( vec3 pos )
{
    return mat4(
             1.0,   0.0,   0.0, 0.0,
             0.0,   1.0,   0.0, 0.0,
             0.0,   0.0,   1.0, 0.0,
           pos.x, pos.y, pos.z, 1.0
    );
}


void main()
{
    geom_minScale = min( min( in_size.x, in_size.y ), in_size.z );

    vec3 relScale = vec3(in_size.x/geom_minScale, in_size.y/geom_minScale, in_size.z/geom_minScale);

    // compute transformation matrix that makes a rotated ellipsoid out of a sphere by scaling and rotation
    mat4 rotation = quaternionToMatrix( in_rotation );

    geom_transform = translationMatrix( in_position )
                   * rotation
                   * scaleMatrix( relScale );

    geom_transform_inv = scaleMatrix( 1.0 / relScale )
                       * transpose( rotation )
                       * translationMatrix( -in_position );

    geom_billboardSize = max( max( in_size.x, in_size.y ), in_size.z );

    // compute position without projection as it is performed in the geometry shader
    gl_Position = view * model * vec4( in_position, 1.0 );
}
