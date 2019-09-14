#include "audeo/vec3.hpp"

#define _USE_MATH_DEFINES
#include <cmath>

namespace audeo {

vec3f operator-(vec3f const& lhs, vec3f const& rhs) {
    return vec3f{lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}

vec3f operator*(vec3f const& lhs, float scalar) {
    return {lhs.x * scalar, lhs.y * scalar, lhs.z * scalar};
}

float magnitude(vec3f v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3f normalize(vec3f v) { return v * (1.0f / magnitude(v)); }

vec3f cross(vec3f const& lhs, vec3f const& rhs) {
    return {lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z,
            lhs.x * rhs.y - lhs.y * rhs.x};
}

float dot(vec3f const& lhs, vec3f const& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

float angle(vec3f const& lhs, vec3f const& rhs) {
    // The cosine of the angle between two vectors is equal to the dot product
    // of the vectors divided by the product of the magnitude of the vectors
    //
    // We get rid of the division by normalizing the vectors
    vec3f a = normalize(lhs);
    vec3f b = normalize(rhs);
    // The cosine is now equal to the dot product of a and b
    float cos_a = dot(a, b);
    // Convert this cosine to an angle in degrees and we are done
    return static_cast<float>(std::acos(cos_a) * 180.0f / M_PI);
}

} // namespace audeo
