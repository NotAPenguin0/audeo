#ifndef AUDEO_VEC3_HPP_
#define AUDEO_VEC3_HPP_

#include "export_import.hpp"

namespace audeo {

struct vec3f {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

AUDEO_API vec3f operator-(vec3f const& lhs, vec3f const& rhs);
AUDEO_API vec3f operator*(vec3f const& lhs, float scalar);

AUDEO_API float magnitude(vec3f v);

AUDEO_API vec3f normalize(vec3f v);

// Calculates the cross product between two vectors
AUDEO_API vec3f cross(vec3f const& lhs, vec3f const& rhs);

// Calculates the dot product of two vectors
AUDEO_API float dot(vec3f const& lhs, vec3f const& rhs);

// Finds the angle (in degrees) between two vectors.
AUDEO_API float angle(vec3f const& lhs, vec3f const& rhs);

} // namespace audeo

#endif
