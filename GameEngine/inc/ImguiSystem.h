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
	void Render(glm::mat3 view, glm::mat3 world, glm::mat3 cam, glm::vec2 pos);
	void HandleEvent(SDL_Event& event);
};


extern float x, y;