#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__
#pragma once

#include <vector>
#include <memory>  
#include "Panel.h"
#include "MyGameEngine/types.h"
#include "MyGameEngine/GameObject.h"

class PanelInspector : public Panel
{

public:
	PanelInspector(std::string name);
	~PanelInspector();

	std::string currentLayer() const { return _currentLayer; }
	void setLayer(const std::string& layer) { _currentLayer = layer; }

	bool Draw();
	void DrawGameObjectControls(GameObject* gameObject);
	void DrawTransformControls(GameObject* gameObject);
	void DrawMeshControls(GameObject* gameObject);
	void DrawMaterialControls(GameObject* gameObject);

private:
	// Tag options
	std::vector<std::string> tagOptions = { "Untagged", "Player", "Other" };

	// Layer options
	std::string _currentLayer = "Default";
	std::vector<std::string> layers = { "Default", "TransparentFX", "Ignore Raycast", "Water", "UI", "PostProcessing" };

	// Component options
	std::vector<std::string> componentOptions = { "Transform", "Mesh", "Material", "Camera" };

	bool showPerTriangleNormals = false;
	bool showPerFaceNormals = false;
	bool showCheckers = false;
	bool showWireframe = false;
	bool uniformScale = false;
};

#endif // !__PANEL_INSPECTOR_H__