#pragma once
#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <ryml/ryml.hpp>
#include "AssetStore.h"

struct Camera {
	float height;
	glm::mat3 camToScreen;
	glm::mat3 worldToScreen;
	bool startCamera;
	Camera(float height = 10);
	Camera(ryml::NodeRef node);
	void Serialize(ryml::NodeRef node);
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
	void InitLoaded(entt::registry& registry);
};

