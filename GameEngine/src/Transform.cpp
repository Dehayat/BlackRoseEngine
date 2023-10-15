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
void TransformSystem::TransformCreated(entt::registry& registry, entt::entity entity)
{
	auto& trx = registry.get<Transform>(entity);
	trx.matrix = CalcMatrix(trx);
	if (trx.parent) {
		if (registry.valid(trx.parent.value())) {
			auto& parentTrx = registry.get<Transform>(trx.parent.value());
			trx.parent = trx.parent.value();
			trx.level = trx.level + 1;
			trx.matrix = trx.matrix * parentTrx.matrix;
		}
		else {
			trx.parent = std::nullopt;
			trx.level = 0;
		}
	}
}
void TransformSystem::Update(entt::registry& registry)
{

	auto view3 = registry.view<Transform>();

	registry.sort<Transform>([](const auto& lhs, const auto& rhs) {
		return lhs.level < rhs.level;
		});
	for (auto entity : view3) {
		auto& trx = view3.get<Transform>(entity);
		trx.matrix = glm::mat3(
			glm::cos(glm::radians(trx.rotation)) * trx.scale.x, -glm::sin(glm::radians(trx.rotation)) * trx.scale.y, trx.position.x,
			glm::sin(glm::radians(trx.rotation)) * trx.scale.x, glm::cos(glm::radians(trx.rotation)) * trx.scale.y, trx.position.y,
			0, 0, 1
		);

		if (trx.hasParent) {
			auto& parentTrx = registry.get<Transform>(trx.parent.value());
			trx.matrix = trx.matrix * parentTrx.matrix;
		}
	}
}
int InitParentRecursive(entt::registry& registry, std::unordered_map<std::uint64_t, entt::entity>& allEntities, entt::entity parent) {
	auto& trx = registry.get<Transform>(parent);
	if (trx.hasParent && trx.level == -1) {
		if (allEntities.find(trx.parentGUID) != allEntities.end()) {
			trx.parent = allEntities[trx.parentGUID];
			trx.level = InitParentRecursive(registry, allEntities, trx.parent.value()) + 1;
		}
		else {
			trx.hasParent = false;
			trx.level = 0;
		}
	}
	return trx.level;
}
void TransformSystem::InitLoaded(entt::registry& registry, std::unordered_map<std::uint64_t, entt::entity>& allEntities)
{
	auto view = registry.view<Transform>();
	for (auto entity : view) {
		auto& trx = view.get<Transform>(entity);
		if (trx.hasParent && !trx.parent) {
			if (allEntities.find(trx.parentGUID) != allEntities.end()) {
				trx.parent = allEntities[trx.parentGUID];
				trx.level = InitParentRecursive(registry, allEntities, trx.parent.value()) + 1;
			}
			else {
				trx.hasParent = false;
				trx.level = 0;
			}
		}
	}
}
glm::mat3 TransformSystem::CalcMatrix(Transform& trx)
{
	return glm::mat3(
		glm::cos(glm::radians(trx.rotation)) * trx.scale.x, -glm::sin(glm::radians(trx.rotation)) * trx.scale.y, trx.position.x,
		glm::sin(glm::radians(trx.rotation)) * trx.scale.x, glm::cos(glm::radians(trx.rotation)) * trx.scale.y, trx.position.y,
		0, 0, 1
	);
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



#ifdef _DEBUG
DebugDrawTransform& TransformSystem::GetDebugRenderer()
{
	return *debugDrawer;
}
#endif // _DEBUG

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
	float scale = (4.0 - t.level) / 4.0;
	glm::vec3 orig = glm::vec3(0, 0, 1);
	glm::vec3 dest = glm::vec3(0, 0.6f * scale, 1);
	orig = orig * t.matrix * matrix;
	dest = dest * t.matrix * matrix;
	filledCircleRGBA(renderer, orig.x, orig.y, 10 * scale, 20, 100, 30, 255);
	thickLineRGBA(renderer, orig.x, orig.y, dest.x, dest.y, 3, 20, 100, 30, 255);
}
