#pragma once

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define BUFFER_SIZE 4096

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using mat4 = glm::dmat4;
using vec4 = glm::dvec4;
using vec3 = glm::dvec3;
using vec2 = glm::dvec2;
using quat = glm::quat;

namespace Colors {
	const glm::u8vec3 Red( 255, 0, 0);
	const glm::u8vec3 Green( 0, 255, 0);
	const glm::u8vec3 Blue( 0, 0, 255);
};

// TYPEDEFS ----------------------------------------------------------------------
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;



struct Point {
	vec3 p;
	Point(const vec3& v3) : p(v3) {}
};

inline Point operator*(const mat4& m, const Point& p) {
	const auto v3 = m * vec4(p.p, 1);
	return Point(vec3(v3) / v3.w);
}

struct Vector {
	vec3 v;
	Vector(const vec3& v3) : v(v3) {}
};

inline Vector operator*(const mat4& m, const Vector& v) { return Vector(vec3(m * vec4(v.v, 0))); }

struct Plane {
	Vector normal;
	Point point;
	double distance(const Point& p) const { return glm::dot(normal.v, p.p - point.p); }
	Plane(const Vector& normal, const Point& point) : normal(normal), point(point) {}
};

inline Plane operator*(const mat4& m, const Plane& plane) { return Plane(m * plane.normal, m * plane.point); }
