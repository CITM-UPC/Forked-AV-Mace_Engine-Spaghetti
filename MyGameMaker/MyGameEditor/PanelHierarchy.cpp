#include "PanelHierarchy.h"

#include <imgui.h>
#include <memory>

#include "MyGameEngine/Engine.h"
#include "MyGameEngine/MyWindow.h"
#include "MyGameEngine/Scene.h"
#include "MyGameEngine/types.h"
#include "PanelInspector.h"

PanelHierarchy::PanelHierarchy(std::string name) : Panel(name, WINDOW_WIDTH * 0.1, WINDOW_HEIGHT - 219)
{
	SwitchState();
}

PanelHierarchy::~PanelHierarchy() {}

bool PanelHierarchy::Draw()
{
   ImGui::SetNextWindowSize(ImVec2(width, Engine::Instance().window->height() - 219));

   ImGui::SetNextWindowPos(ImVec2(0, 19));

   ImGui::Begin("Hierarchy", &showWindow, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

   if (ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered()) {
	   SetSelectedGameObject(nullptr); 
   }
   
   for (const std::shared_ptr<GameObject>& gameObjectPtr : Engine::Instance().scene->root()->children()) {
	   DrawGameObjectTree(gameObjectPtr.get());
   }
   if (ImGui::IsMouseClicked(3) && ImGui::IsWindowHovered()) {
       ImGui::OpenPopup("CreateGameObjectPopup");
       LOG(LogType::LOG_INFO, "Open Popup");
   }


   // Create GameObjects Popup
   if (ImGui::BeginPopup("CreateGameObjectPopup"))
   {
       LOG(LogType::LOG_INFO, "Popup Called");
       ImGui::Selectable("Cut", false);
       ImGui::Selectable("Copy", false);
       ImGui::Selectable("Paste", false);
       ImGui::Selectable("Paste As Child", false);
       ImGui::Separator();
       ImGui::Selectable("Rename", "F2", false);
       ImGui::Selectable("Duplicate", "Ctrl+D", false);
       ImGui::Selectable("Delete", "Del", false);
       ImGui::Separator();
       ImGui::Selectable("Select Children", false);
       ImGui::Separator();
       ImGui::Selectable("Create Empty", false);
       if (ImGui::BeginMenu("Primitives")) {
           // Add items here
           ImGui::EndMenu();
       }
       if (ImGui::BeginMenu("Material")) {
           // Add items here
           ImGui::EndMenu();
       }

       ImGui::EndPopup();
   }
   // Handle dropping at root level
   if (ImGui::BeginDragDropTarget()) {
       if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_DRAG")) {
           GameObject* draggedObject = *(GameObject**)payload->Data;
           draggedObject->reparent(Engine::Instance().scene->root());
       }
       ImGui::EndDragDropTarget();
   }


   ImGui::End();

   if (!showWindow) {
       SwitchState();
   }

   Engine::Instance().scene->selectedGameObject = selectedGameObject();

   return true;
}

void PanelHierarchy::DrawGameObjectTree(GameObject* gameObject)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	if (selectedGameObject() == gameObject) flags |= ImGuiTreeNodeFlags_Selected;

    // Add drag source identifier
    char uniqueLabel[256];
    sprintf_s(uniqueLabel, "%s##%p", gameObject->name().c_str(), (void*)gameObject);

	bool isNodeOpen = ImGui::TreeNodeEx(gameObject->name().c_str(), flags);
    // Handle selection
    if (ImGui::IsItemClicked()) {
        SetSelectedGameObject(gameObject);
    }

    // Begin drag source
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        ImGui::SetDragDropPayload("GAMEOBJECT_DRAG", &gameObject, sizeof(GameObject*));
        ImGui::Text("Moving %s", gameObject->name().c_str());
        ImGui::EndDragDropSource();
    }
    // Handle drop target
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_DRAG")) {
            GameObject* draggedObject = *(GameObject**)payload->Data;

            // Prevent dropping an object onto itself or its descendants
            if (draggedObject != gameObject && !gameObject->isDescendantOf(draggedObject)) {
                draggedObject->reparent(gameObject);
            }
        }
        ImGui::EndDragDropTarget();
    }
	if (isNodeOpen) {
		for (const std::shared_ptr<GameObject>& childObjectPtr : gameObject->children()) {
			DrawGameObjectTree(childObjectPtr.get());
		}
		ImGui::TreePop();
	}
}
