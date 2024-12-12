#include "Raycast.h"

// Constructor
Raycast::Raycast(const glm::vec3& origin, const glm::vec3& direction)
    : origin(origin), direction(glm::normalize(direction)) {
}

// Get a point along the ray at a specified distance
glm::vec3 Raycast::getPoint(float distance) const {
    return origin + direction * distance;
}