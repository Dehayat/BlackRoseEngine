#include "Core/Transform.h"

#include <SDL2/SDL2_gfxPrimitives.h>
#include <entt/entity/registry.hpp>

#include "Core/SdlContainer.h"
#include "Core/Entity.h"

#include "Editor/LevelTree.h"

#include "Core/Systems.h"

#include "Core/Log.h"

TransformSystem::TransformSystem()
{
	debugDrawer = nullptr;
	drawDebug = false;

	entt::registry& registry = ROSE_GETSYSTEM(Entities).GetRegistry();
	registry.on_construct<TransformComponent>().connect<&TransformSystem::TransformCreated>(this);
	registry.on_destroy<TransformComponent>().connect<&TransformSystem::TransformDestroyed>(this);
}
TransformSystem::~TransformSystem()
{
	if (debugDrawer != nullptr) {

		delete debugDrawer;
	}
}
void TransformSystem::TransformCreated(entt::registry& registry, entt::entity entity)
{
	auto& trx = registry.get<TransformComponent>(entity);
	auto& entities = ROSE_GETSYSTEM(Entities);
	ROSE_GETSYSTEM(LevelTree).InsertEntity(entity);
	trx.UpdateGlobals();
}
entt::entity TransformSystem::GetChild(entt::entity entity, const std::string& name)
{
	return ROSE_GETSYSTEM(LevelTree).GetChild(entity, name);
}
void TransformSystem::TransformDestroyed(entt::registry& registry, entt::entity entity)
{
	auto& trx = registry.get<TransformComponent>(entity);
	ROSE_GETSYSTEM(LevelTree).TransformDestroyed(registry, entity);
}
void TransformSystem::MoveTransformToWorldSpace(TransformComponent& trx)
{
	trx.position = trx.globalPosition;
	trx.scale = trx.globalScale;
	trx.rotation = trx.globalRotation;
	trx.UpdateGlobals();
}
void TransformSystem::MoveTransformToParentSpace(TransformComponent& child, TransformComponent& parent)
{
	auto matrixW2sL = glm::inverse(parent.matrixL2W);
	auto matrixtL2sL = child.matrixL2W * matrixW2sL;
	child.position = TransformComponent::GetPosition(matrixtL2sL);
	child.scale = TransformComponent::GetScale(matrixtL2sL);
	child.rotation = TransformComponent::GetRotation(matrixtL2sL);
	child.UpdateGlobals();
}
void TransformSystem::Update()
{
	entt::registry& registry = ROSE_GETSYSTEM(Entities).GetRegistry();

	registry.sort<TransformComponent>([](const auto& lhs, const auto& rhs) {
		return lhs.level < rhs.level;
		});
	auto view3 = registry.view<TransformComponent>();
	for (auto entity : view3) {
		auto& trx = view3.get<TransformComponent>(entity);
		trx.CalcMatrix();
		trx.UpdateGlobals();
	}
}

glm::mat3 TransformSystem::CalcMatrix(TransformComponent& trx)
{
	return glm::mat3(
		glm::cos(glm::radians(trx.rotation)) * trx.scale.x, -glm::sin(glm::radians(trx.rotation)) * trx.scale.y, trx.position.x,
		glm::sin(glm::radians(trx.rotation)) * trx.scale.x, glm::cos(glm::radians(trx.rotation)) * trx.scale.y, trx.position.y,
		0, 0, 1
	);
}

void TransformSystem::InitDebugDrawer()
{
	SdlContainer& sdlRenderer = entt::locator<SdlContainer>::value();
	debugDrawer = new DebugDrawTransform();
}
void TransformSystem::EnableDebug(bool enable)
{
	if (debugDrawer == nullptr) {
		ROSE_ERR("No Transform Debug Drawer attached");
	}
	else {
		drawDebug = enable;
	}
}
void TransformSystem::DebugRender(glm::mat3 viewMatrix, entt::entity selectedEntity)
{
	Entities& entities = entt::locator<Entities>::value();
	entt::registry& registry = entities.GetRegistry();
	debugDrawer->SetMatrix(viewMatrix);
	if (drawDebug) {
		auto view3 = registry.view<const TransformComponent>();
		for (auto entity : view3) {
			const auto& pos = view3.get<TransformComponent>(entity);
			debugDrawer->DrawTransform(pos, selectedEntity == entity);
		}
	}
}
void TransformSystem::SetParent(entt::entity entity, entt::entity parent)
{
	Entities& entities = ROSE_GETSYSTEM(Entities);
	entt::registry& registry = entities.GetRegistry();
	auto& child = registry.get<TransformComponent>(entity);
	auto oldParent = NoEntity();
	if (child.hasParent) {
		oldParent = child.parent;
		ROSE_GETSYSTEM(LevelTree).RemoveParent(entity);
		child.hasParent = false;
		child.parent = NoEntity();
		child.level = 0;
	}

	if (parent != NoEntity()) {
		if (ROSE_GETSYSTEM(LevelTree).TrySetParent(entity, parent)) {
			child.parent = parent;
			auto& parentTrx = registry.get<TransformComponent>(parent);
			child.level = parentTrx.level + 1;
			child.hasParent = true;
			child.parentGUID = entities.GetEntityGuid(parent);
		}
		else {
			if (oldParent != NoEntity()) {
				ROSE_GETSYSTEM(LevelTree).TrySetParent(entity, oldParent);
			}
		}

	}
	child.UpdateGlobals();
}

#ifdef _DEBUG
DebugDrawTransform& TransformSystem::GetDebugRenderer()
{
	return *debugDrawer;
}
#endif // _DEBUG

DebugDrawTransform::DebugDrawTransform() :matrix(1)
{
	SdlContainer& sdlRenderer = entt::locator<SdlContainer>::value();
	this->renderer = sdlRenderer.GetRenderer();
}
void DebugDrawTransform::SetMatrix(glm::mat3 worldToScreen)
{
	matrix = worldToScreen;
}
void DebugDrawTransform::DrawTransform(const TransformComponent& t, bool selected)
{
	float scale = 1 - (0.2 * t.level);
	auto orig = TransformComponent::GetPosition(t.matrixL2W * matrix, vec2(0, 0));
	auto dir = TransformComponent::GetDir(t.matrixL2W * matrix, vec2(0, 1));
	dir = normalize(dir) * 100.0f;
	auto dest = orig + dir * scale;
	if (selected) {
		filledCircleRGBA(renderer, orig.x, orig.y, 14 * scale, 255, 255, 255, 255);
		thickLineRGBA(renderer, orig.x, orig.y, dest.x, dest.y, 10 * scale, 255, 255, 255, 255);
	}
	else {
		filledCircleRGBA(renderer, orig.x, orig.y, 14 * scale, 20, 100, 30, 200);
		thickLineRGBA(renderer, orig.x, orig.y, dest.x, dest.y, 10 * scale, 20, 100, 30, 200);
	}
}
