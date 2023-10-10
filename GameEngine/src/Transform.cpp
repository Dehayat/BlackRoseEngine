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
	auto view3 = registry.view<Transform>();
	for (auto entity : view3) {
		auto& pos = view3.get<Transform>(entity);
		pos.matrix = glm::mat3(
			glm::cos(glm::radians(pos.rotation)) * pos.scale.x, -glm::sin(glm::radians(pos.rotation)) * pos.scale.y, pos.position.x,
			glm::sin(glm::radians(pos.rotation)) * pos.scale.x, glm::cos(glm::radians(pos.rotation)) * pos.scale.y, pos.position.y,
			0, 0, 1
		);
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
	filledCircleRGBA(renderer, orig.x, orig.y, 20, 0, 200, 50, 100);
	thickLineRGBA(renderer, orig.x, orig.y, dest.x, dest.y, 10, 0, 200, 50, 100);
}

inline Transform::Transform(glm::vec2 position, glm::vec2 scale, float rotation) {
	this->position = position;
	this->scale = scale;
	this->rotation = rotation;
	this->matrix = glm::mat3(0);
}
