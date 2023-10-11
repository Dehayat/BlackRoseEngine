#include "Transform.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include "Logger.h"

TransformSystem::TransformSystem()
{
	debugDrawer = nullptr;
	drawDebug = false;
}

TransformSystem::~TransformSystem()
{
	if (debugDrawer != nullptr) {

		delete debugDrawer;
	}
}

void TransformSystem::Update(entt::registry& registry)
{
	registry.sort<Transform>([](const auto& lhs, const auto& rhs) {
		return lhs.level < rhs.level;
		});
	auto view3 = registry.view<Transform>();
	for (auto entity : view3) {
		auto& trx = view3.get<Transform>(entity);
		trx.matrix = glm::mat3(
			glm::cos(glm::radians(trx.rotation)) * trx.scale.x, -glm::sin(glm::radians(trx.rotation)) * trx.scale.y, trx.position.x,
			glm::sin(glm::radians(trx.rotation)) * trx.scale.x, glm::cos(glm::radians(trx.rotation)) * trx.scale.y, trx.position.y,
			0, 0, 1
		);

		if (trx.hasParent) {
			auto& parentTrx = registry.get<Transform>(trx.parent);
			trx.matrix = trx.matrix * parentTrx.matrix;
		}
	}
}

void TransformSystem::InitDebugDrawer(SDL_Renderer* sdl)
{
	debugDrawer = new DebugDrawTransform(sdl);
}

void TransformSystem::EnableDebug(bool enable)
{
	if (debugDrawer == nullptr) {
		Logger::Error("No Transform Debug Drawer attached");
	}
	else {
		drawDebug = enable;
	}
}

void TransformSystem::DebugRender(glm::mat3 viewMatrix, entt::registry& registry)
{
	debugDrawer->SetMatrix(viewMatrix);
	if (drawDebug) {
		auto view3 = registry.view<const Transform>();
		for (auto entity : view3) {
			const auto& pos = view3.get<Transform>(entity);
			debugDrawer->DrawTransform(pos);
		}
	}
}

void TransformSystem::SetParent(entt::registry& registry, Transform& child, entt::entity parent)
{
	if (!registry.valid(parent)) {
		child.parent = entt::entity(-1);
		child.level = 0;
		child.hasParent = false;
		return;
	}
	auto& parentTrx = registry.get<Transform>(parent);
	child.parent = parent;
	child.level = parentTrx.level + 1;
	child.hasParent = true;
}

DebugDrawTransform::DebugDrawTransform(SDL_Renderer* sdl) :matrix(1)
{
	this->renderer = sdl;
}

void DebugDrawTransform::SetMatrix(glm::mat3 worldToScreen)
{
	matrix = worldToScreen;
}

void DebugDrawTransform::DrawTransform(const Transform& t)
{
	glm::vec3 orig = glm::vec3(0, 0, 1);
	glm::vec3 dest = glm::vec3(0, 0.6f, 1);
	orig = orig * t.matrix * matrix;
	dest = dest * t.matrix * matrix;
	int b = (t.level) / 2.0 * 255;
	filledCircleRGBA(renderer, orig.x, orig.y, 15, 0, 200, b, 150);
	thickLineRGBA(renderer, orig.x, orig.y, dest.x, dest.y, 10, 0, 200, b, 150);
}

Transform::Transform(glm::vec2 position, glm::vec2 scale, float rotation) {
	this->position = position;
	this->scale = scale;
	this->rotation = rotation;
	this->matrix = glm::mat3(0);
	this->hasParent = false;
	this->parent = entt::entity(-1);
	this->level = 0;
}
Transform::Transform(ryml::NodeRef node) {

	float x, y;
	node["position"].first_child() >> x;
	node["position"].first_child().next_sibling() >> y;

	this->position = glm::vec2(x, y);
	this->scale = glm::vec2(1, 1);
	this->rotation = 0;
	this->matrix = glm::mat3(0);
	this->hasParent = false;
	this->parent = entt::entity(-1);
	this->level = 0;
}
