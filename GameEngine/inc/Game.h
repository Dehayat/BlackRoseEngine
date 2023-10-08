#pragma once
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include "EventSystem/EventBus.h"

const int FPS = 60;
const int FRAMETIME_MS = 1000 / FPS;

class Game {
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	bool isRunning;
	unsigned int msLastFrame;
	float dt;
	std::unique_ptr<EventBus> eventBus;
	SDL_Rect camera;
	int windowWidth;
	int windowHeight;

public:

	Game();
	~Game();
	void Setup();
	void Run();
	void ProcessInput();
	void Update();
	void Render();
};