#ifndef __SCENE_H__
#define __SCENE_H__
#pragma once

#include "GameObject.h"

#include <string>
#include <memory>

#include "Camera.h"

class Scene
{
	std::string _name;
	bool _active;
	GameObject* _root = new GameObject("SceneRoot");

public:
	Scene(const std::string& name, const std::string& tag = "Untagged", bool active = true) : _name(name), _active(active) {}
	~Scene() {}

	Camera _camera;
    GameObject* root() { return _root; }

	void Start();
	void Update(double& dT);
	void PostUpdate();
	void CleanUp();
	void OnSceneChange();
	void Draw(GameObject* root);




	void loadGameObjectByPath(const std::string& path);
	void loadTextureByPath(const std::string& path);

	GameObject* selectedGameObject = nullptr;

	std::shared_ptr<GameObject> CreateGameObject();
	void CreateCameraObject();
	void CreateCube();
	void CreatePlane();
	void CreateSphere();
	void CreateCylinder();
	void CreateCone();
	void CreateTorus();

private:
	std::shared_ptr<GameObject> selectedGameObject;
};

#endif // !__SCENE_H__