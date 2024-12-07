#ifndef __PANEL_MENU_H__
#define __PANEL_MENU_H__
#pragma once

#include "Panel.h"

enum DrawMode {
	Mesh,
	Wireframe,
	Vertexs
};

class PanelMenu : public Panel
{

public:
	PanelMenu(std::string name);
	~PanelMenu();

	bool Draw();

private:
	int drawMode = 0;
	DrawMode currentDrawMode = Mesh;

	bool showPerTriangleNormals = true;
	bool showPerFaceNormals = true;

	bool isPlaying = false;
};


#endif // !__PANEL_MENU_H__