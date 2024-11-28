#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

glm::dmat4 Camera::projection() const {
	return glm::perspective(_fov, _aspect, zNear, zFar);
}

glm::dmat4 Camera::view() const {
	return glm::lookAt(this->getOwner()->GetComponent<Transform>()->pos(), this->getOwner()->GetComponent<Transform>()->pos() + this->getOwner()->GetComponent<Transform>()->fwd(), this->getOwner()->GetComponent<Transform>()->up());
}