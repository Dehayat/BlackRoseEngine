#pragma once
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <entt/entity/entity.hpp>

#include "Components/TransformComponent.h"

class DebugDrawTransform
{
	SDL_Renderer* renderer;
	glm::mat3 matrix;
public:
	DebugDrawTransform();
	void SetMatrix(glm::mat3 worldToScreen);
	void DrawTransform(const TransformComponent& t, bool selected);
};

class TransformSystem
{
	void TransformCreated(entt::registry& registry, entt::entity);
	void TransformDestroyed(entt::registry& registry, entt::entity);
	void ParentUpdated(entt::registry& registry, entt::entity);
	DebugDrawTransform* debugDrawer;
	bool drawDebug;
public:
	TransformSystem();
	~TransformSystem();
	void Update();
	void InitDebugDrawer();
	void EnableDebug(bool enable);
	void DebugRender(glm::mat3 viewMatrix, entt::entity selectedEntity);
	entt::entity GetChild(entt::entity entity, const std::string& name);
	static void MoveTransformToWorldSpace(TransformComponent& trx);
	static void MoveTransformToParentSpace(TransformComponent& trx, TransformComponent& source);
	static glm::mat3 CalcMatrix(TransformComponent& trx);
	DebugDrawTransform& GetDebugRenderer();
};