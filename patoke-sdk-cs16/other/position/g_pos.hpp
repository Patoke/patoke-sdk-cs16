#pragma once
// g_pos stands for global position, this is the global position header

// imgui declarations
#define IM_VEC2_CLASS_EXTRA \
        constexpr ImVec2(const s_vec2& f) : x(f.x), y(f.y) {}

#define IM_VEC4_CLASS_EXTRA \
		constexpr ImVec4(const s_rect& f) : x(f.x), y(f.y), z(f.width), w(f.height) {}

// raw defines for position and size, these don't have any usage more than define the variables they contain
struct s_position {
	float x, y;

	s_position(float x, float y) : x(x), y(y) {}
};

struct s_position3d : public s_position {
	float z;

	s_position3d(float x, float y, float z) : s_position(x, y), z(z) {}
};

struct s_size {
	float width, height;

	s_size(float width, float height) : width(width), height(height) {}
};

// usage defines, these contain all the helpers for these specific types unlike raw defines
struct s_rect : public s_position, s_size {
	s_rect(float x, float y, float width, float height) : s_position(x, y), s_size(width, height) {}
};

struct s_vec2 : public s_position {
	s_vec2(float x, float y) : s_position(x, y) {}
};

struct s_vec3 : public s_position3d {
	s_vec3(float x, float y, float z) : s_position3d(x, y, z) {}
};