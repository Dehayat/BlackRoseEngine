#pragma once
#include <SDL2/SDL.h>

class ImguiSystem
{
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
public:
	ImguiSystem();
	~ImguiSystem();
	void Render();
	void Present();
	void HandleEvent(SDL_Event& event);
	bool ProcessEvents();
};
