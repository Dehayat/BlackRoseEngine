#include "Physics.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include "Transform.h"
#include "Logger.h"

PhysicsBody::PhysicsBody(Physics& physics, glm::vec2 pos, glm::vec2 size, bool isStatic, bool keepAwake)
{
	if (!isStatic) {
		bodyDef.type = b2_dynamicBody;
	}
	bodyDef.position.Set(pos.x, pos.y);
	b2Body* body = physics.GetWorld().CreateBody(&bodyDef);
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(size.x, size.y);
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	body->CreateFixture(&fixtureDef);
	//body->SetFixedRotation(true);
	body->SetSleepingAllowed(!keepAwake);
	this->body = body;
	isInit = true;
#ifdef _EDITOR
	sizex = size.x;
	sizey = size.y;
	this->isStatic = isStatic;
#endif // _EDITOR
}
PhysicsBody::PhysicsBody(ryml::NodeRef node)
{
	float x = 0.5, y = 0.5;
	bool isStatic = false;
	if (node.is_map() && node.has_child("size")) {
		node["size"][0] >> x;
		node["size"][1] >> y;
	}
	if (node.is_map() && node.has_child("static")) {
		isStatic = true;
	}
	if (!isStatic) {
		bodyDef.type = b2_dynamicBody;
	}
	shape.SetAsBox(x, y);
	fixture.shape = &shape;
	fixture.density = 1.0f;
	fixture.friction = 0.3f;

	this->body = nullptr;
	isInit = false;

#ifdef _EDITOR
	sizex = x;
	sizey = y;
	this->isStatic = isStatic;
#endif // _EDITOR
}
void PhysicsBody::Init(Physics& physics, const Transform& trx)
{
	bodyDef.position.Set(trx.position.x, trx.position.y);
	b2Body* body = physics.GetWorld().CreateBody(&bodyDef);
	body->CreateFixture(&fixture);
	this->body = body;
	isInit = true;
}

Physics::Physics(float gravityX, float gravityY) {
	b2Vec2 gravity(gravityX, gravityY);
	physicsWorld = std::make_unique<b2World>(gravity);
	drawDebug = false;
	debugDrawer = nullptr;
}
Physics::~Physics()
{
	if (debugDrawer != nullptr) {
		delete debugDrawer;
	}
}
void Physics::Update(entt::registry& registry)
{
	auto phView = registry.view<PhysicsBody, Transform>();
	for (auto entity : phView) {
		const auto& pos = phView.get<Transform>(entity);
		auto& body = phView.get<PhysicsBody>(entity);
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
		auto& pos = phView.get<Transform>(entity);
		const auto& body = phView.get<PhysicsBody>(entity);
		pos.position = glm::vec2(body.body->GetPosition().x, body.body->GetPosition().y);
		pos.rotation = glm::degrees(body.body->GetAngle());
	}
}
b2World& Physics::GetWorld()
{
	return *physicsWorld;
}
void Physics::InitLoaded(entt::registry& registry)
{
	auto phView = registry.view<PhysicsBody, Transform>();
	for (auto entity : phView) {
		const auto& pos = phView.get<Transform>(entity);
		auto& body = phView.get<PhysicsBody>(entity);
		if (body.isInit) {
			continue;
		}
		body.Init(*this, pos);
	}
}

void Physics::InitDebugDrawer(SDL_Renderer* sdl) {
	debugDrawer = new DebugDraw(sdl);
	physicsWorld->SetDebugDraw(debugDrawer);
}
void Physics::EnableDebug(bool enable) {
	if (debugDrawer == nullptr) {
		Logger::Error("No Physics Debug Drawer attached");
	}
	else {
		drawDebug = enable;
	}
}
DebugDraw* Physics::GetDebug()
{
	return debugDrawer;
}
void Physics::DebugRender(glm::mat3 viewMatrix)
{
	debugDrawer->SetMatrix(viewMatrix);
	if (drawDebug) {
		physicsWorld->DebugDraw();
	}
}

DebugDraw::DebugDraw(SDL_Renderer* sdl) :matrix(1) {
	this->renderer = sdl;
	SetFlags(15);
}
void DebugDraw::SetMatrix(glm::mat3 worldToScreen) {
	matrix = worldToScreen;
}
void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
	SDL_SetRenderDrawColor(renderer, color.r * 255, color.g * 255, color.b * 255, color.a * 255);
	std::unique_ptr<Sint16[]> vx(new Sint16[vertexCount]);
	std::unique_ptr<Sint16[]> vy(new Sint16[vertexCount]);

	for (int i = 0;i < vertexCount;i++) {
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

	for (int i = 0;i < vertexCount;i++) {
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
