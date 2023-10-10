#pragma once
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

class SdlContainer
{
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
public:
	SdlContainer(int windowWidth, int windowHeight);
	~SdlContainer();
	SDL_Renderer* GetRenderer();
	SDL_Window* GetWindow();
	glm::ivec2 GetWindowSize();
};

