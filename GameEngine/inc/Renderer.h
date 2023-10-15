#pragma once
#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include "AssetStore.h"

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
	void InitLoaded(entt::registry& registry);
};

