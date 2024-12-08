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

   // Handle root level drops
   if (ImGui::BeginDragDropTarget()) {
       if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_DRAG")) {
           IM_ASSERT(payload->DataSize == sizeof(GameObject*));
           GameObject* draggedObject = *static_cast<GameObject**>(payload->Data);

           if (draggedObject) {
               draggedObject->reparent(Engine::Instance().scene->root());
           }
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
    if (!gameObject) return;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_OpenOnDoubleClick |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    if (selectedGameObject() == gameObject) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    // Add drag source identifier
    char uniqueLabel[256];
    const size_t labelSize = sizeof(uniqueLabel);
    int result = sprintf_s(uniqueLabel, labelSize, "%s##%p",
        gameObject->name().c_str(),
        static_cast<void*>(gameObject));
    if (result < 0) {
        strcpy_s(uniqueLabel, labelSize, gameObject->name().c_str());
    }

	bool isNodeOpen = ImGui::TreeNodeEx(gameObject->name().c_str(), flags);
    // Handle selection
    if (ImGui::IsItemClicked()) {
        SetSelectedGameObject(gameObject);
    }

    // Begin drag source
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        // Store pointer to dragged object
        ImGui::SetDragDropPayload("GAMEOBJECT_DRAG", &gameObject, sizeof(GameObject*));

        // Show preview text
        ImGui::Text("Moving %s", gameObject->name().c_str());
        ImGui::EndDragDropSource();
    }
    // Handle drop target
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_DRAG")) {
            IM_ASSERT(payload->DataSize == sizeof(GameObject*));
            GameObject* draggedObject = *static_cast<GameObject**>(payload->Data);

            if (draggedObject && draggedObject != gameObject) {
                draggedObject->reparent(gameObject);
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (isNodeOpen) {
        // Draw children
        auto children = gameObject->children(); // Get a copy of children list
        for (const auto& child : children) {
            if (child) {
                DrawGameObjectTree(child.get());
            }
        }
        ImGui::TreePop();
    }
}
