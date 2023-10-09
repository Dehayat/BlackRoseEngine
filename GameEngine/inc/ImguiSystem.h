#pragma once
#include <SDL2/SDL.h>
#include <imgui.h>

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

