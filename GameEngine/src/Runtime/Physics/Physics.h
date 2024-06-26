#pragma once
#include <memory>

#include <box2d/box2d.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "Components/PhysicsBodyComponent.h"
#include "Components/TransformComponent.h"

class DebugDraw : public b2Draw
{
	SDL_Renderer* renderer;
	glm::mat3 matrix;
public:
	DebugDraw();
	void SetMatrix(glm::mat3 worldToScreen);
	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
	void DrawCircle(const b2Vec2& center, float radius, const b2Color& color);
	void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color);
	void DrawTransform(const b2Transform& xf);
	void DrawPoint(const b2Vec2& p, float size, const b2Color& color);
};


class PhysicsSystem
{
	DebugDraw* debugDrawer;
	bool drawDebug;
	std::unique_ptr<b2World> physicsWorld;
	std::unique_ptr<b2ContactListener> contactListener;
	void PhysicsBodyCreated(entt::registry& registry, entt::entity entity);
	void CreateEntityBody(entt::registry& registry, entt::entity entity);
	void PhysicsBodyDestroyed(entt::registry& registry, entt::entity entity);
	void DestroyEntityBody(entt::registry& registry, entt::entity entity);
	void EntityDisabled(entt::registry& registry, entt::entity entity);
	void EntityEnabled(entt::registry& registry, entt::entity entity);
public:
	PhysicsSystem(float gravityX, float gravityY);
	~PhysicsSystem();
	void CopyTransformToBody(PhysicsBodyComponent& phys, TransformComponent& trx);
	void CopyBodyToTransform(PhysicsBodyComponent& phys, TransformComponent& trx);
	void RemoveBody(PhysicsBodyComponent& phys);
	void AddBody(entt::entity entity, PhysicsBodyComponent& phys);
	void Update();
	b2World& GetWorld();

	void InitDebugDrawer();
	void EnableDebug(bool enable);
	DebugDraw* GetDebug();
	void DebugRender(glm::mat3 viewMatrix);
};