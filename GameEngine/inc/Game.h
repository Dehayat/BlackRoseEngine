#pragma once
#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include "SdlContainer.h"
#include "AssetStore.h"
#include "ImguiSystem.h"
#include "Transform.h"
#include "Physics.h"
#include "Renderer.h"
#include "InputSystem.h"
#include "LevelLoader.h"

const int FPS = 60;
const int FRAMETIME_MS = 1000 / FPS;



class Game {
private:
	std::unique_ptr<SdlContainer> sdl;
	std::unique_ptr<AssetStore> assetStore;
	std::unique_ptr<Physics> physics;
	std::unique_ptr<Renderer> renderer;
	TransformSystem transformSystem;
	LevelLoader levelLoader;
	InputSystem input;
	bool isRunning;
	unsigned int msLastFrame;
	float dt;
	entt::registry registry;
	std::unordered_map < std::uint64_t, entt::entity> allEntities;

	entt::entity player;
	ImguiSystem imgui;


public:

	Game();
	~Game();
	void Setup();
	void Run();
	void Update();
	void Render();
	void UpdateInputSystem();
	void RegisterAllEntities();
};
