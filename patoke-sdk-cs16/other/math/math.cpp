#include "math.hpp"

// create math container instance
s_math math_i;

inline void s_math::sin_cos(float radians, float* sine, float* cosine) const {
    *sine = sinf(radians);
    *cosine = cosf(radians);
}

inline float s_math::normalize_angle(float angle, float start, float end) const {
    const float width = end - start;
    const float offset_angle = angle - start;

    return (offset_angle - (floor(offset_angle / width) * width)) + start;
}

s_angle s_math::calculate_angle(const s_vec3& source, const s_vec3& destination) {
    s_vec3 delta = source - destination;
    s_angle angles;

    angles.x = rad2deg(asinf(delta.z / delta.length()));
    angles.y = rad2deg(atanf(delta.y / delta.x));
    angles.z = 0.0f;

    if (delta.x >= 0.0)
        angles.y += 180.0f;

    return angles;
}

s_angle s_math::calculate_angle(const s_vec3& source, const s_vec3& destination, const s_angle& viewangles) {
    s_vec3 delta = source - destination;
    s_angle angles;

    angles.x = rad2deg(asinf(delta.z / delta.length())) - viewangles.x;
    angles.y = rad2deg(atanf(delta.y / delta.x)) - viewangles.y;
    angles.z = 0.0f;

    if (delta.x >= 0.0)
        angles.y += 180.0f;

    return angles;
}

s_vec3 s_math::vector_transform(const s_vec3& in_vec, const s_matrix3x4& in_matrix, s_vec3& out) {
    out.x = in_vec.dot(in_matrix[0]) + in_matrix[0][3];
    out.y = in_vec.dot(in_matrix[1]) + in_matrix[1][3];
    out.z = in_vec.dot(in_matrix[2]) + in_matrix[2][3];

    return out;
}