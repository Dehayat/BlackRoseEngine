#pragma once
#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include "AssetStore.h"

struct Sprite {
	std::string sprite;
	int layer;
	SDL_Color color;
	Sprite(std::string sprite, int layer = 0, SDL_Color color = SDL_Color{ 255,255,255,255 });
};

struct Camera {
	float height;
	glm::mat3 camToScreen;
	glm::mat3 worldToScreen;
	Camera(float height = 10);
};


class Renderer
{
	SDL_Renderer* sdl;
	entt::entity camera;
	glm::mat3 worldToScreenMatrix;
public:
	Renderer(SDL_Renderer* sdl);
	~Renderer();
	void Render(entt::registry* registry, const AssetStore& assetStore);
	void Present();
	void SetCamera(entt::entity cam);
	entt::entity GetCamera();
	glm::mat3 GetWorldToScreenMatrix();
	glm::mat3 GetScreenToWorldMatrix();
};

