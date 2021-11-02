#version 330 core

// FAnToM uniforms
uniform mat4 model;
uniform mat4 view;

// vertex attributes
in vec3 in_position;

// attributes forwarded to geometry shader
out vec3 geom_position;


void main()
{
    // forward color
    geom_position = 
    geom_color = in_color;

    // compute transformation matrix that makes a rotated ellipsoid out of a sphere by scaling and rotation
    mat4 rotation = quaternionToMatrix( in_rotation );

    geom_transform = translationMatrix( in_position )
                   * rotation
                   * scaleMatrix( in_size );

    geom_transform_inv = scaleMatrix( 1.0 / in_size )
                       * transpose( rotation )
                       * translationMatrix( -in_position );

    geom_billboardSize = max( max( in_size.x, in_size.y ), in_size.z );

    // compute position without projection as it is performed in the geometry shader
    gl_Position = view * model * vec4( in_position, 1.0 );
}
