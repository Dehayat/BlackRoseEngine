#pragma once
#include <queue>

#include <entt/entt.hpp>

class ScriptSystem {
private:
	std::queue<entt::entity> newlyAddedScripts;
public:
	ScriptSystem();
	void ScriptComponentCreated(entt::registry& registry, entt::entity entity);
	void Update();
};