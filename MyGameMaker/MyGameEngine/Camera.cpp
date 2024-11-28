// Camera.cpp
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

glm::dmat4 Camera::projection() const {
    return glm::perspective(fov, aspect, zNear, zFar);
}

glm::dmat4 Camera::view() const {
    const auto& transform = getOwner()->GetComponent<Transform>();
    return glm::lookAt(transform->pos(), transform->pos() + transform->fwd(), transform->up());
}
Transform& Camera::transform() const {
    return *(getOwner()->GetComponent<Transform>());
}
