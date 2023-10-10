#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include "SdlContainer.h"
#include "AssetStore.h"
#include "ImguiSystem.h"
#include "Transform.h"
#include "Physics.h"

const int FPS = 60;
const int FRAMETIME_MS = 1000 / FPS;

struct KeyData {
	bool justPressed;
	bool justReleased;
	bool isPressed;
	KeyData() {
		justPressed = false;
		justReleased = false;
		isPressed = false;
	}
};

struct InputData {
	KeyData keys[4];
};



class Game {
private:
	std::unique_ptr<SdlContainer> sdl;
	std::unique_ptr<AssetStore> assetStore;
	std::unique_ptr<Physics> physics;
	TransformSystem transformSystem;
	bool isRunning;
	unsigned int msLastFrame;
	float dt;
	entt::registry registry;
	
	entt::entity player;
	entt::entity camera;
	InputData input;
	ImguiSystem imgui;


public:

	Game();
	~Game();
	void Setup();
	void Run();
	void ProcessEvents();
	void Update();
	void Render();
	void UpdateInputSystem();
};
