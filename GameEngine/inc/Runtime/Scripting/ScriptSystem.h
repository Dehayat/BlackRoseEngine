#pragma once
#include <queue>
#include <unordered_map>
#include <sol/sol.hpp>

#include <entt/entt.hpp>

#include "Events/EntityEvent.h"

class ScriptSystem {
private:
	std::queue<entt::entity> setupNextFrame;
	std::unordered_map<entt::entity, sol::state> scriptStates;
public:
	ScriptSystem();
	void ScriptComponentCreated(entt::registry& registry, entt::entity entity);
	void Update();
	void CallEvent(EntityEvent eventData);
};