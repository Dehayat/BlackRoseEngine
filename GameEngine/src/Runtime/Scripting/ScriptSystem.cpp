#include "Scripting/ScriptSystem.h"

#include "Entity.h"

#include "Systems.h"

#include "Components/ScriptComponent.h"

ScriptSystem::ScriptSystem()
{
	entt::registry& registry = GETSYSTEM(Entities).GetRegistry();
	registry.on_construct<ScriptComponent>().connect<&ScriptSystem::ScriptComponentCreated>(this);
}

void ScriptSystem::ScriptComponentCreated(entt::registry& registry, entt::entity entity)
{
	scriptStates[entity] = sol::state();
	auto& state = scriptStates[entity];
	state.open_libraries();
	if (registry.valid(entity) && registry.any_of<ScriptComponent>(entity)) {
		auto& scriptComponent = registry.get<ScriptComponent>(entity);
		if (scriptComponent.scriptFile != "") {
			state.script_file(scriptComponent.scriptFile);
			sol::optional setup = state["setup"];
			if (setup) {
				setupNextFrame.push(entity);
			}
		}
	}
}

void ScriptSystem::Update()
{
	entt::registry& registry = GETSYSTEM(Entities).GetRegistry();
	while (!setupNextFrame.empty()) {
		auto& entity = setupNextFrame.front();
		if (registry.valid(entity) && registry.any_of<ScriptComponent>(entity)) {
			auto& scriptComponent = registry.get<ScriptComponent>(entity);
			auto& state = scriptStates[entity];
			state["setup"]();
		}
		setupNextFrame.pop();
	}
	auto view = registry.view<ScriptComponent>();
	for (auto entity : view) {
		auto& scriptComponent = registry.get<ScriptComponent>(entity);
		if (scriptComponent.scriptFile != "") {
			auto& state = scriptStates[entity];
			sol::optional update = state["update"];
			if (update) {
				update.value()();
			}
		}
	}
}

void ScriptSystem::CallEvent(EntityEvent eventData)
{
	entt::registry& registry = GETSYSTEM(Entities).GetRegistry();
	auto entity = eventData.entity;
	auto& scriptComponent = registry.get<ScriptComponent>(entity);
	if (scriptComponent.scriptFile != "") {
		auto& state = scriptStates[entity];
		sol::optional eventCall = state["on_event"];
		if (eventCall) {
			eventCall.value()(eventData.name);
		}
	}
}
