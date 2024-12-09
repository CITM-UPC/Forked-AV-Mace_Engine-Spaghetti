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
	InitCamera();

	std::shared_ptr<GameObject> bakerHouse = CreateGameObject("Street");

	ModelLoader modelLoader;
	std::vector<std::shared_ptr<Model>> models;
	modelLoader.load("Assets/FBX/street2.fbx", models);
	for (size_t i = 0; i < models.size(); i++)
	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>(models[i].get()->GetMeshName());
		bakerHouse->addChild(go);

		go->GetComponent<Transform>()->pos() = vec3(0, 0, 0);
		go->GetComponent<Transform>()->rotate(glm::radians(-90.0), vec3(1, 0, 0));

		go->GetComponent<Transform>()->updateGlobalMatrix();
		go->AddComponent<Mesh>();
		go->GetComponent<Camera>()->frustumPlanes();
		go->GetComponent<Mesh>()->setModel(models[i]);
		


		go->GetComponent<Mesh>()->setFilePath("Assets/FBX/street2.fbx"); 
		go->AddComponent<Material>();

		
			std::string texturePath = models[i].get()->GetTexturePath();
			go->GetComponent<Material>()->m_Texture = std::make_unique<Texture>(texturePath);
		
		
		/*go->GetComponent<Material>()->m_Texture = std::make_unique<Texture>("Assets/Textures/building 01_c.tga");*/
		std::cout << "Textura : " << models[i].get()->GetTexturePath() << std::endl;
		/*go->GetComponent<Material>()->m_Texture = std::make_unique<Texture>("Assets/Textures/building 06_ c.tga");
		go->GetComponent<Material>()->m_Texture = std::make_unique<Texture>("Assets/Textures/building 06_ c.tga");
		go->GetComponent<Material>()->m_Texture = std::make_unique<Texture>("Assets/Textures/building_016_c.tga");
		go->GetComponent<Material>()->m_Texture = std::make_unique<Texture>("Assets/Textures/building_025_c.tga");
		go->GetComponent<Material>()->m_Texture = std::make_unique<Texture>("Assets/Textures/Building_V01_C.png");
		go->GetComponent<Material>()->m_Texture = std::make_unique<Texture>("Assets/Textures/Building_V02_C.png");
		go->GetComponent<Material>()->m_Texture = std::make_unique<Texture>("Assets/Textures/building03_c.tga");
		go->GetComponent<Material>()->m_Texture = std::make_unique<Texture>("Assets/Textures/building05 _c.tga");*/

		go->GetComponent<Material>()->m_Shader = std::make_unique<Shader>("Assets/Shaders/Basic.shader");
		go->GetComponent<Mesh>()->loadToOpenGL();
	}
}

void Scene::Update(double& dT)
{
	//camera speed
	if (Engine::Instance().input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		camera()->GetComponent<Camera>()->speed() = 20.0f;
	}
	else {
		camera()->GetComponent<Camera>()->speed() = 10.0f;
	}

	//camera rotation
	if (Engine::Instance().input->GetMouseButtonDown(3) == KEY_DOWN) {
		rightMouse = true;
		Engine::Instance().input->GetMousePosition(lastMouseX, lastMouseY);
	}
	else if (Engine::Instance().input->GetMouseButtonDown(3) == KEY_UP) {
		rightMouse = false;
	}

	if (rightMouse)
	{
		//camera movement
		if (Engine::Instance().input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			camera()->GetComponent<Transform>()->translate(vec3(0, 0, camera()->GetComponent<Camera>()->speed() * dT));
		}
		if (Engine::Instance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			camera()->GetComponent<Transform>()->translate(vec3(camera()->GetComponent<Camera>()->speed() * dT, 0, 0));
		}
		if (Engine::Instance().input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			camera()->GetComponent<Transform>()->translate(vec3(0, 0, -camera()->GetComponent<Camera>()->speed() * dT));
		}
		if (Engine::Instance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			camera()->GetComponent<Transform>()->translate(vec3(-camera()->GetComponent<Camera>()->speed() * dT, 0, 0));
		}

		Engine::Instance().input->GetMousePosition(mouseX, mouseY);

		float dx = (float)(mouseX - lastMouseX);
		float dy = (float)(mouseY - lastMouseY);

		// Ajusta la sensibilidad
		float sensitivity = 0.5f;
		dx *= sensitivity;
		dy *= sensitivity;

		yaw = glm::degrees(atan2(camera()->GetComponent<Transform>()->fwd().z, camera()->GetComponent<Transform>()->fwd().x));
		pitch = glm::degrees(atan2(camera()->GetComponent<Transform>()->fwd().y, 
		glm::sqrt(camera()->GetComponent<Transform>()->fwd().x * camera()->GetComponent<Transform>()->fwd().x 
			+ camera()->GetComponent<Transform>()->fwd().z 
			* camera()->GetComponent<Transform>()->fwd().z)));

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
		camera()->GetComponent<Transform>()->setFwd(glm::normalize(direction));
		//uso vec3(0,1,0) porque busco el vector up en coordenadas world!!! igualmente acutalizo el vector up(local) de la camera
		camera()->GetComponent<Transform>()->setRigth(glm::normalize(glm::cross(vec3(0, 1, 0), camera()->GetComponent<Transform>()->fwd())));
		camera()->GetComponent<Transform>()->setUp(glm::normalize(glm::cross(camera()->GetComponent<Transform>()->fwd(), camera()->GetComponent<Transform>()->right())));

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
		camera()->GetComponent<Camera>()->fov() = glm::radians(60 + fovModifier);
	}

	//camera focus
	if (Engine::Instance().input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		if (selectedGameObject != nullptr) {
			camera()->GetComponent<Transform>()->pos() = selectedGameObject->GetComponent<Transform>()->glob_pos() + vec3(0, 3, 8);
			camera()->GetComponent<Transform>()->setFwd(glm::normalize(selectedGameObject->GetComponent<Transform>()->glob_pos() - camera()->GetComponent<Transform>()->pos()));
			camera()->GetComponent<Transform>()->setRigth(glm::normalize(glm::cross(vec3(0, 1, 0), camera()->GetComponent<Transform>()->fwd())));
			camera()->GetComponent<Transform>()->setUp(glm::normalize(glm::cross(camera()->GetComponent<Transform>()->fwd(), camera()->GetComponent<Transform>()->right())));
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
			vec3 targetPos = selectedGameObject->GetComponent<Transform>()->glob_pos();

			// Calcular la distancia y offset inicial entre la c�mara y el objeto
			vec3 offset = camera()->GetComponent<Transform>()->pos() - targetPos;
			float orbitDistance = glm::length(offset);

			// Calcular �ngulos iniciales de la c�mara
			horizontalAngle = atan2(offset.x, offset.z);
			verticalAngle = asin(offset.y / orbitDistance);

			// Sensibilidad del movimiento del rat�n
			float sensitivity = 0.005f;

			float dx = (float)(mouseX - lastMouseX);
			float dy = (float)(mouseY - lastMouseY);

			// Actualizar los angulos en base al movimiento del rat�n
			horizontalAngle -= dx * sensitivity;
			verticalAngle += dy * sensitivity;

			// Limitar el �ngulo vertical para no pasar por encima o debajo del objeto
			verticalAngle = glm::clamp(verticalAngle, -glm::half_pi<float>() + 0.1f, glm::half_pi<float>() - 0.1f);

			// Calcular la nueva posici�n de la camara en coordenadas esf�ricas
			offset.x = orbitDistance * cos(verticalAngle) * sin(horizontalAngle);
			offset.y = orbitDistance * sin(verticalAngle);
			offset.z = orbitDistance * cos(verticalAngle) * cos(horizontalAngle);

			camera()->GetComponent<Transform>()->pos() = targetPos + offset;

			// Actualizar la direcci�n de la camara para que mire al objeto
			camera()->GetComponent<Transform>()->setFwd(glm::normalize(targetPos - camera()->GetComponent<Transform>()->pos()));
			camera()->GetComponent<Transform>()->setRigth(glm::normalize(glm::cross(vec3(0, 1, 0), camera()->GetComponent<Transform>()->fwd())));
			camera()->GetComponent<Transform>()->setUp(glm::normalize(glm::cross(camera()->GetComponent<Transform>()->fwd(), camera()->GetComponent<Transform>()->right())));

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
		if (child.get()->isActive() && child->HasComponent<Mesh>() && child->GetComponent<Mesh>()->isActive()) {
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
		LOG(LogType::LOG_WARNING, "Select an Object!");
	}
}

std::shared_ptr<GameObject> Scene::CreateGameObject(const std::string& name)
{
	ModelLoader modelLoader;
	std::shared_ptr<Model> model;
	modelLoader.load(Shapes::EMPTY, model);
	std::shared_ptr<GameObject> go = std::make_shared<GameObject>(name.c_str());
	go->GetComponent<Transform>()->pos() = vec3(0, 0, 0);

	if (selectedGameObject == nullptr) root()->addChild(go);
	else selectedGameObject->addChild(go);

	return go;
}

void Scene::InitCamera()
{
	_camera.AddComponent<Camera>();
	_camera.AddComponent<Camera>()->frustumPlanes();
	_camera.GetComponent<Transform>()->translate(vec3(0, 3, 8));
	_camera.GetComponent<Transform>()->rotate(glm::radians(180.0), vec3(0, 1, 0));
	_camera.GetComponent<Transform>()->rotate(glm::radians(20.0), vec3(1, 0, 0));
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