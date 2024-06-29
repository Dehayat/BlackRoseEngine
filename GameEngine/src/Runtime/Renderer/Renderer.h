#pragma once
#include <SDL2/SDL.h>
#include <entt/entity/entity.hpp>
#include <glm/glm.hpp>

class RendererSystem
{
	SDL_Renderer* sdlRenderer;
	entt::entity camera;
	glm::mat3 worldToScreenMatrix;
	float aspectRatio;
public:
	glm::vec2 editorViewPos;
	float editorViewHeight;
	RendererSystem();
	~RendererSystem();
	void Render();
	void Present();
	void SetCamera(entt::entity cam);
	entt::entity GetCamera() const;
	const glm::mat3 GetWorldToScreenMatrix() const;
	const glm::mat3 GetScreenToWorldMatrix() const;
	void InitLoaded();
	float GetAspectRatio();
};