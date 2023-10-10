#pragma once
#include <SDL2/SDL.h>
#include <imgui.h>
#include <glm/glm.hpp>

class ImguiSystem
{
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
public:
	ImguiSystem();
	~ImguiSystem();
	void Render();
	void HandleEvent(SDL_Event& event);
};


extern float x, y;