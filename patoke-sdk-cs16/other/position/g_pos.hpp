#pragma once
// g_pos stands for global position, this is the global position header

// imgui declarations
#define IM_VEC2_CLASS_EXTRA                                                                                                                          \
    constexpr ImVec2(const s_vec2 &f) : x(f.x), y(f.y) {}                                                                                            \
    constexpr ImVec2(const s_position &f) : x(f.x), y(f.y) {}                                                                                        \
    operator s_vec2() const { return s_vec2(x, y); }                                                                                                 \
    operator s_position() const { return s_position(x, y); }

#define IM_VEC4_CLASS_EXTRA                                                                                                                          \
    constexpr ImVec4(const s_rect &f) : x(f.x), y(f.y), z(f.width), w(f.height) {}                                                                   \
    operator s_rect() const { return s_rect(x, y, w, z); }

// raw defines for position and size, these don't have any usage more than define the variables they contain and operators
struct s_position {
    float x, y;

    s_position(float x, float y) : x(x), y(y) {}

    float& operator[](int i) {
        return ((float*)this)[i];
    }

    float operator[](int i) const {
        return ((float*)this)[i];
    }

    inline s_position operator+(const s_position& other) const {
        return s_position(this->x + other.x, this->y + other.y);
    }

    inline s_position operator-(const s_position& other) const {
        return s_position(this->x - other.x, this->y - other.y);
    }

    inline s_position operator/(const float& val) const {
        return s_position(this->x / val, this->y / val);
    }

    inline s_position operator-=(const s_position& other) {
        this->x -= other.x;
        this->y -= other.y;
        return *this;
    }
};

struct s_position3d : public s_position {
    float z;

    s_position3d(float x, float y, float z) : s_position(x, y), z(z) {}

    inline s_position3d operator+(const s_position3d& other) const {
        return s_position3d(this->x + other.x, this->y + other.y, this->z + other.z);
    }

    inline s_position3d operator-(const s_position3d& other) const {
        return s_position3d(this->x - other.x, this->y - other.y, this->z - other.z);
    }

    inline s_position3d operator-=(const s_position3d& other) {
        this->x -= other.x;
        this->y -= other.y;
        this->z -= other.z;
        return *this;
    }

    inline s_position3d operator*=(const s_position3d& other) {
        this->x *= other.x;
        this->y *= other.y;
        this->z *= other.z;
        return *this;
    }

    inline s_position3d operator/(const float& val) const {
        return s_position3d(this->x / val, this->y / val, this->z / val);
    }

    inline s_position3d operator*=(const float& val) {
        this->x *= val;
        this->y *= val;
        this->z *= val;
        return *this;
    }

    inline float length() const {
        return sqrt((x * x) + (y * y) + (z * z));
    }

    inline float length_sqr() const {
        return ((x * x) + (y * y) + (z * z));
    }

    inline float length_2d() const {
        return sqrt((x * x) + (y * y));
    }

    inline bool is_zero(float tolerance) const {
        return (x > -tolerance && x < tolerance&& y > -tolerance && y < tolerance&& z > -tolerance && z < tolerance);
    }

    inline bool is_valid() const {
        return (std::isfinite(x) && std::isfinite(y) && std::isfinite(z));
    }
};

struct s_size {
    float width, height;

    s_size(float width, float height) : width(width), height(height) {}

    inline s_size operator+(const s_size& other) const {
        return s_size(this->width + other.width, this->height + other.height);
    }

    inline s_size operator-(const s_size& other) const {
        return s_size(this->width - other.width, this->height - other.height);
    }

    inline s_size operator/(const float& val) const {
        return s_size(this->width / val, this->height / val);
    }

    inline s_size operator-=(const s_size& other) {
        this->width -= other.width;
        this->height -= other.height;
        return *this;
    }
};

// usage defines, these contain all the helpers for these specific types unlike raw defines
struct s_rect : public s_position, s_size {
    s_rect() : s_position(0, 0), s_size(0, 0) {}
    s_rect(float x, float y, float width, float height) : s_position(x, y), s_size(width, height) {}
    s_rect(const s_position& other_pos, const s_size& other_size)
        : s_position(other_pos.x, other_pos.y), s_size(other_size.width, other_size.height) {}
};

struct s_vec2 : public s_position {
    s_vec2() : s_position(0, 0) {}
    s_vec2(float x, float y) : s_position(x, y) {}
    s_vec2(int x, int y) : s_position(float(x), float(y)) {}
    s_vec2(const s_position& other) : s_vec2(other.x, other.y) {}
};

struct s_angle : public s_position3d {
    s_angle() : s_position3d(0, 0, 0) {}
    s_angle(float x, float y, float z) : s_position3d(x, y, z) {}
    s_angle(const s_position3d& other) : s_angle(other.x, other.y, other.z) {}

    inline s_angle normalize();
};

struct s_vec3 : public s_position3d {
    s_vec3() : s_position3d(0, 0, 0) {}
    s_vec3(float x, float y, float z) : s_position3d(x, y, z) {}
    s_vec3(const s_position3d& other) : s_vec3(other.x, other.y, other.z) {}
    s_vec3(const float* other) : s_vec3(other[0], other[1], other[2]) {}

    inline float dot(const s_vec3& other) const {
        const s_vec3& a = *this;

        return (a.x * other.x + a.y * other.y + a.z * other.z);
    }

    inline s_vec3 normalize();
    inline float normalize_in_place();
};

// source engine specific
class __declspec(align(16)) s_vec_aligned : public s_vec3 {
public:
    inline s_vec_aligned(void) : s_vec3(0, 0, 0) {}
    inline s_vec_aligned(float _x, float _y, float _z) : s_vec3(_x, _y, _z) {}
    explicit s_vec_aligned(const s_vec3& other) : s_vec3(other.x, other.y, other.z) {}

    s_vec_aligned& operator=(const s_vec3& other) {
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;
        return *this;
    }

    float w;
};

// matrix defines, these are separated from everything else due to their nature
// used for bone matrices and stuff
class s_matrix3x4 {
public:
    s_matrix3x4() {
        mat_val[0][0] = 0;
        mat_val[0][1] = 0;
        mat_val[0][2] = 0;
        mat_val[0][3] = 0;
        mat_val[1][0] = 0;
        mat_val[1][1] = 0;
        mat_val[1][2] = 0;
        mat_val[1][3] = 0;
        mat_val[2][0] = 0;
        mat_val[2][1] = 0;
        mat_val[2][2] = 0;
        mat_val[2][3] = 0;
    }

    s_matrix3x4(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23) {
        mat_val[0][0] = m00;
        mat_val[0][1] = m01;
        mat_val[0][2] = m02;
        mat_val[0][3] = m03;
        mat_val[1][0] = m10;
        mat_val[1][1] = m11;
        mat_val[1][2] = m12;
        mat_val[1][3] = m13;
        mat_val[2][0] = m20;
        mat_val[2][1] = m21;
        mat_val[2][2] = m22;
        mat_val[2][3] = m23;
    }

    inline s_matrix3x4 operator=(float** other) {
        mat_val[0][0] = other[0][0];
        mat_val[0][1] = other[0][1];
        mat_val[0][2] = other[0][2];
        mat_val[0][3] = other[0][3];
        mat_val[1][0] = other[1][0];
        mat_val[1][1] = other[1][1];
        mat_val[1][2] = other[1][2];
        mat_val[1][3] = other[1][3];
        mat_val[2][0] = other[2][0];
        mat_val[2][1] = other[2][1];
        mat_val[2][2] = other[2][2];
        mat_val[2][3] = other[2][3];

        return *this;
    }

    void init(const s_vec3& x_axis, const s_vec3& y_axis, const s_vec3& z_axis, const s_vec3& vec_origin) {
        mat_val[0][0] = x_axis.x;
        mat_val[0][1] = y_axis.x;
        mat_val[0][2] = z_axis.x;
        mat_val[0][3] = vec_origin.x;
        mat_val[1][0] = x_axis.y;
        mat_val[1][1] = y_axis.y;
        mat_val[1][2] = z_axis.y;
        mat_val[1][3] = vec_origin.y;
        mat_val[2][0] = x_axis.z;
        mat_val[2][1] = y_axis.z;
        mat_val[2][2] = z_axis.z;
        mat_val[2][3] = vec_origin.z;
    }

    s_matrix3x4(const s_vec3& x_axis, const s_vec3& y_axis, const s_vec3& z_axis, const s_vec3& vec_origin) {
        init(x_axis, y_axis, z_axis, vec_origin);
    }

    inline void set_origin(s_vec3 const& p) {
        mat_val[0][3] = p.x;
        mat_val[1][3] = p.y;
        mat_val[2][3] = p.z;
    }

    inline void invalidate(void) {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 4; j++)
                mat_val[i][j] = std::numeric_limits<float>::infinity();
        ;
    }

    float* operator[](int i) {
        return mat_val[i];
    }
    const float* operator[](int i) const {
        return mat_val[i];
    }
    float* base() {
        return &mat_val[0][0];
    }
    const float* base() const {
        return &mat_val[0][0];
    }

    float mat_val[3][4];
};

// used for stuff like the viewmatrix and stuff (this is also known in the source engine as vmatrix)
class s_matrix4x4 {
public:
    s_matrix4x4();
    s_matrix4x4(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33);

    s_matrix4x4(const s_vec3& forward, const s_vec3& left, const s_vec3& up);

    s_matrix4x4(const s_matrix3x4& matrix);

    void init(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33);

    void init(const s_matrix3x4& matrix);

    inline float* operator[](int i) {
        return m[i];
    }

    inline const float* operator[](int i) const {
        return m[i];
    }

    inline float* base() {
        return &m[0][0];
    }

    inline const float* base() const {
        return &m[0][0];
    }
public:
    float m[4][4];
};