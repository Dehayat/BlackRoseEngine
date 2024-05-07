#pragma once
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include "Components/TransformComponent.h"

class DebugDrawTransform
{
	SDL_Renderer* renderer;
	glm::mat3 matrix;
public:
	DebugDrawTransform(SDL_Renderer* sdl);
	void SetMatrix(glm::mat3 worldToScreen);
	void DrawTransform(const TransformComponent& t);
};

class TransformSystem
{
	DebugDrawTransform* debugDrawer;
	bool drawDebug;
public:
	TransformSystem();
	~TransformSystem();
	void Update();
	void InitLoaded();
	void InitDebugDrawer(SDL_Renderer* sdl);
	void EnableDebug(bool enable);
	void DebugRender(glm::mat3 viewMatrix);
	void SetParent(TransformComponent& child, entt::entity parent);
	void TransformCreated(entt::registry& registry, entt::entity);
	static glm::mat3 CalcMatrix(TransformComponent& trx);
#ifdef _DEBUG
	DebugDrawTransform& GetDebugRenderer();
#endif // _DEBUG
};