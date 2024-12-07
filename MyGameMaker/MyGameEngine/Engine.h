#ifndef __ENGINE_H__
#define __ENGINE_H__
#pragma once

#include <vector>

#include "Log.h"

class MyWindow;
class Input;
class Renderer;
class Scene;

class Engine
{
public:
	static Engine& Instance() {
		static Engine instance;  // La instancia única de la clase Engine
		return instance;
	}

	void Awake();
	void Start();

	bool PreUpdate();
	void Update(double& dt);
	void PostUpdate();

	void CleanUp();

	void SwapBuffers();

	void StartGame();
	void PauseGame();


	std::vector<LogInfo> GetLogs();
	void AddLog(LogType type, const char* entry);
	void CleanLogs();

	// Evita la copia y asignación del singleton
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	void SetFPS(float* fps) { this->fps = fps; }
	float* GetFPS() { return fps; }

public:

	double dt = 0;
	bool vsync = false;
	float* fps;

	MyWindow* window = nullptr;
	Input* input = nullptr;
	Renderer* renderer = nullptr;
	Scene* scene = nullptr;

	bool quit = false;

private:
	// Constructor y destructor privados para el singleton
	Engine() = default;
	~Engine();
	// Logs
	LogInfo logInfo;
	std::vector<LogInfo> logs;
};

#endif // !__ENGINE_H__