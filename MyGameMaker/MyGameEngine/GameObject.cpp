#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Camera.h"
#include "Material.h"
#include "BoundingBox.h"

GameObject::GameObject(const std::string& name, const std::string& tag, bool active) : _name(name), _tag(tag), _active(active)
{
	this->AddComponent<Transform>();
	this->AddComponent<Camera>();
}



bool GameObject::operator==(const GameObject& other) const
{
	// Compare name, tag, and active state
	if (_name != other._name || _tag != other._tag || _active != other._active) {
		return false;
	}

	// Compare components by checking each component type
	if (_components.size() != other._components.size()) {
		return false;
	}

	for (size_t i = 0; i < _components.size(); ++i) {
		if (*_components[i] != *other._components[i]) {
			return false;
		}
	}



	return true;
}
