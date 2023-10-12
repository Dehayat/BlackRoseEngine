#pragma once
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <ryml/ryml.hpp>

#ifdef _EDITOR
#include <imgui.h>
#endif

struct Transform {
	bool hasParent;
	int level;
	entt::entity parent;
	glm::vec2 position;
	glm::vec2 scale;
	float rotation;
	glm::mat3 matrix;
	std::uint64_t parentGUID;
	Transform(glm::vec2 position = glm::vec2(0, 0), glm::vec2 scale = glm::vec2(1, 1), float rotation = 0);
	Transform(ryml::NodeRef node);
	void Serialize(ryml::NodeRef node);
#ifdef _EDITOR
	void SetParent(entt::entity newParent, std::uint64_t guid);
	void DrawEditor() {
		ImGui::DragFloat("Position X", &position.x, 0.2f);
		ImGui::DragFloat("Position y", &position.y, 0.2f);
		ImGui::DragFloat("Scale x", &scale.x, 0.2f);
		ImGui::DragFloat("Scale y", &scale.y, 0.2f);
		ImGui::DragFloat("Rotation", &rotation, 5.f);
	}
#endif
};
class DebugDrawTransform
{
	SDL_Renderer* renderer;
	glm::mat3 matrix;
public:
	DebugDrawTransform(SDL_Renderer* sdl);
	void SetMatrix(glm::mat3 worldToScreen);
	void DrawTransform(const Transform& t);
};

class TransformSystem
{
	DebugDrawTransform* debugDrawer;
	bool drawDebug;
public:
	TransformSystem();
	~TransformSystem();
	void Update(entt::registry& registry);
	void InitLoaded(entt::registry& registry, std::unordered_map < std::uint64_t, entt::entity>& allEntities);
	void InitDebugDrawer(SDL_Renderer* sdl);
	void EnableDebug(bool enable);
	void DebugRender(glm::mat3 viewMatrix, entt::registry& registry);
	void SetParent(entt::registry& registry, Transform& child, entt::entity parent);
#ifdef _EDITOR
	DebugDrawTransform& GetDebugRenderer();
#endif // _EDITOR
};