#pragma once

#include "Transform.h"
#include "types.h"

class Camera : public Component
{
	double _fov = glm::radians(60.0);
	double _aspect = 16.0 / 9.0;
	double zNear = 0.1;
	double zFar = 128.0;
	float _speed = 10.0f;
	Transform _transform;

public:

	Camera(bool active, GameObject* owner) : Component(active, owner) {}
	~Camera() {}

	const double fov() const { return _fov; };
	const double aspect() const { return _aspect; };
	const double near() const { return zNear; };
	const double far() const { return zFar; };
	const float speed() const { return _speed; };
	double& fov() { return _fov; };
	double& aspect() { return _aspect; };
	double& near() { return zNear; }
	double& far() { return zFar; }
	float& speed() { return _speed; };

    std::list<Plane> frustumPlanes() const;

	glm::dmat4 projection() const;
	glm::dmat4 view() const;

	
	bool isAABBInFrustum(const glm::dvec3& min, const glm::dvec3& max) const;

};

