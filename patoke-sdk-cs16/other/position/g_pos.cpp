#include "g_pos.hpp"
#include <math/math.hpp>

#pragma region s_angle
inline s_angle s_angle::normalize() {
    this->x = math_i.normalize_angle(this->x);
    this->y = math_i.normalize_angle(this->y);
    this->z = this->z < 50 && this->z > -50 ? this->z : 0.f;

    return *this;
}
#pragma endregion

#pragma region s_vec3
inline s_vec3 s_vec3::normalize() {
    this->x = std::isfinite(this->x) ? std::remainderf(this->x, 360.0f) : 0.0f;
    this->y = std::isfinite(this->y) ? std::remainderf(this->y, 360.0f) : 0.0f;
    this->z = 0.0f;

    return *this;
}

inline float s_vec3::normalize_in_place() {
    float radius = sqrt(x * x + y * y + z * z);

    // FLT_EPSILON is added to the radius to eliminate the possibility of division by zero (syn tax eror :3)
    float iradius = 1.f / (radius + FLT_EPSILON);

    this->x *= iradius;
    this->y *= iradius;
    this->z *= iradius;

    return radius;
}
#pragma endregion