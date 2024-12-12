#include "Scene.h"
#include "types.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <memory>
#include <GL/glut.h>
#include <iostream>

#include "Engine.h"
#include "Log.h"
#include "Input.h"
#include "ModelLoader.h"
#include "Mesh.h"
#include "Material.h"
#include "debugDraws.h"
#include "BoundingBox.h"
#include "MyWindow.h"
#include "Raycast.h"

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
	gameObjects.clear();

	std::vector<GameObject> gameObjects;
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

		BoundingBox cubo;

		cubo.min = models[i].get()->GetModelData().vertexData.front();
		cubo.max = models[i].get()->GetModelData().vertexData.front();

		for (const auto& v : models[i].get()->GetModelData().vertexData) {
			cubo.min = glm::min(cubo.min, glm::dvec3(v));
			cubo.max = glm::max(cubo.max, glm::dvec3(v));
		}

		// Convertir la caja delimitadora a AABB
		glm::vec3 globalMin = glm::vec3(go->GetComponent<Transform>()->mat() * glm::vec4(cubo.min, 1.0f));
		glm::vec3 globalMax = glm::vec3(go->GetComponent<Transform>()->mat() * glm::vec4(cubo.max, 1.0f));

		cubo.min = glm::min(globalMin, globalMax);
		cubo.max = glm::max(globalMin, globalMax);


		// Rotar la AABB 90 grados a la derecha (alrededor del eje X)
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0));

		glm::vec3 rotatedMin = glm::vec3(rotationMatrix * glm::vec4(cubo.min, 1.0f));
		glm::vec3 rotatedMax = glm::vec3(rotationMatrix * glm::vec4(cubo.max, 1.0f));

		cubo.min = glm::min(rotatedMin, rotatedMax);
		cubo.max = glm::max(rotatedMin, rotatedMax);
		 
		//if (bakerHouse->GetComponent<Camera>()->isAABBInFrustrum(cubo.min, cubo.max) == false) { 
		//	// No renderizar
		//	go->GetComponent<Mesh>()->isActive() = false; 
		//}
		//else {
		//	// Renderizar el mesh
		//	go->GetComponent<Mesh>()->isActive()= true; 
		//}

		go->boundingBox = cubo;
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


	if (Engine::Instance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
		int mouseX, mouseY;
		Engine::Instance().input->GetMousePosition(mouseX, mouseY);

		// Screen dimensions
		int screenWidth = Engine::Instance().window->width();
		int screenHeight = Engine::Instance().window->height();

		if (mouseX < Engine::Instance().window->width() || mouseX >(Engine::Instance().window->height() + screenWidth) || mouseY < 20 || mouseY > screenHeight) {
			return;
		}
		Raycast ray = CalculatePickingRay(mouseX, mouseY - Engine::Instance().window->height(), camera()->GetComponent<Camera>(), screenWidth, screenHeight);

		float closestT = FLT_MAX;
		std::shared_ptr<GameObject> closestObject = nullptr;

		// Start the recursive check from the root object
		for (auto& child : root()->children()) {
			closestObject = CheckIntersectionRecursive(ray, child, closestT, closestObject);
		}

		if (closestObject) {
			selectedGameObject = closestObject.get();
			LOG(LogType::LOG_INFO, ("Selected object: " + selectedGameObject->name()).c_str());
		}
		else {
			selectedGameObject = nullptr;
			LOG(LogType::LOG_INFO, "No object selected.");
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


bool isAABBInsideFrustum(const BoundingBox& aabb, const std::list<Plane>& planes) {
	for (const auto& plane : planes) {
		// Calcula los puntos más cercano y lejano al plano
		vec3 farthest = vec3(
			plane.normal.v.x > 0 ? aabb.max.x : aabb.min.x,
			plane.normal.v.y > 0 ? aabb.max.y : aabb.min.y,
			plane.normal.v.z > 0 ? aabb.max.z : aabb.min.z
		);

		vec3 nearest = vec3(
			plane.normal.v.x > 0 ? aabb.min.x : aabb.max.x,
			plane.normal.v.y > 0 ? aabb.min.y : aabb.max.y,
			plane.normal.v.z > 0 ? aabb.min.z : aabb.max.z
		);

		// Revisar si el AABB está fuera del frustum
		if (glm::dot(plane.normal.v, farthest - plane.point.p) < 0) {
			return false; // El AABB está fuera del frustum
			
		}
	}
	return true; // Está dentro del frustum
}

void Scene::Draw(GameObject* root)
{

	for (auto& child : root->children())
	{
		std::list<Plane> frustumPlanes = camera()->GetComponent<Camera>()->frustumPlanes();
		if (child.get()->isActive() && child->HasComponent<Mesh>() && child->GetComponent<Mesh>()->isActive()) {
			BoundingBox oper = child->GetComponent<Transform>()->mat() * child->boundingBox;

			// Convertir la BoundingBox a AABB
			glm::vec3 corners[8] = {
				oper.v000(), oper.v001(), oper.v010(), oper.v011(),
				oper.v100(), oper.v101(), oper.v110(), oper.v111()
			};

			glm::vec3 aabbMin = corners[0];
			glm::vec3 aabbMax = corners[0];

			for (int i = 1; i < 8; ++i) {
				aabbMin = glm::min(aabbMin, corners[i]);
				aabbMax = glm::max(aabbMax, corners[i]);
			}

			BoundingBox aabb;
			aabb.min = aabbMin;
			aabb.max = aabbMax;

			glLineWidth(2.0);
			drawWiredQuad(aabb.v000(), aabb.v001(), aabb.v011(), aabb.v010());
			drawWiredQuad(aabb.v100(), aabb.v101(), aabb.v111(), aabb.v110());
			drawWiredQuad(aabb.v000(), aabb.v001(), aabb.v101(), aabb.v100());
			drawWiredQuad(aabb.v010(), aabb.v011(), aabb.v111(), aabb.v110());
			drawWiredQuad(aabb.v000(), aabb.v010(), aabb.v110(), aabb.v100());
			drawWiredQuad(aabb.v001(), aabb.v011(), aabb.v111(), aabb.v101());

			if (isAABBInsideFrustum(aabb, frustumPlanes)) {
				child->GetComponent<Mesh>()->drawModel(); // Dibujar solo si pasa el culling
			}
		}

		if (!child->children().empty()) Draw(child.get());
	}
	/*drawFrustum(*camera()->GetComponent<Camera>());*/

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

	std::shared_ptr<GameObject> camera = std::make_shared<GameObject>(_camera);
	root()->addChild(camera);
	drawFrustum(*camera->GetComponent<Camera>());

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

Raycast Scene::CalculatePickingRay(int mouseX, int mouseY, Camera* camera, int screenWidth, int screenHeight) {
	glm::vec4 viewport(WINDOW_WIDTH * 0.15, 200, screenWidth, screenHeight);

	glm::mat4 view = glm::mat4(camera->view());
	glm::mat4 projection = glm::mat4(camera->projection());

	glm::vec3 nearPoint = glm::unProject(glm::vec3(mouseX, screenHeight - mouseY, 0.0f),
		view,
		projection,
		viewport);
	glm::vec3 farPoint = glm::unProject(glm::vec3(mouseX, screenHeight - mouseY, 1.0f),
		view,
		projection,
		viewport);

	glm::vec3 direction = glm::normalize(farPoint - nearPoint);
	return Raycast{ nearPoint, direction };
}

bool Scene::IntersectRayBox(const Raycast& ray, const BoundingBox& box, float& t) {
	float tmin = (box.min.x - ray.origin.x) / ray.direction.x;
	float tmax = (box.max.x - ray.origin.x) / ray.direction.x;

	if (tmin > tmax) std::swap(tmin, tmax);

	float tymin = (box.min.y - ray.origin.y) / ray.direction.y;
	float tymax = (box.max.y - ray.origin.y) / ray.direction.y;

	if (tymin > tymax) std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax)) return false;

	if (tymin > tmin) tmin = tymin;
	if (tymax < tmax) tmax = tymax;

	float tzmin = (box.min.z - ray.origin.z) / ray.direction.z;
	float tzmax = (box.max.z - ray.origin.z) / ray.direction.z;

	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax)) return false;

	t = tmin;
	return true;
}


std::shared_ptr<GameObject>& Scene::CheckIntersectionRecursive(const Raycast& ray, std::shared_ptr<GameObject> object, float& closestT, std::shared_ptr<GameObject>& closestObject) {
	// Check the bounding box of the current object
	BoundingBox bbox = object->boundingBox;
	float t;
	if (object->HasComponent<Mesh>()) {
		if (IntersectRayBox(ray, bbox, t) && t < closestT) {
			closestT = t;
			closestObject = object;
		}
	}

	// Recursively check the children of the current object
	for (auto& child : object->children()) {
		CheckIntersectionRecursive(ray, child, closestT, closestObject);
	}

	return closestObject;
}