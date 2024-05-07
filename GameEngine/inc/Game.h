#pragma once

#ifdef _EDITOR
#include "ImguiSystem.h"
#include "Editor/LevelTree.h"
#endif // _EDITOR



class Game {
private:
	bool isRunning;

#ifdef _EDITOR
	ImguiSystem imgui;
	LevelEditor::LevelTree levelTree;
#endif // _EDITOR



public:
	Game();
	~Game();
	void SetupBaseSystems();
	void SetupLowLevelSystems();
	void LoadAssets();
	void LoadLevel();
	void Setup();
	void Run();
	void Update();
	void Render();
	void RegisterAllEntities();
};
