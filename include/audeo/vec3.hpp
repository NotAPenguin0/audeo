#ifndef AUDEO_VEC3_HPP_
#define AUDEO_VEC3_HPP_

namespace audeo {

struct vec3f {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

vec3f operator-(vec3f const& lhs, vec3f const& rhs);
vec3f operator*(vec3f const& lhs, float scalar);

float magnitude(vec3f v);

vec3f normalize(vec3f v);

// Calculates the cross product between two vectors
vec3f cross(vec3f const& lhs, vec3f const& rhs);

// Calculates the dot product of two vectors
float dot(vec3f const& lhs, vec3f const& rhs);

// Finds the angle (in degrees) between two vectors.
float angle(vec3f const& lhs, vec3f const& rhs);

} // namespace audeo

#endif
