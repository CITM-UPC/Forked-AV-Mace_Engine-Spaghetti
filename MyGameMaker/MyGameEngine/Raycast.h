#pragma once
#ifndef RAYCAST_H
#define RAYCAST_H

#include <glm/glm.hpp> // For vector mathematics

class Raycast {
public:
    glm::vec3 origin;    // Starting point of the ray
    glm::vec3 direction; // Normalized direction of the ray

    // Constructor
    Raycast(const glm::vec3& origin, const glm::vec3& direction);

    // A function to get a point along the ray at a given distance
    glm::vec3 getPoint(float distance) const;
};

#endif