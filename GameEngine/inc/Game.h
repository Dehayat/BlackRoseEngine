#pragma once
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include "EventSystem/EventBus.h"
#include "AssetStore.h"
#include "ImguiSystem.h"

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
	SDL_Window* window;
	SDL_Renderer* renderer;
	bool isRunning;
	unsigned int msLastFrame;
	float dt;
	std::unique_ptr<EventBus> eventBus;
	int windowWidth;
	int windowHeight;
	entt::registry registry;
	std::unique_ptr<AssetStore> assetStore;
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