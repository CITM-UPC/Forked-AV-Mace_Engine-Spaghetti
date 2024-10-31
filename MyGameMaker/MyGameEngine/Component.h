#pragma once

#include "GameObject.h"
#include <string>

class GameObject;

enum class ComponentType
{
	Transform,
	Mesh,
	Camera,
	Material,

	Light,
	Script,
	Audio,
	Physics,
	Animator,
	Particle,
	Count,

	Invalid = -1
};

class Component
{
	bool _active;
	GameObject* _owner;
	ComponentType _type;

public:
	Component() : _active(true), _owner(nullptr), _type(ComponentType::Invalid) {}
	Component(bool active, GameObject* owner) : _active(active), _owner(owner), _type(ComponentType::Invalid)  {}
	virtual ~Component() = default;

	GameObject* getOwner() const { return _owner; }
	bool isActive() const { return _active; }
	void setActive(bool active) { this->_active = active; }
	ComponentType type() const { return _type; }

	virtual bool operator==(const Component& other) const {	return _active == other._active && _owner == other._owner && _type == other._type; }
	virtual bool operator!=(const Component& other) const {	return !(*this == other); }
};
