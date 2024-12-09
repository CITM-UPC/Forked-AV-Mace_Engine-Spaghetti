#ifndef __SCENE_H__
#define __SCENE_H__
#pragma once

#include "GameObject.h"

#include <string>
#include <memory>

#include "Camera.h"

#include <vector>

class Scene
{
	std::string _name;
	bool _active;
	GameObject* _root = new GameObject("SceneRoot");
	GameObject _camera = GameObject("EditorCamera", "Camera");
	

public:

	Scene(const std::string& name, bool active = true) : _name(name), _active(active) {}
	~Scene() {}

	std::vector<GameObject> gameObjects; 

	
	GameObject* root() { return _root; }
	GameObject* camera() { return &_camera; }

	void Start();
	void Update(double& dT);
	void PostUpdate();
	void CleanUp();
	void OnSceneChange();
	void Draw(GameObject* root);

	void loadGameObjectByPath(const std::string& path);
	void loadTextureByPath(const std::string& path);

	GameObject* selectedGameObject = nullptr;

	std::shared_ptr<GameObject> CreateGameObject(const std::string& name = "GameObject");
	void InitCamera();
	void CreateCube();
	void CreatePlane();
	void CreateSphere();
	void CreateCylinder();
	void CreateCone();
	void CreateTorus();

};

#endif // !__SCENE_H__