#version 330 core

// FAnToM uniforms
uniform mat4 model;
uniform mat4 view;

// vertex attributes
in vec3 in_position;
in float in_size;

// attributes forwarded to geometry shader
out mat4 geom_transform;
out mat4 geom_transform_inv;
out float geom_billboardSize;


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
    // compute transformation matrix that makes a rotated ellipsoid out of a sphere by scaling and rotation
    geom_transform = translationMatrix( in_position );

    geom_transform_inv = translationMatrix( -in_position );

    geom_billboardSize = in_size;

    // compute position without projection as it is performed in the geometry shader
    gl_Position = view * model * vec4( in_position, 1.0 );
}
