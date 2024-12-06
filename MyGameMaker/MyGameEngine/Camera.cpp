#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

glm::dmat4 Camera::projection() const {
	return glm::perspective(_fov, _aspect, zNear, zFar);
}

glm::dmat4 Camera::view() const {
	return glm::lookAt(this->getOwner()->GetComponent<Transform>()->pos(), this->getOwner()->GetComponent<Transform>()->pos() + this->getOwner()->GetComponent<Transform>()->fwd(), this->getOwner()->GetComponent<Transform>()->up());
}

std::list<Plane> Camera::frustumPlanes() const {

    // Get the combined projection-view matrix
    glm::dmat4 projectionMatrix = projection();
    glm::dmat4 viewMatrix = view();
    glm::dmat4 pvMatrix = projectionMatrix * viewMatrix;

    std::list<Plane> planes;

    // Extract planes from the projection-view matrix
    // Left Plane
    planes.emplace_back(
        Vector(glm::normalize(glm::dvec3(pvMatrix[0][3] + pvMatrix[0][0],
            pvMatrix[1][3] + pvMatrix[1][0],
            pvMatrix[2][3] + pvMatrix[2][0]))),
        Point(glm::dvec3(0))); // Point is less critical, often just a placeholder

    // Right Plane
    planes.emplace_back(
        Vector(glm::normalize(glm::dvec3(pvMatrix[0][3] - pvMatrix[0][0],
            pvMatrix[1][3] - pvMatrix[1][0],
            pvMatrix[2][3] - pvMatrix[2][0]))),
        Point(glm::dvec3(0)));

    // Bottom Plane
    planes.emplace_back(
        Vector(glm::normalize(glm::dvec3(pvMatrix[0][3] + pvMatrix[0][1],
            pvMatrix[1][3] + pvMatrix[1][1],
            pvMatrix[2][3] + pvMatrix[2][1]))),
        Point(glm::dvec3(0)));

    // Top Plane
    planes.emplace_back(
        Vector(glm::normalize(glm::dvec3(pvMatrix[0][3] - pvMatrix[0][1],
            pvMatrix[1][3] - pvMatrix[1][1],
            pvMatrix[2][3] - pvMatrix[2][1]))),
        Point(glm::dvec3(0)));

    // Near Plane
    planes.emplace_back(
        Vector(glm::normalize(glm::dvec3(pvMatrix[0][3] + pvMatrix[0][2],
            pvMatrix[1][3] + pvMatrix[1][2],
            pvMatrix[2][3] + pvMatrix[2][2]))),
        Point(glm::dvec3(0)));

    // Far Plane
    planes.emplace_back(
        Vector(glm::normalize(glm::dvec3(pvMatrix[0][3] - pvMatrix[0][2],
            pvMatrix[1][3] - pvMatrix[1][2],
            pvMatrix[2][3] - pvMatrix[2][2]))),
        Point(glm::dvec3(0)));

    return planes;
}

