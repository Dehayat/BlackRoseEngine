#include "Physics.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include "Logger.h"
#include "SdlContainer.h"

PhysicsSystem::PhysicsSystem(float gravityX, float gravityY) {
	b2Vec2 gravity(gravityX, gravityY);
	physicsWorld = std::make_unique<b2World>(gravity);
	drawDebug = false;
	debugDrawer = nullptr;
}
PhysicsSystem::~PhysicsSystem()
{
	if (debugDrawer != nullptr) {
		delete debugDrawer;
	}
}
void PhysicsSystem::PhysicsBodyCreated(entt::registry& registry, entt::entity entity)
{
	auto& phys = registry.get<PhysicsBodyComponent>(entity);
	auto& trx = registry.get<TransformComponent>(entity);
	if (!phys.isStatic) {
		phys.bodyDef.type = b2_dynamicBody;
	}
	phys.bodyDef.position.Set(trx.position.x, trx.position.y);
	b2Body* body = GetWorld().CreateBody(&phys.bodyDef);
	phys.shape.SetAsBox(phys.size.x, phys.size.y);
	phys.fixture.shape = &phys.shape;
	phys.fixture.density = 1.0f;
	phys.fixture.friction = 0.3f;
	body->CreateFixture(&phys.fixture);
	phys.body = body;
}
void PhysicsSystem::PhysicsBodyDestroyed(entt::registry& registry, entt::entity entity)
{
	auto& phys = registry.get<PhysicsBodyComponent>(entity);
	GetWorld().DestroyBody(phys.body);
}

void PhysicsSystem::Update()
{
	Entities& entities = entt::locator<Entities>::value();
	entt::registry& registry = entities.GetRegistry();
	auto phView = registry.view<PhysicsBodyComponent, TransformComponent>();
	for (auto entity : phView) {
		const auto& pos = phView.get<TransformComponent>(entity);
		auto& body = phView.get<PhysicsBodyComponent>(entity);
		auto rotation = body.body->GetAngle();
		auto position = body.body->GetPosition();
		position.x = pos.position.x;
		position.y = pos.position.y;
		rotation = glm::radians(pos.rotation);
		body.body->SetTransform(position, rotation);
		body.body->SetAwake(true);
	}

	float timeStep = 1.0f / 60.0f;
	int velocityIterations = 10;
	int positionIterations = 12;
	physicsWorld->Step(timeStep, velocityIterations, positionIterations);

	for (auto entity : phView) {
		auto& pos = phView.get<TransformComponent>(entity);
		const auto& body = phView.get<PhysicsBodyComponent>(entity);
		pos.position = glm::vec2(body.body->GetPosition().x, body.body->GetPosition().y);
		pos.rotation = glm::degrees(body.body->GetAngle());
	}
}
b2World& PhysicsSystem::GetWorld()
{
	return *physicsWorld;
}

void PhysicsSystem::InitDebugDrawer() {
	debugDrawer = new DebugDraw();
	physicsWorld->SetDebugDraw(debugDrawer);
}
void PhysicsSystem::EnableDebug(bool enable) {
	if (debugDrawer == nullptr) {
		Logger::Error("No Physics Debug Drawer attached");
	}
	else {
		drawDebug = enable;
	}
}
DebugDraw* PhysicsSystem::GetDebug()
{
	return debugDrawer;
}
void PhysicsSystem::DebugRender(glm::mat3 viewMatrix)
{
	debugDrawer->SetMatrix(viewMatrix);
	if (drawDebug) {
		physicsWorld->DebugDraw();
	}
}

DebugDraw::DebugDraw() :matrix(1) {
	SdlContainer& sdl = entt::locator<SdlContainer>::value();
	this->renderer = sdl.GetRenderer();
	SetFlags(15);
}
void DebugDraw::SetMatrix(glm::mat3 worldToScreen) {
	matrix = worldToScreen;
}
void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
	SDL_SetRenderDrawColor(renderer, color.r * 255, color.g * 255, color.b * 255, color.a * 255);
	std::unique_ptr<Sint16[]> vx(new Sint16[vertexCount]);
	std::unique_ptr<Sint16[]> vy(new Sint16[vertexCount]);

	for (int i = 0; i < vertexCount; i++) {
		glm::vec3 pos = glm::vec3(vertices[i].x, vertices[i].y, 1);
		pos = pos * matrix;
		b2Vec2 nextVertex;
		if (i == vertexCount - 1) {
			nextVertex = vertices[0];
		}
		else {
			nextVertex = vertices[i + 1];
		}
		glm::vec3 nextPos = glm::vec3(vertices[i].x, vertices[i].y, 1);
		nextPos = nextPos * matrix;
		vx[i] = pos[0];
		vy[i] = pos[1];
	}
	polygonRGBA(renderer, vx.get(), vy.get(), vertexCount, color.r * 255, color.g * 255, color.b * 255, color.a / 2 * 255);
}
void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
	SDL_SetRenderDrawColor(renderer, color.r * 255, color.g * 255, color.b * 255, color.a / 2 * 255);
	std::unique_ptr<Sint16[]> vx(new Sint16[vertexCount]);
	std::unique_ptr<Sint16[]> vy(new Sint16[vertexCount]);

	for (int i = 0; i < vertexCount; i++) {
		glm::vec3 pos = glm::vec3(vertices[i].x, vertices[i].y, 1);
		pos = pos * matrix;
		b2Vec2 nextVertex;
		if (i == vertexCount - 1) {
			nextVertex = vertices[0];
		}
		else {
			nextVertex = vertices[i + 1];
		}
		glm::vec3 nextPos = glm::vec3(vertices[i].x, vertices[i].y, 1);
		nextPos = nextPos * matrix;
		vx[i] = pos[0];
		vy[i] = pos[1];
	}
	filledPolygonRGBA(renderer, vx.get(), vy.get(), vertexCount, color.r * 255, color.g * 255, color.b * 255, color.a / 2 * 255);
}
void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
	glm::vec3 pos = glm::vec3(p1.x, p1.y, 1);
	pos = pos * matrix;
	glm::vec3 nextPos = glm::vec3(p2.x, p2.y, 1);
	nextPos = nextPos * matrix;
	thickLineRGBA(renderer, pos[0], pos[1], nextPos[0], nextPos[1], 10, color.r * 255, color.g * 255, color.b * 255, color.a / 2 * 255);
}
void DebugDraw::DrawCircle(const b2Vec2& center, float radius, const b2Color& color) {
	glm::vec3 pos = glm::vec3(center.x, center.y, 1);
	glm::mat3 posMat = glm::mat3(radius, 0, pos.x, 0, radius, pos.y, 0, 0, 1);
	posMat = posMat * matrix;
	circleRGBA(renderer, posMat[0][2], posMat[1][2], posMat[0][0], color.r * 255, color.g * 255, color.b * 255, color.a / 2 * 255);
}
void DebugDraw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
{
	glm::vec3 pos = glm::vec3(center.x, center.y, 1);
	glm::mat3 posMat = glm::mat3(radius, 0, pos.x, 0, radius, pos.y, 0, 0, 1);
	posMat = posMat * matrix;
	filledCircleRGBA(renderer, posMat[0][2], posMat[1][2], posMat[0][0], color.r * 255, color.g * 255, color.b * 255, color.a / 2 * 255);
}
void DebugDraw::DrawTransform(const b2Transform& xf) {
	glm::vec3 pos = glm::vec3(xf.p.x, xf.p.y, 1);
	pos = pos * matrix;
	filledCircleRGBA(renderer, pos[0], pos[1], 5, 50, 0, 0, 50);
}
void DebugDraw::DrawPoint(const b2Vec2& p, float size, const b2Color& color) {
	glm::vec3 pos = glm::vec3(p.x, p.y, 1);
	pos = pos * matrix;
	filledCircleRGBA(renderer, pos[0], pos[1], 5, color.r * 255, color.g * 255, color.b * 255, color.a / 2 * 255);
}
