#include "Scene.h"
#include "types.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <memory>
#include <iostream>

#include "Engine.h"
#include "Log.h"
#include "Input.h"
#include "ModelLoader.h"
#include "Mesh.h"
#include "Material.h"
#include "debugDraws.h"

//camera movement variables
bool rightMouse = false;
int lastMouseX = -1;
int lastMouseY = -1;
int mouseX = 0;
int mouseY = 0;
float yaw = 0.0f;
float pitch = 0.0f;

//camera orbit variables
bool leftMouse = false;
float horizontalAngle = 0.0f;
float verticalAngle = 0.0f;

//camera zoom variables
float fovModifier = 0;
float zoomValue = 0;
bool isZooming = false;

void Scene::Start()
{
	// Init camera
	void InitCamera();

    std::shared_ptr<GameObject> bakerHouse = CreateGameObject();

	ModelLoader modelLoader;
	std::vector<std::shared_ptr<Model>> models;
	modelLoader.load("Assets/FBX/BakerHouse.fbx", models);
	for (size_t i = 0; i < models.size(); i++)
	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>(models[i].get()->GetMeshName());
		bakerHouse->addChild(go);
		
		go->GetComponent<Transform>()->pos() = vec3(0, 0, 0);
		go->GetComponent<Transform>()->updateGlobalMatrix();
		go->AddComponent<Mesh>();
		go->GetComponent<Mesh>()->setModel(models[i]);
		go->GetComponent<Mesh>()->setFilePath("Assets/FBX/BakerHouse.fbx");
		go->AddComponent<Material>();
		go->GetComponent<Material>()->m_Texture = std::make_unique<Texture>("Assets/Textures/Baker_house.png");
		go->GetComponent<Material>()->m_Shader = std::make_unique<Shader>("Assets/Shaders/Basic.shader");
		go->GetComponent<Mesh>()->loadToOpenGL();
	}
}

void Scene::Update(double& dT)
{
	// Get the main camera
	auto cameraObject = root()->FindChildWithComponent<Camera>();
	if (!cameraObject) {
		LOG(LogType::LOG_WARNING, "No camera found in the scene.");
		return;
	}
	if (cameraObject) {
		auto camera = cameraObject->GetComponent<Camera>();
		camera->isActiveCamera = true;
	}

	auto camera = cameraObject->GetComponent<Camera>();
	auto& cameratransform = camera->transform();

	//camera speed
	if (Engine::Instance().input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		_camera.speed = 20.0f;
	}
	else {
		_camera.speed = 10.0f;
	}

	//camera rotation
	if (Engine::Instance().input->GetMouseButtonDown(3) == KEY_DOWN) {
		rightMouse = true;
		Engine::Instance().input->GetMousePosition(lastMouseX, lastMouseY);
	}
	else if (Engine::Instance().input->GetMouseButtonDown(3) == KEY_UP) {
		rightMouse = false;
	}

	if (rightMouse) {

		//camera movement
		if (Engine::Instance().input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			_camera.transform().translate(vec3(0, 0, _camera.speed * dT));
		}
		if (Engine::Instance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			_camera.transform().translate(vec3(_camera.speed * dT, 0, 0));
		}
		if (Engine::Instance().input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			_camera.transform().translate(vec3(0, 0, -_camera.speed * dT));
		}
		if (Engine::Instance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			_camera.transform().translate(vec3(-_camera.speed * dT, 0, 0));
		}

		Engine::Instance().input->GetMousePosition(mouseX, mouseY);

		float dx = (float)(mouseX - lastMouseX);
		float dy = (float)(mouseY - lastMouseY);

		// Ajusta la sensibilidad
		float sensitivity = 0.5f;
		dx *= sensitivity;
		dy *= sensitivity;

		yaw = glm::degrees(atan2(_camera.transform().fwd().z, _camera.transform().fwd().x));
		pitch = glm::degrees(atan2(_camera.transform().fwd().y, glm::sqrt(_camera.transform().fwd().x * _camera.transform().fwd().x + _camera.transform().fwd().z * _camera.transform().fwd().z)));

		yaw += dx;
		pitch -= dy;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		//actualizo el vector forward de la camera
		_camera.transform().setFwd(glm::normalize(direction));
		//uso vec3(0,1,0) porque busco el vector up en coordenadas world!!! igualmente acutalizo el vector up(local) de la camera
		_camera.transform().setRigth(glm::normalize(glm::cross(vec3(0, 1, 0), _camera.transform().fwd())));
		_camera.transform().setUp(glm::normalize(glm::cross(_camera.transform().fwd(), _camera.transform().right())));

		Engine::Instance().input->GetMousePosition(lastMouseX, lastMouseY);
	}

	//camera zoom
	Engine::Instance().input->GetMouseWheelMotion(isZooming);
	if (isZooming) {
		Engine::Instance().input->GetMouseWheel((int&)zoomValue);
		if (zoomValue > 0) {
			if (fovModifier > -30.0) {
				fovModifier -= 1.0;
			}
		}
		else {
			if (fovModifier < 0) {
				fovModifier += 1.0;
			}
		}
		_camera.fov = glm::radians(60 + fovModifier);
	}

	//camera focus
	if (Engine::Instance().input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		if (selectedGameObject != nullptr) {
			cameratransform.pos() = selectedGameObject->GetComponent<Transform>()->pos() + vec3(0, 3, 8);
			_camera.transform().setFwd(glm::normalize(selectedGameObject->GetComponent<Transform>()->pos() - cameratransform.pos()));
			_camera.transform().setRigth(glm::normalize(glm::cross(vec3(0, 1, 0), _camera.transform().fwd())));
			_camera.transform().setUp(glm::normalize(glm::cross(_camera.transform().fwd(), _camera.transform().right())));
		}
		else {
			LOG(LogType::LOG_WARNING, "Select an Object!");
		}
	}

	//camera orbit
	if (Engine::Instance().input->GetMouseButtonDown(1) == KEY_DOWN) {
		leftMouse = true;
		Engine::Instance().input->GetMousePosition(lastMouseX, lastMouseY);
	}
	else if (Engine::Instance().input->GetMouseButtonDown(1) == KEY_UP) {
		leftMouse = false;
	}

	if (Engine::Instance().input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && leftMouse) {
		if (selectedGameObject != nullptr) 
		{
			Engine::Instance().input->GetMousePosition(mouseX, mouseY);
			vec3 targetPos = selectedGameObject->GetComponent<Transform>()->pos();

			// Calcular la distancia y offset inicial entre la cámara y el objeto
			vec3 offset = cameratransform.pos() - targetPos;
			float orbitDistance = glm::length(offset);

			// Calcular ángulos iniciales de la cámara
			horizontalAngle = atan2(offset.x, offset.z);
			verticalAngle = asin(offset.y / orbitDistance);

			// Sensibilidad del movimiento del ratón
			float sensitivity = 0.005f;

			float dx = (float)(mouseX - lastMouseX);
			float dy = (float)(mouseY - lastMouseY);

			// Actualizar los ángulos en base al movimiento del ratón
			horizontalAngle -= dx * sensitivity;
			verticalAngle += dy * sensitivity;

			// Limitar el ángulo vertical para no pasar por encima o debajo del objeto
			verticalAngle = glm::clamp(verticalAngle, -glm::half_pi<float>() + 0.1f, glm::half_pi<float>() - 0.1f);

			// Calcular la nueva posición de la cámara en coordenadas esféricas
			offset.x = orbitDistance * cos(verticalAngle) * sin(horizontalAngle);
			offset.y = orbitDistance * sin(verticalAngle);
			offset.z = orbitDistance * cos(verticalAngle) * cos(horizontalAngle);

			cameratransform.pos() = targetPos + offset;

			// Actualizar la dirección de la cámara para que mire al objeto
			_camera.transform().setFwd(glm::normalize(targetPos - cameratransform.pos()));
			_camera.transform().setRigth(glm::normalize(glm::cross(vec3(0, 1, 0), _camera.transform().fwd())));
			_camera.transform().setUp(glm::normalize(glm::cross(_camera.transform().fwd(), _camera.transform().right())));

			Engine::Instance().input->GetMousePosition(lastMouseX, lastMouseY);
		}
		else {
			LOG(LogType::LOG_WARNING, "Select an Object!");
		}
	}


}

void Scene::PostUpdate()
{
	
}

void Scene::CleanUp()
{
	/*for (auto& child : children())
	{
		child.get()->CleanUp()
	}*/
}

void Scene::OnSceneChange() {}




void Scene::Draw(GameObject* root)
{
	for (auto& child : root->children())
	{
		if (child.get()->isActive() && child->GetComponent<Mesh>() != nullptr && child->GetComponent<Mesh>()->isActive()) {
			child->GetComponent<Mesh>()->drawModel();
		}

		if (!child->children().empty()) Draw(child.get());
	}

	drawDebugInfoForGraphicObject(*root);
}

void Scene::loadGameObjectByPath(const std::string& path)
{
	ModelLoader modelLoader;
	std::vector<std::shared_ptr<Model>> models;
	modelLoader.load(path, models);
	for (size_t i = 0; i < models.size(); i++)
	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>(models[i].get()->GetMeshName());
		go->GetComponent<Transform>()->pos() = vec3(5, 0, 0);
		go->AddComponent<Mesh>();
		go->GetComponent<Mesh>()->setModel(models[i]);
		go->GetComponent<Mesh>()->setFilePath(path);
		go->GetComponent<Mesh>()->loadToOpenGL();
		go->AddComponent<Material>();
		go->GetComponent<Material>()->m_Shader = std::make_unique<Shader>("Assets/Shaders/Basic.shader");
		root()->addChild(go);
	}

	LOG(LogType::LOG_INFO, "Model loaded successfully!");
}

void Scene::loadTextureByPath(const std::string& path)
{
	if (selectedGameObject != nullptr) {
		selectedGameObject->GetComponent<Material>()->m_Texture = std::make_unique<Texture>(path);
	}
	else {
		LOG(LogType::LOG_WARNING,"Select an Object!");
	}
}

std::shared_ptr<GameObject> Scene::CreateGameObject()
{
	ModelLoader modelLoader;
	std::shared_ptr<Model> model;
	modelLoader.load(Shapes::EMPTY, model);
	std::shared_ptr<GameObject> go = std::make_shared<GameObject>("GameObject");
	go->GetComponent<Transform>()->pos() = vec3(0, 0, 0);

	if (selectedGameObject == nullptr) root()->addChild(go);
	else selectedGameObject->addChild(go);

	return go;
}

// Create a GameObject with a Camera component
void Scene::CreateCameraObject()
{
	auto cameraObject = std::make_shared<GameObject>("MainCamera");
	cameraObject->AddComponent<Camera>();
	cameraObject->GetComponent<Transform>()->pos() = vec3(0, 3, 8);
	root()->addChild(cameraObject);
}

void Scene::CreateCube()
{
	ModelLoader modelLoader;
	std::shared_ptr<Model> model;
	modelLoader.load(Shapes::CUBE, model);
	std::shared_ptr<GameObject> go = std::make_shared<GameObject>(model.get()->GetMeshName());
	
	if (selectedGameObject == nullptr) root()->addChild(go);
	else selectedGameObject->addChild(go);
	
	go->GetComponent<Transform>()->pos() = vec3(10, 0, 0);
	go->GetComponent<Transform>()->updateGlobalMatrix();
	go->AddComponent<Mesh>();
	go->GetComponent<Mesh>()->setModel(model);
	go->GetComponent<Mesh>()->loadToOpenGL();
	go->AddComponent<Material>();
	go->GetComponent<Material>()->m_Shader = std::make_unique<Shader>("Assets/Shaders/Basic.shader");
}

void Scene::CreatePlane() 
{
	ModelLoader modelLoader;
	std::shared_ptr<Model> model;
	modelLoader.load(Shapes::PLANE, model);
	std::shared_ptr<GameObject> go = std::make_shared<GameObject>(model.get()->GetMeshName());
	go->GetComponent<Transform>()->pos() = vec3(-5, 0, 0);
	go->AddComponent<Mesh>();
	go->GetComponent<Mesh>()->setModel(model);
	go->GetComponent<Mesh>()->loadToOpenGL();
	go->AddComponent<Material>();
	go->GetComponent<Material>()->m_Shader = std::make_unique<Shader>("Assets/Shaders/Basic.shader");

	if (selectedGameObject == nullptr) root()->addChild(go);
	else selectedGameObject->addChild(go);
}

void Scene::CreateSphere()
{
	ModelLoader modelLoader;
	std::shared_ptr<Model> model;
	modelLoader.load(Shapes::SPHERE, model);
	std::shared_ptr<GameObject> go = std::make_shared<GameObject>(model.get()->GetMeshName());
	go->GetComponent<Transform>()->pos() = vec3(10, 0, -5);
	go->AddComponent<Mesh>();
	go->GetComponent<Mesh>()->setModel(model);
	go->GetComponent<Mesh>()->loadToOpenGL();
	go->AddComponent<Material>();
	go->GetComponent<Material>()->m_Shader = std::make_unique<Shader>("Assets/Shaders/Basic.shader");

	if (selectedGameObject == nullptr) root()->addChild(go);
	else selectedGameObject->addChild(go);
}

void Scene::CreateCylinder()
{
	ModelLoader modelLoader;
	std::shared_ptr<Model> model;
	modelLoader.load(Shapes::CYLINDER, model);
	std::shared_ptr<GameObject> go = std::make_shared<GameObject>(model.get()->GetMeshName());
	go->GetComponent<Transform>()->pos() = vec3(-5, 0, -5);
	go->AddComponent<Mesh>();
	go->GetComponent<Mesh>()->setModel(model);
	go->GetComponent<Mesh>()->loadToOpenGL();
	go->AddComponent<Material>();
	go->GetComponent<Material>()->m_Shader = std::make_unique<Shader>("Assets/Shaders/Basic.shader");

	if (selectedGameObject == nullptr) root()->addChild(go);
	else selectedGameObject->addChild(go);
}

void Scene::CreateCone()
{
	ModelLoader modelLoader;
	std::shared_ptr<Model> model;
	modelLoader.load(Shapes::CONE, model);
	std::shared_ptr<GameObject> go = std::make_shared<GameObject>(model.get()->GetMeshName());
	go->GetComponent<Transform>()->pos() = vec3(0, 0, -5);
	go->AddComponent<Mesh>();
	go->GetComponent<Mesh>()->setModel(model);
	go->GetComponent<Mesh>()->loadToOpenGL();
	go->AddComponent<Material>();
	go->GetComponent<Material>()->m_Shader = std::make_unique<Shader>("Assets/Shaders/Basic.shader");

	if (selectedGameObject == nullptr) root()->addChild(go);
	else selectedGameObject->addChild(go);
}

void Scene::CreateTorus()
{
	ModelLoader modelLoader;
	std::shared_ptr<Model> model;
	modelLoader.load(Shapes::TORUS, model);
	std::shared_ptr<GameObject> go = std::make_shared<GameObject>(model.get()->GetMeshName());
	go->GetComponent<Transform>()->pos() = vec3(5, 0, -5);
	go->AddComponent<Mesh>();
	go->GetComponent<Mesh>()->setModel(model);
	go->GetComponent<Mesh>()->loadToOpenGL();
	go->AddComponent<Material>();
	go->GetComponent<Material>()->m_Shader = std::make_unique<Shader>("Assets/Shaders/Basic.shader");

	if (selectedGameObject == nullptr) root()->addChild(go);
	else selectedGameObject->addChild(go);
}