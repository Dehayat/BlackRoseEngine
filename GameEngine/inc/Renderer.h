#pragma once
#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include "AssetStore.h"
#ifdef _EDITOR
#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"
#endif

class RendererSystem
{
	SDL_Renderer* sdl;
	entt::entity camera;
	glm::mat3 worldToScreenMatrix;
#ifdef _EDITOR
	Camera editorCam;
	Transform editorCamTrx;
#endif
public:
	RendererSystem();
	~RendererSystem();
	void Render();
	void Present();
	void SetCamera(entt::entity cam);
	entt::entity GetCamera();
	const glm::mat3 GetWorldToScreenMatrix();
	const glm::mat3 GetScreenToWorldMatrix();
	void InitLoaded();
#ifdef _EDITOR
	Transform& GetEditorCamTrx() {
		return editorCamTrx;
	}
#endif
};

