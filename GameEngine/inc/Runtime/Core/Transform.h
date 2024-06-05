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
	DebugDrawTransform();
	void SetMatrix(glm::mat3 worldToScreen);
	void DrawTransform(const TransformComponent& t, bool selected);
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
	void InitDebugDrawer();
	void EnableDebug(bool enable);
	void DebugRender(glm::mat3 viewMatrix, entt::entity selectedEntity);
	void SetParent(entt::entity entity, entt::entity parent);
	void TransformCreated(entt::registry& registry, entt::entity);
	void TransformDestroyed(entt::registry& registry, entt::entity);
	static void BakeTransform(TransformComponent& trx);
	static void MoveTransformToParentSpace(TransformComponent& trx, TransformComponent& source);
	static glm::mat3 CalcMatrix(TransformComponent& trx);
	static glm::mat3 CalcMatrixL2W(TransformComponent& trx);
#ifdef _DEBUG
	DebugDrawTransform& GetDebugRenderer();
#endif // _DEBUG
};