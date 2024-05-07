#pragma once
#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include "SdlContainer.h"
#include "AssetStore.h"
#include "Transform.h"
#include "Physics.h"
#include "Renderer.h"
#include "InputSystem.h"
#include "LevelLoader.h"

#ifdef _EDITOR
#include "ImguiSystem.h"
#include "Editor/LevelTree.h"
#endif // _EDITOR


const int FPS = 60;
const int FRAMETIME_MS = 1000 / FPS;


class Game {
private:
	//std::unique_ptr<RendererSystem> renderer;
	LevelLoader levelLoader;
	InputSystem input;
	bool isRunning;
	unsigned int msLastFrame;
	float dt;

	entt::entity player;
#ifdef _EDITOR
	ImguiSystem imgui;
	LevelEditor::LevelTree levelTree;
#endif // _EDITOR



public:
	Game();
	~Game();
	void SetupBaseSystems();
	void SetupLowLevelSystems();
	void Setup();
	void Run();
	void Update();
	void Render();
	void UpdateInputSystem();
	void RegisterAllEntities();
};
