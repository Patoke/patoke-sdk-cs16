#pragma once
#include <precompiled.hpp>
#include <position/g_pos.hpp>

// angle functions
constexpr float rad2deg(const float x) {
    return x * (180.f / std::numbers::pi_v<float>);
}

constexpr float deg2rad(const float x) {
    return x * (std::numbers::pi_v<float> / 180.f);
}

struct s_math {
    inline void sin_cos(float radians, float* sine, float* cosine) const;
    inline float normalize_angle(float angle, float start = -180.f, float end = 180.f) const;
    s_angle calculate_angle(const s_vec3& source, const s_vec3& destination);
    s_angle calculate_angle(const s_vec3& source, const s_vec3& destination, const s_angle& viewangles);
    s_vec3 vector_transform(const s_vec3& in_vec, const s_matrix3x4& in_matrix, s_vec3& out);
};

extern s_math math_i;