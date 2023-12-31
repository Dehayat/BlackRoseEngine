#pragma once
#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include "AssetStore.h"
#ifdef _EDITOR
#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"
#endif

class Renderer
{
	SDL_Renderer* sdl;
	entt::entity camera;
	glm::mat3 worldToScreenMatrix;
#ifdef _EDITOR
	Camera editorCam;
	Transform editorCamTrx;
#endif
public:
	Renderer(SDL_Renderer* sdl);
	~Renderer();
	void Render(entt::registry* registry, const AssetStore& assetStore);
	void Present();
	void SetCamera(entt::entity cam);
	entt::entity GetCamera();
	const glm::mat3 GetWorldToScreenMatrix();
	const glm::mat3 GetScreenToWorldMatrix();
	void InitLoaded(entt::registry& registry);
#ifdef _EDITOR
	Transform& GetEditorCamTrx() {
		return editorCamTrx;
	}
#endif
};

