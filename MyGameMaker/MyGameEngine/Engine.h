#ifndef __ENGINE_H__
#define __ENGINE_H__
#pragma once

#include <vector>

#include "Log.h"

class MyWindow;
class Input;

class Engine
{
public:

	// M�todos p�blicos de la clase
	static Engine& Instance() {
		static Engine instance;  // La instancia �nica de la clase Engine
		return instance;
	}

	void Awake();
	void Start();

	bool PreUpdate();
	void Update(double dt);

	void CleanUp();

	std::vector<LogInfo> GetLogs();
	void AddLog(LogType type, const char* entry);
	void CleanLogs();

	// Evita la copia y asignaci�n del singleton
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

public:

	double dt = 0;
	bool vsync = false;

	MyWindow* window = nullptr;
	Input* input = nullptr;

private:
	// Constructor y destructor privados para el singleton
	Engine() = default;
	~Engine() = default;
	// Logs
	LogInfo logInfo;
	std::vector<LogInfo> logs;
};

#endif // !__ENGINE_H__