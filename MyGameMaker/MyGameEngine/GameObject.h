#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__
#pragma once

#include <string>
#include <vector>
#include "TreeExtension.h"
#include "Component.h"

class Component;

class GameObject : public TreeExtension<GameObject>
{
	std::string _name;
	std::string _tag;
	bool _active;
	std::vector<Component*> _components;

public:
	GameObject(const std::string& name, const std::string& tag = "Untagged", bool active = true);
	~GameObject() {}

	auto& name() { return _name; }
	auto& tag() { return _tag; }
	
	auto& isActive() { return _active; }
	virtual bool SetActive(bool active) { return this->_active = active; }
	virtual bool SwitchState() { return _active = !_active; }

	template <typename T>
	std::shared_ptr<GameObject> FindChildWithComponent() const {
		for (const auto& child : children()) {
			if (child->GetComponent<T>()) {
				return child;
			}
			auto found = child->FindChildWithComponent<T>();
			if (found) {
				return found;
			}
		}
		return nullptr;
	}

	template <typename T>
	T* AddComponent() {
		static_assert(std::is_base_of<Component, T>::value, "ERROR: T must inherit from Component");
		T* newComponent = new T(true, this);
		if (newComponent) _components.push_back(dynamic_cast<Component*>(newComponent));
		return newComponent;
	}

	template <typename T>
	T* GetComponent() const {
		for (Component* component : _components) {
			if (T* specificComponent = dynamic_cast<T*>(component)) {
				return specificComponent;
			}
		}
		return nullptr;
	}

	bool operator==(const GameObject& other) const;

protected:
	virtual void Start() {}
	virtual void Update() {}
	virtual void CleanUp() {}
	virtual void OnSceneChange() {}
};

#endif // __GAMEOBJECT_H__