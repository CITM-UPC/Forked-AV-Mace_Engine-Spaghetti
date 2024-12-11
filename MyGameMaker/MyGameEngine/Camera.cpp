#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/gtx/rotate_vector.hpp>

glm::dmat4 Camera::projection() const {
	return glm::perspective(_fov, _aspect, zNear, zFar);
}

glm::dmat4 Camera::view() const {
	return glm::lookAt(this->getOwner()->GetComponent<Transform>()->pos(), this->getOwner()->GetComponent<Transform>()->pos() + this->getOwner()->GetComponent<Transform>()->fwd(), this->getOwner()->GetComponent<Transform>()->up());
}

std::list<Plane> Camera::frustumPlanes() const {
	const double h_fov = glm::radians(_fov);
	const double v_fov = glm::radians(_fov / _aspect);
	return {
		//near
		Plane(_transform.fwd(), _transform.pos() + _transform.fwd() * zNear),
		//far
		Plane(-_transform.fwd(), _transform.pos() + _transform.fwd() * zFar),
		//left
		Plane(glm::rotate(_transform.left(), h_fov, _transform.up()), _transform.pos()),
		//right
		Plane(glm::rotate(_transform.left(), -h_fov, _transform.up()), _transform.pos()),
		//top
		Plane(glm::rotate(_transform.up(), -v_fov, _transform.left()), _transform.pos()),
		//bottom
		Plane(glm::rotate(_transform.up(), v_fov, _transform.left()), _transform.pos())
	};
}

bool Camera::isAABBInFrustum(const glm::dvec3& min, const glm::dvec3& max) const {
	auto planes = frustumPlanes();
	for (const auto& plane : planes) {
		if (plane.distance(min) > 0 && plane.distance(max) > 0) {
			return false;
		}
	}
	return true;
}

std::pair<glm::vec3, glm::vec3> GetRayFromMouse(int mouseX, int mouseY,
	const glm::mat4& projection,
	const glm::mat4& view,
	const glm::ivec2& viewportSize) {
	// Coordenadas del mouse a NDC
	float ndcX = (2.0f * mouseX) / viewportSize.x - 1.0f;
	float ndcY = 1.0f - (2.0f * mouseY) / viewportSize.y; // OpenGL invierte el eje Y

	// Rayo en espacio de clip
	glm::vec4 rayClip(ndcX, ndcY, -1.0f, 1.0f);

	// Transformar a espacio de vista
	glm::vec4 rayEye = glm::inverse(projection) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f); // Convertir a vector (w = 0)

	// Transformar a espacio mundial
	glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(view) * rayEye));
	glm::vec3 rayOrigin = glm::vec3(glm::inverse(view) * glm::vec4(0, 0, 0, 1));

	return { rayOrigin, rayWorld };
}