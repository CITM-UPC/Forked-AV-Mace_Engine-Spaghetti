#include "PanelInspector.h"
#include <imgui.h>
#include "MyGUI.h"
#include "PanelHierarchy.h"
#include "MyGameEngine/Engine.h"
#include "MyGameEngine/MyWindow.h"
#include "MyGameEngine/Transform.h"
#include "MyGameEngine/Mesh.h"
#include "MyGameEngine/Material.h"
#include "MyGameEngine/Camera.h"

#include "MyGameEngine/Log.h"

PanelInspector::PanelInspector(std::string name) : Panel(name, WINDOW_WIDTH * 0.25, WINDOW_HEIGHT - 200)
{
    SwitchState();
}

PanelInspector::~PanelInspector() {}

bool PanelInspector::Draw()
{
    ImGui::SetNextWindowSize(ImVec2(width, Engine::Instance().window->height() - 200));
    ImGui::SetNextWindowPos(ImVec2(Engine::Instance().window->width() - width, 0));

    if (auto* selectedGameObject = MyGUI::Instance().hierarchy().selectedGameObject())
    {
        ImGui::Begin("Inspector", &showWindow, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        DrawGameObjectControls(selectedGameObject);
        if (selectedGameObject->HasComponent<Transform>()) DrawTransformControls(selectedGameObject);
        if (selectedGameObject->HasComponent<Mesh>()) DrawMeshControls(selectedGameObject);
        if (selectedGameObject->HasComponent<Material>()) DrawMaterialControls(selectedGameObject);
        if (selectedGameObject->HasComponent<Camera>()) DrawCameraControls(selectedGameObject);
        ImGui::Text(" ");

        // Add Component
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 180) * 0.5f);
        if (ImGui::Button("Add Component", ImVec2(200.0f, 25.0))) ImGui::OpenPopup("AddComponentPopup");
        if (ImGui::BeginPopup("AddComponentPopup"))
        {
            ImGui::Text("Select Component to Add:");
            ImGui::Separator();

            for (auto& componentName : componentOptions)
            {
                bool isDisabled = false;
                if (componentName == "Transform" && selectedGameObject->HasComponent<Transform>() != nullptr)    isDisabled = true;
                //else if (componentName == "Mesh" && selectedGameObject->HasComponent<Mesh>() != nullptr)         isDisabled = true;
                else if (componentName == "Material" && selectedGameObject->HasComponent<Material>() != nullptr) isDisabled = true;
                else if (componentName == "Camera" && selectedGameObject->HasComponent<Camera>() != nullptr)     isDisabled = true;

                if (isDisabled) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f)); // Dimmed text color
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f); // Reduce opacity
                }

                if (ImGui::Selectable(componentName.c_str(), false, isDisabled ? ImGuiSelectableFlags_Disabled : 0))
                {
                    if (componentName == "Transform" && selectedGameObject->HasComponent<Transform>() == nullptr)       selectedGameObject->AddComponent<Transform>();
                    //else if (componentName == "Mesh" && selectedGameObject->HasComponent<Mesh>() == nullptr)            selectedGameObject->AddComponent<Mesh>();
                    else if (componentName == "Material" && selectedGameObject->HasComponent<Material>() == nullptr)    selectedGameObject->AddComponent<Material>();
                    else if (componentName == "Camera" && selectedGameObject->HasComponent<Camera>() == nullptr)        selectedGameObject->AddComponent<Camera>();
                }

                if (isDisabled) {
                    ImGui::PopStyleVar();
                    ImGui::PopStyleColor();
                }
            }
            ImGui::EndPopup();
        }
        ImGui::End();
    }

    if (!showWindow) {
        Engine::Instance().input->ActivateTextInput(false);
        SwitchState();
    }
    return true;
}

void PanelInspector::DrawGameObjectControls(GameObject* gameObject)
{
    ImGui::Checkbox("Active", &gameObject->isActive());
    ImGui::SameLine();

    Engine::Instance().input->ActivateTextInput();

    // Name input
    ImGui::SetNextItemWidth(160.0f);
    char buffer[128] = {};
    strncpy_s(buffer, gameObject->name().c_str(), sizeof(buffer));
    if (ImGui::InputText("##gameobject_name", buffer, sizeof(buffer), ImGuiInputTextFlags_None))
    {
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            gameObject->name() = buffer;
            Engine::Instance().input->ActivateTextInput(false);
        }
    }
    ImGui::SameLine();
    ImGui::Checkbox("Static", &gameObject->isActive());

    // Tag selection
    ImGui::Text("Tag");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.0f);
    if (ImGui::BeginCombo("##tag", gameObject->tag().c_str())) {
        for (const auto& option : tagOptions) {
            if (ImGui::Selectable(option.c_str(), gameObject->tag() == option)) {
                gameObject->tag() = option;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();

    // Layer selection
    ImGui::Text("Layer");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.0f);
    if (ImGui::BeginCombo("##layer", currentLayer().c_str()))
    {
        for (const auto& layer : layers)
        {
            if (ImGui::Selectable(layer.c_str(), currentLayer() == layer))
            {
                setLayer(layer);
                //gameObject->layer() = layer;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::Separator();
}

void PanelInspector::DrawTransformControls(GameObject* gameObject)
{
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
        auto* transform = gameObject->HasComponent<Transform>();

        showComponent = transform->isActive();
        ImGui::Checkbox("Active", &showComponent);
        transform->SetActive(showComponent);

        ImGui::SameLine();
        ImGui::Text("      X         Y         Z");


        // Position
        ImGui::Text("Position   ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(210.0f);

        float pos[3] = { transform->pos().x, transform->pos().y, transform->pos().z };
        if (ImGui::DragFloat3("##position", pos, 0.1f, -FLT_MAX, FLT_MAX, "%.2f") | ImGuiInputTextFlags_CharsDecimal)
        {
            if (ImGui::IsItemActive()) {
                Engine::Instance().input->ActivateTextInput();
                transform->translate(pos);
            }
            else if (ImGui::IsItemDeactivatedAfterEdit()) {
                Engine::Instance().input->ActivateTextInput(false);
            }
        }

        // Rotation
        ImGui::Text("Rotation   ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(210.0f);

        glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(transform->rot()));
        if (ImGui::DragFloat3("##rotation", &eulerAngles.x, 0.1f, -360.0f, 360.0f, "%.2f") | ImGuiInputTextFlags_CharsDecimal)
        {
            if (ImGui::IsItemActive()) {
                Engine::Instance().input->ActivateTextInput();
                transform->rotate(glm::radians(eulerAngles));
            }
            else if (ImGui::IsItemDeactivatedAfterEdit()) {
                Engine::Instance().input->ActivateTextInput(false);
            }
        }

        ImGui::Text("Scale  ");
        ImGui::SameLine();
        ImGui::Checkbox("##uniform_scale", &uniformScale);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(210.0f);

        float scale[3] = { transform->scale().x, transform->scale().y, transform->scale().z };
        if (ImGui::DragFloat3("##scale", scale, 0.1f, -FLT_MAX, FLT_MAX, "%.2f") | ImGuiInputTextFlags_CharsDecimal)
        {
            if (ImGui::IsItemActive()) {
                Engine::Instance().input->ActivateTextInput();
                if (uniformScale) transform->scale(vec3(scale[0]));
                else transform->scale(glm::vec3(scale[0], scale[1], scale[2]));
            }
            else if (ImGui::IsItemDeactivatedAfterEdit()) {
                Engine::Instance().input->ActivateTextInput(false);
            }
        }
        ImGui::Separator();
    }
}

void PanelInspector::DrawMeshControls(GameObject* gameObject)
{
    if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
    {
        auto* mesh = gameObject->HasComponent<Mesh>();

        showComponent = mesh->isActive();

        ImGui::Checkbox("Active", &showComponent);
        mesh->SetActive(showComponent);

        ImGui::SameLine();
        ImGui::Text("  File:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(170.0f);
        char buffer[128] = {};
        strncpy_s(buffer, mesh->getFilePath().c_str(), sizeof(buffer));
        ImGui::InputText("##mesh_path", buffer, sizeof(buffer));

        ImGui::Text("Display Normals:");

        showPerTriangleNormals = mesh->getDebugNormals();
        ImGui::Checkbox("Vertex Normals", &showPerTriangleNormals);
        mesh->setDebugNormals(showPerTriangleNormals);

        showPerFaceNormals = gameObject->HasComponent<Mesh>()->getDebugFaceNormals();
        ImGui::Checkbox("Face Normals", &showPerFaceNormals);
        mesh->setDebugFaceNormals(showPerFaceNormals);
        ImGui::Separator();
    }
}

void PanelInspector::DrawMaterialControls(GameObject* gameObject)
{
    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
    {
        auto* material = gameObject->HasComponent<Material>();

        showComponent = material->isActive();
        ImGui::Checkbox("Active", &showComponent);
        material->SetActive(showComponent);

        ImGui::Text(" ");
        ImGui::Text("Main Maps");
        ImVec4 magenta(1.0f, 0.0f, 1.0f, 1.0f); // Magenta

        if (material->m_Texture) {
            ImGui::Text("Texture:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200.0f);
            ImGui::TextColored(magenta, material->m_Texture->GetFilePath().c_str());
        }

        if (material->m_Shader) {
            ImGui::Text("Shader:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200.0f);
            ImGui::TextColored(magenta, material->m_Shader->GetFilePath().c_str());
        }

        ImGui::Checkbox("Albedo", &showCheckers);
        ImGui::Checkbox("Show Checkers", &showCheckers);
        ImGui::Separator();
    }
}

void PanelInspector::DrawCameraControls(GameObject* gameObject)
{
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
    {
        auto* camera = gameObject->HasComponent<Camera>();

        showComponent = camera->isActive();
        ImGui::Checkbox("Active", &showComponent);
        camera->SetActive(showComponent);

        ImGui::Text(" ");
        ImGui::Text("Projection:");
        ImGui::Text("Field of View");
        ImGui::Text("       ");
        ImGui::SameLine();


        if (ImGui::SliderScalar("##fov", ImGuiDataType_Double, &camera->fov(), &min_value, &max_value, "%.2f"))
        {
            if (ImGui::IsItemActive()) {
                Engine::Instance().input->ActivateTextInput();
            }
            else if (ImGui::IsItemDeactivatedAfterEdit()) {
                Engine::Instance().input->ActivateTextInput(false);
            }
        }

        ImGui::Text("Near Plane");
        ImGui::Text("       ");
        ImGui::SameLine();

        if (ImGui::SliderScalar("##zNear", ImGuiDataType_Double, &camera->near(), &min_value, &max_value, "%.2f"))
        {
            if (ImGui::IsItemActive()) {
                Engine::Instance().input->ActivateTextInput();
            }
            else if (ImGui::IsItemDeactivatedAfterEdit()) {
                Engine::Instance().input->ActivateTextInput(false);
            }
        }

        ImGui::Text("Far Plane");
        ImGui::Text("       ");
        ImGui::SameLine();

        if (ImGui::SliderScalar("##zFar", ImGuiDataType_Double, &camera->far(), &min_value, &max_value, "%.2f"))
        {
            if (ImGui::IsItemActive()) {
                Engine::Instance().input->ActivateTextInput();
            }
            else if (ImGui::IsItemDeactivatedAfterEdit()) {
                Engine::Instance().input->ActivateTextInput(false);
            }
        }

        ImGui::Separator();
    }
}