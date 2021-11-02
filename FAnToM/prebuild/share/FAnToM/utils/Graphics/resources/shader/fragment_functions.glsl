#version 330

vec4 phong(vec3 color, vec3 n, vec3 l, vec3 material, mat3x3 l_color, float s) {
    vec3 ambient = material.x * color * l_color[0];
    vec3 diffuse = material.y * max(dot(n,l), 0.)  * color * l_color[1];
    vec3 specular = material.z * pow(abs(dot(l, n)), s) * l_color[2];

    return vec4(ambient + diffuse + specular, 1.0);
}

float depth(vec3 intersection, mat4 proj) {
    float far=gl_DepthRange.far;
    float near=gl_DepthRange.near;

    vec4 clip_space_pos = proj * vec4(intersection, 1.0);
    float ndc_depth = clip_space_pos.z / clip_space_pos.w;

    return (((far-near) * ndc_depth) + near + far) / 2.0;
}

vec4 conjugate(vec4 quat) {
    return vec4(quat.x, -quat.y, -quat.z, -quat.w);
}

vec4 negate(vec4 quat) {
    return -conjugate(quat);
}

vec4 mult_quat(vec4 quat1, vec4 quat2) {
    float s1 = quat1.x;
    float s2 = quat2.x;
    vec3 v1 = quat1.yzw;
    vec3 v2 = quat2.yzw;
    return vec4(s1 * s2 - dot(v1, v2), s2 * v1 + s1 * v2 + cross(v1, v2));
}

vec3 rotate(vec3 point, vec4 rot_quaternion) {
    vec4 p_quat = vec4(0, point);
    float phi = rot_quaternion.x;
    vec3 axis = rot_quaternion.yzw;

    vec4 unit_rot = vec4(cos(phi / 2), sin(phi / 2) * axis);
    vec4 unit_inv = conjugate(unit_rot);

    vec4 rotated = mult_quat(unit_rot, mult_quat(p_quat, unit_inv));

    return rotated.yzw;
}
