#include "PanelMenu.h"
#include <imgui.h>

#include "MyGUI.h"

#include "MyGameEngine/Engine.h"
#include "MyGameEngine/Scene.h"
#include "MyGameEngine/GameObject.h"
#include "MyGameEngine/Mesh.h"

#include "PanelConsole.h"
#include "PanelInspector.h"
#include "PanelHierarchy.h"
#include "PanelConfiguration.h"

PanelMenu::PanelMenu(std::string name) : Panel(name, 0, 0)
{
	SwitchState();
}

PanelMenu::~PanelMenu() {}

bool PanelMenu::Draw()
{
    
    if (ImGui::BeginMainMenuBar()) 
    {
        // File menu
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene", "Ctrl+N", nullptr, false)) {
                // Action for "New" item
            }
            if (ImGui::MenuItem("Open Scene", "Ctrl+O", nullptr, false)) {
                // Action for "Open" item
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Save", "Ctrl+S", nullptr, false)) {
                // Action for "Save" item
            }  
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S", nullptr, false)) {
                // Action for "Save" item
            }
            ImGui::Separator();
            if (ImGui::MenuItem("New Project", "Ctrl+Shift+N", nullptr, false)) {
                // Action for "Exit" item
            }
            if (ImGui::MenuItem("Open Project", "Ctrl+Shift+O", nullptr, false)) {
                // Action for "Exit" item
            }
            if (ImGui::MenuItem("Save Project", nullptr, nullptr, false)) {
                // Action for "Exit" item
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
				Engine::Instance().quit = true;
            }
            ImGui::EndMenu();
        }

        // Edit menu
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z", nullptr, false)) {
                // Action for "Undo" item
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y", nullptr, false)) {
                // Action for "Redo" item
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Select All", "Ctrl+A", nullptr, false)) {
                // Action for "Undo" item
            }
            if (ImGui::MenuItem("Deselect All", "Shift+D", nullptr, false)) {
                // Action for "Undo" item
            }
            if (ImGui::MenuItem("Select Children", "Shift+C", nullptr, false)) {
                // Action for "Undo" item
            }
            if (ImGui::MenuItem("Invert Selection", "Ctrl+I", nullptr, false)) {
                // Action for "Undo" item
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Duplicate", "Ctrl+D", nullptr, false)) {
                // Action for "Undo" item
            }
            if (ImGui::MenuItem("Delete", "Supr", nullptr, false)) {
                // Action for "Undo" item
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Play", "Ctrl+P", nullptr, false)) {
                // Action for "Undo" item
            }
            if (ImGui::MenuItem("Pause", "Ctrl+Shift+P", nullptr, false)) {
                // Action for "Undo" item
            }
            if (ImGui::MenuItem("Step", "Ctrl+Alt+P", nullptr, false)) {
                // Action for "Undo" item
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Settings...")) {
				MyGUI::Instance().configuration().SwitchState();
            }
            ImGui::EndMenu();
        }

        // Window menu
        if (ImGui::BeginMenu("Window")) {
            
            if (ImGui::BeginMenu("Veiwport", false))
            {
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Menus"))
            {
                if (ImGui::MenuItem("Toolbar", nullptr, nullptr, false)) {
                    // Action for "Undo" item
                }
                if (ImGui::MenuItem("Hierarchy", nullptr, MyGUI::Instance().hierarchy().GetState())) {
					MyGUI::Instance().hierarchy().SwitchState();
                }
                if (ImGui::MenuItem("Project", nullptr, nullptr, false)) {
                    // Action for "Undo" item
                }
                if (ImGui::MenuItem("Console", nullptr, MyGUI::Instance().console().GetState())) {
                    MyGUI::Instance().console().SwitchState();
                }
				if (ImGui::MenuItem("Inspector", nullptr, MyGUI::Instance().inspector().GetState())) {
					MyGUI::Instance().inspector().SwitchState();
				}
                if (ImGui::MenuItem("Assets", nullptr, MyGUI::Instance().assets().GetState())) {
                    MyGUI::Instance().assets().SwitchState();
                }
                if (ImGui::MenuItem("Scene", nullptr, nullptr, false)) {
                    // Action for "Undo" item
                }
                if (ImGui::MenuItem("Game", nullptr, nullptr, false)) {
                    // Action for "Undo" item
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("DevDebug"))
            {
				if (ImGui::MenuItem("Show UI Demo")) {
					// Action for "Undo" item
				}
				if (ImGui::MenuItem("Generate Game", nullptr, nullptr, false)) {
					// Action for "Undo" item
				}
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        // GameObjects menu
        if (ImGui::BeginMenu("GameObjects")) 
        {
            if (ImGui::BeginMenu("Create..."))
            {
                if (ImGui::MenuItem("Create Empty", nullptr, nullptr)) {
					Engine::Instance().scene->CreateGameObject();
                }
                if (ImGui::MenuItem("Create Empty x10", nullptr, nullptr)) {
                    for (size_t i = 0; i < 10; i++) {
                        Engine::Instance().scene->CreateGameObject();
                    }
                }
                if (ImGui::MenuItem("Plane", nullptr, nullptr)) {
					Engine::Instance().scene->CreatePlane();
                }
                if (ImGui::MenuItem("Cube", nullptr, nullptr)) {
					Engine::Instance().scene->CreateCube();
                }
				if (ImGui::MenuItem("Sphere", nullptr, nullptr)) {
					Engine::Instance().scene->CreateSphere();
				}
                if (ImGui::MenuItem("Hemisphere", nullptr, nullptr, false)) {
                    // Action for "Undo" item
                }
				if (ImGui::MenuItem("Cylinder", nullptr, nullptr)) {
					Engine::Instance().scene->CreateCylinder();
				}
                if (ImGui::MenuItem("Cone", nullptr, nullptr)) {
					Engine::Instance().scene->CreateCone();
                }
                if (ImGui::MenuItem("Torus", nullptr, nullptr)) {
					Engine::Instance().scene->CreateTorus();
                }
                ImGui::EndMenu();
            }
			
            if (ImGui::BeginMenu("Draw Mode")) {

                if (ImGui::MenuItem("Mesh", nullptr, currentDrawMode == Mesh)) {
                    currentDrawMode = Mesh;
                }
                if (ImGui::MenuItem("Wireframe", nullptr, currentDrawMode == Wireframe)) {
                    currentDrawMode = Wireframe;
                }
                if (ImGui::MenuItem("Vertexs", nullptr, currentDrawMode == Vertexs)) {
                    currentDrawMode = Vertexs;
                }
                ImGui::Separator();
                if (ImGui::Checkbox("Vertex Normals", &showPerTriangleNormals)) {
                    for (auto& child : Engine::Instance().scene->root()->children())
                    {
                        //child->GetComponent<Mesh>()->setDebugNormals(showPerTriangleNormals);
                    }
                }
                if (ImGui::Checkbox("Face Normals", &showPerFaceNormals)) {
                    for (auto& child : Engine::Instance().scene->root()->children())
                    {
                        //child->GetComponent<Mesh>()->setDebugFaceNormals(showPerFaceNormals);
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        // Help menu
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::Button("About AV-Mace...")) {
                system("start chrome.exe https://github.com/CITM-UPC/AV-Mace_Engine");
            }
			ImGui::Separator();
			if (ImGui::MenuItem("Documentation")) {
				// Action for "Documentation" item
                system("start chrome.exe https://github.com/CITM-UPC/AV-Mace_Engine/blob/main/README.md");
			}
            if (ImGui::MenuItem("Download latest version")) {
                //link to release when uploaded
                system("start chrome.exe https://github.com/CITM-UPC/AV-Mace_Engine/releases");
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Report bug / Suggest Feature")) {
                // Action for "Documentation" item
            }
            if (ImGui::MenuItem("Give us Feedback!")) {
                // Action for "Documentation" item
            }   
            if (ImGui::MenuItem("Contact us!")) {
                // Action for "Documentation" item
            }
            ImGui::EndMenu();
        }

        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2 - 100, 20), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(200, 50));

        if (ImGui::Begin("Game Controls", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar))
        {
            ImGui::Columns(2, nullptr, false); 
            ImGui::SetColumnWidth(0, 100);    
            // Bot�n Play
            if (ImGui::Button("Play", ImVec2(90, 40))) {
                isPlaying = true;
                Engine::Instance().StartGame(); 
            }

            ImGui::NextColumn(); 

            // Bot�n Pause
            if (ImGui::Button("Pause", ImVec2(90, 40))) {
                isPlaying = false;
                Engine::Instance().PauseGame(); 
            }

            ImGui::End(); 
        }


        ImGui::EndMainMenuBar();
    }

    return true;
}