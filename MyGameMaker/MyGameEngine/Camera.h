#pragma once

#include "Transform.h"
#include "Component.h"
#include <glm/glm.hpp>

class Camera : public Component
{
public:
	bool isActiveCamera = false;

	double fov = glm::radians(60.0);
	double aspect = 16.0 / 9.0;
	double zNear = 0.1;
	double zFar = 128.0;
	float speed = 10.0f;

	Camera(GameObject* owner, bool active = true)
		: Component(active, owner) {
	}


	glm::dmat4 projection() const;
	glm::dmat4 view() const;

    Transform& transform() const;

    // Additional camera methods...
};


