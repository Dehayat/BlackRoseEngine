#pragma once
#include <memory>
#include <box2d/box2d.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <ryml/ryml.hpp>
#include "Components/PhysicsBodyComponent.h"
#include "Components/TransformComponent.h"

class DebugDraw : public b2Draw
{
	SDL_Renderer* renderer;
	glm::mat3 matrix;
public:
	DebugDraw(SDL_Renderer* sdl);
	void SetMatrix(glm::mat3 worldToScreen);
	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
	void DrawCircle(const b2Vec2& center, float radius, const b2Color& color);
	void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color);
	void DrawTransform(const b2Transform& xf);
	void DrawPoint(const b2Vec2& p, float size, const b2Color& color);
};

class Physics
{
	DebugDraw* debugDrawer;
	bool drawDebug;
	std::unique_ptr<b2World> physicsWorld;
public:
	Physics(float gravityX, float gravityY);
	~Physics();
	void PhysicsBodyCreated(entt::registry& registry, entt::entity entity);
	void PhysicsBodyDestroyed(entt::registry& registry, entt::entity entity);
	void Update(entt::registry& registry);
	b2World& GetWorld();

	void InitDebugDrawer(SDL_Renderer* sdl);
	void EnableDebug(bool enable);
	DebugDraw* GetDebug();
	void DebugRender(glm::mat3 viewMatrix);
};