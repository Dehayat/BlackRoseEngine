#pragma once
#include <string>

#include <SDL2/SDL.h>

class SdlContainer
{
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
public:
	SdlContainer(int windowWidth, int windowHeight, std::string windowName = "Rose Engine");
	~SdlContainer();
	SDL_Renderer* GetRenderer();
	SDL_Window* GetWindow();
	bool ProcessEvents();
};

