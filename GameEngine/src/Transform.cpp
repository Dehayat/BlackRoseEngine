#include "Transform.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include "Logger.h"

Transform::Transform(glm::vec2 position, glm::vec2 scale, float rotation) {
	this->position = position;
	this->scale = scale;
	this->rotation = rotation;
	this->matrix = glm::mat3(0);
	this->hasParent = false;
	this->parent = entt::entity(-1);
	this->level = 0;
	this->parentGUID = -1;
}

void Transform::Serialize(ryml::NodeRef node)
{
	node |= ryml::MAP;
	auto posNode = node.append_child();
	posNode.set_key("position");
	posNode |= ryml::SEQ;
	auto xNode = posNode.append_child();
	xNode << position.x;
	auto yNode = posNode.append_child();
	yNode << position.y;
	if (hasParent) {
		auto parentNode = node.append_child();
		parentNode.set_key("parent");
		node["parent"] << parentGUID;
	}

}

void Transform::SetParent(entt::entity newParent)
{
	parent = newParent;
	if (newParent == entt::entity(-1)) {
		hasParent = false;
		level = 0;
	}
	else {
		hasParent = true;
		level = -1;
	}
	Logger::Log("Not updating parent guid or carrying over global transform");
}


Transform::Transform(ryml::NodeRef node) {
	this->position = glm::vec2(0, 0);
	this->scale = glm::vec2(1, 1);
	this->rotation = 0;
	this->matrix = glm::mat3(0);
	this->hasParent = false;
	this->parent = entt::entity(-1);
	this->level = 0;
	this->parentGUID = 0;

	if (node.has_child("position")) {
		node["position"][0] >> position.x;
		node["position"][1] >> position.y;
	}

	if (node.has_child("parent")) {
		hasParent = true;
		node["parent"] >> parentGUID;
		level = -1;
	}
}


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
#ifdef _EDITOR
	for (auto entity : view3) {
		auto& trx = view3.get<Transform>(entity);
		if (trx.level == -1) {
			SetParent(registry, trx, trx.parent);
		}
	}
#endif // _EDITOR

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
			auto& parentTrx = registry.get<Transform>(trx.parent);
			trx.matrix = trx.matrix * parentTrx.matrix;
		}
	}
}
int InitParentRecursive(entt::registry& registry, std::unordered_map<std::uint64_t, entt::entity>& allEntities, entt::entity parent) {
	auto& pos = registry.get<Transform>(parent);
	if (pos.hasParent && pos.level == -1) {
		pos.parent = allEntities[pos.parentGUID];
		pos.level = InitParentRecursive(registry, allEntities, pos.parent) + 1;
	}
	return pos.level;
}
void TransformSystem::InitLoaded(entt::registry& registry, std::unordered_map<std::uint64_t, entt::entity>& allEntities)
{
	auto view = registry.view<Transform>();
	for (auto entity : view) {
		auto& pos = view.get<Transform>(entity);
		if (pos.hasParent && pos.level == -1) {
			pos.parent = allEntities[pos.parentGUID];
			pos.level = InitParentRecursive(registry, allEntities, pos.parent) + 1;
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


#ifdef _EDITOR
DebugDrawTransform& TransformSystem::GetDebugRenderer()
{
	return *debugDrawer;
}
#endif // _EDITOR

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
