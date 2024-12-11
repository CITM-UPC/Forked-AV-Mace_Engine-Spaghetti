#ifndef __PANEL_H__
#define __PANEL_H__
#pragma once

#include <string>

class Panel
{
public:

	Panel(std::string name, float width, float height) : name(name), width(width), height(height) {}
	virtual ~Panel() = default;

	virtual void Start() {}
	virtual bool Draw() = 0;
	virtual bool CleanUp() { return true; }

	virtual void OnSceneChange() {}

	virtual std::string GetName() { return name; }

	virtual bool GetState() { return showWindow; }
	virtual void SetState(bool state) { this->enabled = state; }
	virtual void SwitchState() { 
		enabled = !enabled; 
		showWindow = enabled;
	}

	bool isActiveTab = false;

protected:

	std::string name;
	bool enabled = false;
	float width;
	float height;
	bool showWindow = true;
	
};

#endif // !__PANEL_H__
