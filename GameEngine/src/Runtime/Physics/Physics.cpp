#include "Physics/Physics.h"

#include <SDL2/SDL2_gfxPrimitives.h>

#include "SdlContainer.h"
#include "TimeSystem.h"

#include "Systems.h"

#include "Debugging/Logger.h"

PhysicsSystem::PhysicsSystem(float gravityX, float gravityY) {
	b2Vec2 gravity(gravityX, gravityY);
	physicsWorld = std::make_unique<b2World>(gravity);
	drawDebug = false;
	debugDrawer = nullptr;
	entt::registry& registry = GETSYSTEM(Entities).GetRegistry();
	registry.on_construct<PhysicsBodyComponent>().connect<&PhysicsSystem::PhysicsBodyCreated>(this);
	registry.on_destroy<PhysicsBodyComponent>().connect<&PhysicsSystem::PhysicsBodyDestroyed>(this);
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
	phys.bodyDef.position.Set(trx.globalPosition.x, trx.globalPosition.y);

	b2Body* body = GetWorld().CreateBody(&phys.bodyDef);
	auto globalScale = glm::abs(trx.globalScale);
	if (globalScale.x < 0.01) {
		globalScale.x = 0.01;
	}
	if (globalScale.y < 0.01) {
		globalScale.y = 0.01;
	}
	phys.globalSize = vec2(phys.size.x * globalScale.x, phys.size.y * globalScale.y);
	phys.shape.SetAsBox(phys.globalSize.x / 2, phys.globalSize.y / 2);

	phys.fixture.shape = &phys.shape;
	phys.fixture.density = 1.0f;
	phys.fixture.friction = 0.3f;
	phys.fixture.isSensor = phys.isSensor;
	body->CreateFixture(&phys.fixture);
	phys.body = body;
	phys.body->SetTransform(b2Vec2(trx.globalPosition.x, trx.globalPosition.y), glm::radians(trx.globalRotation));
}
void PhysicsSystem::PhysicsBodyDestroyed(entt::registry& registry, entt::entity entity)
{
	auto& phys = registry.get<PhysicsBodyComponent>(entity);
	GetWorld().DestroyBody(phys.body);
}
void PhysicsSystem::CopyTransformToBody(PhysicsBodyComponent& phys, TransformComponent& trx)
{
	//Logger::Log("PosX: " + std::to_string(trx.globalPosition.x));
	//Logger::Log("PosY: " + std::to_string(trx.globalPosition.y));
	//Logger::Log("Rot: " + std::to_string(trx.globalRotation));

	//Logger::Log("->PosX: " + std::to_string(phys.body->GetPosition().x));
	//Logger::Log("->PosY: " + std::to_string(phys.body->GetPosition().y));
	//Logger::Log("->Rot: " + std::to_string(glm::degrees(phys.body->GetAngle())));

	auto globalScale = glm::abs(trx.globalScale);
	if (globalScale.x < 0.01) {
		globalScale.x = 0.01;
	}
	if (globalScale.y < 0.01) {
		globalScale.y = 0.01;
	}
	auto newSize = vec2(phys.size.x * globalScale.x, phys.size.y * globalScale.y);
	if (phys.globalSize != newSize) {
		phys.body->DestroyFixture(&phys.body->GetFixtureList()[0]);
		phys.globalSize = newSize;
		phys.shape.SetAsBox(phys.globalSize.x / 2, phys.globalSize.y / 2);
		phys.body->CreateFixture(&phys.fixture);
	}
	phys.body->SetTransform(b2Vec2(trx.globalPosition.x, trx.globalPosition.y), glm::radians(trx.globalRotation));
	phys.body->SetAwake(true);
}
void PhysicsSystem::CopyBodyToTransform(PhysicsBodyComponent& phys, TransformComponent& trx)
{
	trx.globalPosition = glm::vec2(phys.body->GetPosition().x, phys.body->GetPosition().y);
	trx.globalRotation = glm::degrees(phys.body->GetAngle());
	trx.UpdateLocals();
	trx.UpdateGlobals();

	//Logger::Log("PosX: " + std::to_string(trx.globalPosition.x));
	//Logger::Log("PosY: " + std::to_string(trx.globalPosition.y));
	//Logger::Log("Rot: " + std::to_string(trx.globalRotation));

	//Logger::Log("->PosX: " + std::to_string(phys.body->GetPosition().x));
	//Logger::Log("->PosY: " + std::to_string(phys.body->GetPosition().y));
	//Logger::Log("->Rot: " + std::to_string(glm::degrees(phys.body->GetAngle())));
}
void PhysicsSystem::Update()
{
	Entities& entities = entt::locator<Entities>::value();
	entt::registry& registry = entities.GetRegistry();
	auto phView = registry.view<PhysicsBodyComponent, TransformComponent>();
	for (auto entity : phView) {
		auto& pos = phView.get<TransformComponent>(entity);
		auto& body = phView.get<PhysicsBodyComponent>(entity);
		CopyTransformToBody(body, pos);
		//Logger::Log("->Rot: " + std::to_string(glm::degrees(body.body->GetAngle())));
	}

	TimeSystem& timeSystem = GETSYSTEM(TimeSystem);
	float timeStep = timeSystem.GetdeltaTime();
	int velocityIterations = 10;
	int positionIterations = 12;

	physicsWorld->Step(timeStep, velocityIterations, positionIterations);

	for (auto entity : phView) {
		auto& pos = phView.get<TransformComponent>(entity);
		auto& body = phView.get<PhysicsBodyComponent>(entity);
		//Logger::Log("Rot: " + std::to_string(glm::degrees(body.body->GetAngle())));
		CopyBodyToTransform(body, pos);
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
	SdlContainer& sdlRenderer = entt::locator<SdlContainer>::value();
	this->renderer = sdlRenderer.GetRenderer();
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
