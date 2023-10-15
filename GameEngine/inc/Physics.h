#pragma once
#include <memory>
#include <box2d/box2d.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <ryml/ryml.hpp>
#include "Transform.h"

#ifdef _EDITOR
#include <imgui.h>
#endif

class Physics;

struct PhysicsBody {
	bool isInit;
	b2Body* body;
	b2PolygonShape shape;
	b2FixtureDef fixture;
	b2BodyDef bodyDef;
	PhysicsBody(Physics& physics, glm::vec2 pos = { 0.f,0.f }, glm::vec2 size = { 1.f,1.f }, bool isStatic = false, bool keepAwake = false);
	PhysicsBody(ryml::NodeRef node);
	~PhysicsBody();
	void Serialize(ryml::NodeRef node);
	void Init(Physics& physics, const Transform& trx);
	float sizex, sizey;
	bool isStatic;
	Physics* physics;
#ifdef _EDITOR
	void DrawEditor(Transform trx) {
		auto pos = b2Vec2(trx.position.x, trx.position.y);
		body->SetTransform(pos, glm::radians(trx.rotation));
		if (
			ImGui::DragFloat("size x", &sizex, 0.1f, 0.05f, 2000)
			|| ImGui::DragFloat("size y", &sizey, 0.1f, 0.05f, 2000)) {
			body->DestroyFixture(&body->GetFixtureList()[0]);
			shape.SetAsBox(sizex, sizey);
			body->CreateFixture(&fixture);
		}
		if (ImGui::Checkbox("Static", &isStatic)) {
			if (isStatic) {
				body->SetType(b2BodyType::b2_staticBody);
			}
			else {
				body->SetType(b2BodyType::b2_dynamicBody);
			}
		}
	}
#endif
};
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
	void Update(entt::registry& registry);
	b2World& GetWorld();
	void InitLoaded(entt::registry& registry);

	void InitDebugDrawer(SDL_Renderer* sdl);
	void EnableDebug(bool enable);
	DebugDraw* GetDebug();
	void DebugRender(glm::mat3 viewMatrix);
};