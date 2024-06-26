#pragma once
#include <SDL2/SDL.h>

class ImguiSystem
{
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	bool capturedMouse;
public:
	ImguiSystem();
	~ImguiSystem();
	void Render();
	void Present();
	void HandleEvent(SDL_Event& event);
	bool IsMouseCaptured();
};
