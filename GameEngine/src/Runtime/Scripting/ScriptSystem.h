#pragma once
#include <queue>
#include <unordered_map>
#include <sol/sol.hpp>

#include <entt/entt.hpp>

#include "Events/EntityEvent.h"

class ScriptSystem
{
private:
	std::set<entt::entity> setupNextFrame;
	std::unordered_map<entt::entity, std::unordered_map<std::string, sol::state>> scriptStates;
	void ScriptComponentCreated(entt::registry& registry, entt::entity entity);
	void ScriptComponentDestroyed(entt::registry& registry, entt::entity entity);
public:
	ScriptSystem();
	void Update();
	void CallEvent(EntityEvent eventData);
	void AddScript(entt::entity entity, const std::string scriptName, const std::string script);
	void RefreshScript(entt::entity entity);
	void RemoveScript(entt::entity entity, const std::string& removeScript);
};