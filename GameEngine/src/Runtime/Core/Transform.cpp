#include "Transform.h"

#include <SDL2/SDL2_gfxPrimitives.h>

#include "SdlContainer.h"
#include "Entity.h"

#include "Editor/LevelTree.h"

#include "Systems.h"

#include "Debugging/Logger.h"

TransformSystem::TransformSystem()
{
	debugDrawer = nullptr;
	drawDebug = false;

	entt::registry& registry = GETSYSTEM(Entities).GetRegistry();
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
	auto& entities = GETSYSTEM(Entities);
	GETSYSTEM(LevelTree).InsertEntity(entity);
	UpdateGlobals(trx);
}
void TransformSystem::TransformDestroyed(entt::registry& registry, entt::entity entity)
{
	auto& trx = registry.get<TransformComponent>(entity);
	GETSYSTEM(LevelTree).TransformDestroyed(registry, entity);
}
void TransformSystem::BakeTransform(TransformComponent& trx)
{
	auto pos = glm::vec3(0, 0, 1);
	auto bakedPos = pos * trx.matrixL2W;
	trx.position.x = bakedPos.x;
	trx.position.y = bakedPos.y;

	auto bakedScale = glm::vec2(glm::sqrt(trx.matrixL2W[0][0] * trx.matrixL2W[0][0] + trx.matrixL2W[1][0] * trx.matrixL2W[1][0]), glm::sqrt(trx.matrixL2W[0][1] * trx.matrixL2W[0][1] + trx.matrixL2W[1][1] * trx.matrixL2W[1][1]));

	trx.scale.x = bakedScale.x;
	trx.scale.y = bakedScale.y;
	auto bakedRotation = glm::degrees(std::atan2f(trx.matrixL2W[1][0], trx.matrixL2W[0][0]));
	trx.rotation = bakedRotation;
	trx.matrixL2W = CalcMatrixL2W(trx);
}
void TransformSystem::MoveTransformToParentSpace(TransformComponent& child, TransformComponent& parent)
{
	auto matrixW2sL = glm::inverse(parent.matrixL2W);

	auto matrixtL2sL = child.matrixL2W * matrixW2sL;

	auto pos = glm::vec3(0, 0, 1);
	auto extractedPos = pos * matrixtL2sL;
	child.position.x = extractedPos.x;
	child.position.y = extractedPos.y;

	auto extractedScale = glm::vec2(glm::sqrt(matrixtL2sL[0][0] * matrixtL2sL[0][0] + matrixtL2sL[1][0] * matrixtL2sL[1][0]), glm::sqrt(matrixtL2sL[0][1] * matrixtL2sL[0][1] + matrixtL2sL[1][1] * matrixtL2sL[1][1]));

	child.scale.x = extractedScale.x;
	child.scale.y = extractedScale.y;
	auto extractedRotation = glm::degrees(std::atan2f(matrixtL2sL[1][0], matrixtL2sL[0][0]));
	child.rotation = extractedRotation;
}
void TransformSystem::Update()
{
	entt::registry& registry = GETSYSTEM(Entities).GetRegistry();

	registry.sort<TransformComponent>([](const auto& lhs, const auto& rhs) {
		return lhs.level < rhs.level;
		});
	auto view3 = registry.view<TransformComponent>();
	//Logger::Log("Frame");
	for (auto entity : view3) {
		auto& trx = view3.get<TransformComponent>(entity);
		trx.matrixL2W = CalcMatrixL2W(trx);
		UpdateGlobals(trx);
	}
}

void TransformSystem::UpdateGlobals(TransformComponent& trx)
{
	auto pos = glm::vec3(0, 0, 1);
	auto globalPos = pos * trx.matrixL2W;
	trx.globalPosition.x = globalPos.x;
	trx.globalPosition.y = globalPos.y;

	auto globalScale = glm::vec2(glm::sqrt(trx.matrixL2W[0][0] * trx.matrixL2W[0][0] + trx.matrixL2W[1][0] * trx.matrixL2W[1][0]), glm::sqrt(trx.matrixL2W[0][1] * trx.matrixL2W[0][1] + trx.matrixL2W[1][1] * trx.matrixL2W[1][1]));

	trx.globalScale.x = globalScale.x;
	trx.globalScale.y = globalScale.y;
	auto globalRotation = glm::degrees(std::atan2f(trx.matrixL2W[1][0], trx.matrixL2W[0][0]));
	trx.globalRotation = globalRotation;
}

glm::mat3 TransformSystem::CalcMatrix(TransformComponent& trx)
{
	return glm::mat3(
		glm::cos(glm::radians(trx.rotation)) * trx.scale.x, -glm::sin(glm::radians(trx.rotation)) * trx.scale.y, trx.position.x,
		glm::sin(glm::radians(trx.rotation)) * trx.scale.x, glm::cos(glm::radians(trx.rotation)) * trx.scale.y, trx.position.y,
		0, 0, 1
	);
}

glm::mat3 TransformSystem::CalcMatrixL2W(TransformComponent& trx)
{
	//Logger::Log(std::to_string(trx.level));
	auto matrixL2P = glm::mat3(
		glm::cos(glm::radians(trx.rotation)) * trx.scale.x, -glm::sin(glm::radians(trx.rotation)) * trx.scale.y, trx.position.x,
		glm::sin(glm::radians(trx.rotation)) * trx.scale.x, glm::cos(glm::radians(trx.rotation)) * trx.scale.y, trx.position.y,
		0, 0, 1
	);
	if (trx.hasParent) {
		entt::registry& registry = GETSYSTEM(Entities).GetRegistry();
		auto matrixP2W = registry.get<TransformComponent>(trx.parent).matrixL2W;
		return matrixL2P * matrixP2W;
	}
	else {
		return matrixL2P;
	}
}

void TransformSystem::InitDebugDrawer()
{
	SdlContainer& sdlRenderer = entt::locator<SdlContainer>::value();
	debugDrawer = new DebugDrawTransform();
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
	Entities& entities = GETSYSTEM(Entities);
	entt::registry& registry = entities.GetRegistry();
	auto& child = registry.get<TransformComponent>(entity);
	if (child.hasParent) {
		GETSYSTEM(LevelTree).RemoveParent(entity);
		child.hasParent = false;
		child.parent = NoEntity();
		child.level = 0;
	}

	if (parent != NoEntity()) {
		GETSYSTEM(LevelTree).SetParent(entity, parent);
		child.parent = parent;
		auto& parentTrx = registry.get<TransformComponent>(parent);
		child.level = parentTrx.level + 1;
		child.hasParent = true;
		child.parentGUID = entities.GetEntityGuid(parent);
	}
	UpdateGlobals(child);
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
	float scale = 1;
	glm::vec3 orig = glm::vec3(0, 0, 1);
	glm::vec3 dest = glm::vec3(0, 0.6f * scale, 1);
	orig = orig * t.matrixL2W * matrix;
	dest = dest * t.matrixL2W * matrix;
	if (selected) {
		filledCircleRGBA(renderer, orig.x, orig.y, 10 * scale, 20, 30, 130, 255);
		thickLineRGBA(renderer, orig.x, orig.y, dest.x, dest.y, 3, 20, 300, 130, 255);
	}
	else {
		filledCircleRGBA(renderer, orig.x, orig.y, 10 * scale, 20, 100, 30, 255);
		thickLineRGBA(renderer, orig.x, orig.y, dest.x, dest.y, 3, 20, 100, 30, 255);
	}
}
