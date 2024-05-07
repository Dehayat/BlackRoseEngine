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
	void Update(entt::registry& registry);
	void InitLoaded(entt::registry& registry, std::unordered_map < std::uint64_t, entt::entity>& allEntities);
	void InitDebugDrawer(SDL_Renderer* sdl);
	void EnableDebug(bool enable);
	void DebugRender(glm::mat3 viewMatrix, entt::registry& registry);
	void SetParent(entt::registry& registry, TransformComponent& child, entt::entity parent);
	void TransformCreated(entt::registry& registry, entt::entity entity);
	static glm::mat3 CalcMatrix(TransformComponent& trx);
#ifdef _DEBUG
	DebugDrawTransform& GetDebugRenderer();
#endif // _DEBUG
};